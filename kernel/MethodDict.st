"======================================================================
| MIT License
|
| Copyright (c) 2022 GST
|
| Permission is hereby granted, free of charge, to any person obtaining a copy
| of this software and associated documentation files (the ""Software""), to deal
| in the Software without restriction, including without limitation the rights
| to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
| copies of the Software, and to permit persons to whom the Software is
| furnished to do so, subject to the following conditions:
|
| The above copyright notice and this permission notice shall be included in all
| copies or substantial portions of the Software.
|
| THE SOFTWARE IS PROVIDED ""AS IS"", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
| IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
| FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
| AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
| LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
| OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
| SOFTWARE.
======================================================================"

IdentityDictionary subclass: MethodDictionary [

    | keys values |

    <shape: nil>

    <category: 'Language-Implementation'>

    MethodDictionary class >> primNew: anInteger [
        <category: 'instance creation'>

        ^ self basicNew initialize: anInteger;
                        yourself
    ]

    initialize: anInteger [
        <category: 'initialization'>

        super initialize: anInteger.
        keys := Array new: anInteger.
        values := Array new: anInteger.
    ]

    postCopy [
        <category: 'copying'>

        keys := keys copy.
        values := values copy.
    ]

    deepCopy [
        <category: 'copying'>

         ^ self copy
    ]

    primSize [
        <category: 'private-accessing'>

        ^ keys size
    ]

    primAt: anInteger [
        <category: 'private-accessing'>

        ^ keys at: anInteger
    ]

    primAt: anInteger put: anObject [

        ^ keys at: anInteger put: anObject
    ]

    valueAt: anInteger [
        <category: 'private-accessing'>

        ^ values at: anInteger
    ]

    valueAt: anInteger put: anObject [
        <category: 'private-accessing'>

        ^ values at: anInteger put: anObject
    ]

    at: key put: value [
        "Store value as associated to the given key"

        <category: 'adding'>
        | index |
        index := self findIndex: key.
        (self primAt: index) isNil
            ifTrue: [
                self incrementTally ifTrue: [ index := self findIndex: key ].
                " This is a VM specific implementation detail:
                  by inserting the value first, we are resiliant to multi threading issue
                  a method is inserted before the symbol and is only available when the symbol is present.
                  otherway the lookup return nil and send a message to it (object as message).

                  Adding a mutex protect the smalltalk part but we should be carreful while playing with the method dictionary "
                self valueAt: index put: value.
                self primAt: index put: key ]
            ifFalse: [
                (self valueAt: index) discardTranslation.
                self valueAt: index put: value ].
        " The cache is flushed whether a new method is added or a previous one is replaced.
          It's done this way because the virtual machine cache hash is computed with the symbol
          and the receiver class - and a new method can be added while a cache entry exists for that
          symbol with the receiver's class "
        Behavior flushCache. 
        ^ value
    ]

    remove: anAssociation [
        "The interpreter might be using this MethodDictionary while
	       this method is running!!  Therefore we perform the removal
         in a copy, and then atomically become that copy"

        <category: 'removing'>
        | copy result |
        (self includesKey: anAssociation key) ifFalse: [ SystemExceptions.NotFound signalOn: anAssociation key what: 'key' ].
        copy := self copy.
        result := copy dangerouslyRemove: anAssociation.
        self become: copy.
        Behavior flushCache.
        ^ result
    ]

    removeKey: anElement ifAbsent: aBlock [
        "The interpreter might be using this MethodDictionary while
	       this method is running!!  Therefore we perform the removal
         in a copy, and then atomically become that copy"

       <category: 'removing'>
       | copy result |
       (self includesKey: anElement) ifFalse: [ ^ aBlock value ].
       copy := self copy.
       result := copy dangerouslyRemoveKey: anElement.
       self become: copy.
       Behavior flushCache.
       ^ result
    ]

    select: aBlock [
        "Answer a new dictionary containing the key/value pairs for which aBlock
        returns true. aBlock only receives the value part of the pairs."

        <category: 'dictionary enumerating'>
        | newDict |
        newDict := self copyEmpty: self capacity.
        self
            associationsDo: [:assoc |
                (aBlock value: assoc value) ifTrue: [ newDict dangerouslyAt: assoc key put: assoc value ] ].
        ^ newDict
    ]

    dangerouslyAt: aKey put: aValue [
        <category: 'private methods'>

        | index |
        index := self findIndex: aKey.
        (self primAt: index) isNil
            ifTrue: [
                self incrementTally
                    ifTrue: [ index := self findIndex: aKey ].
                self primAt: index put: aKey].
        self valueAt: index put: aValue.
       ^ aValue
    ]

    dangerouslyRemove: anAssociation [
	      "This is not really dangerous.  But if normal removal
	       were done WHILE a MethodDictionary were being used, the
	       system might crash.  So instead we make a copy, then do
	       this operation (which is NOT dangerous in a copy that is
	       not being used), and then use the copy after the removal."

	      <category: 'private methods'>
	      | association |
	      association := super remove: anAssociation.
	      association value discardTranslation.
	      ^ association
    ]

    dangerouslyRemoveKey: anElement [
        "This is not really dangerous.  But if normal removal
	       were done WHILE a MethodDictionary were being used, the
	       system might crash.  So instead we make a copy, then do
	       this operation (which is NOT dangerous in a copy that is
	       not being used), and then use the copy after the removal."

	      <category: 'private methods'>
        | value |
        value := super removeKey: anElement ifAbsent: [ self error: 'synchronization problem?' ].
        value discardTranslation.
        ^ value
    ]
]

