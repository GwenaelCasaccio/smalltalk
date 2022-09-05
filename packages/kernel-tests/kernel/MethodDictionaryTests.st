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

Object subclass: FakeValueForMethodDictionaryTest [

    discardTranslation [
    ]
]

Object subclass: FakeObjectForMethodDictionaryTest [

]

TestCase subclass: TestMethodDictionary [

    instanceCreation [

        | md |
        md := MethodDictionary new.
        self assert: md size equals: 0.
    ]

    instanceCreationWithSize [

        | md |
        md := MethodDictionary new: 100.
        self assert: md size equals: 0.
    ]

    testStructural [

        | md |
        md := MethodDictionary new.
        self assert: (md instVarAt: 1) class equals: SmallInteger. 
        self assert: (md instVarAt: 2) class equals: Array. 
        self assert: (md instVarAt: 3) class equals: Array. 
    ]

    testAtPut [

        | md res key value listOfAssociations |
        listOfAssociations := OrderedCollection new.

        key := FakeObjectForMethodDictionaryTest new.

        md := MethodDictionary new.
        value := FakeValueForMethodDictionaryTest new.
        res := md at: key put: value.
        self assert: res identityEquals: value.
        self assert: md size equals: 1.

        value := FakeValueForMethodDictionaryTest new.
        listOfAssociations add: (Association key: key value: value).

        res := md at: key put: value.
        self assert: res identityEquals: value.
        self assert: md size equals: 1.

        2 to: 1000 do: [ : i |
            key := FakeObjectForMethodDictionaryTest new.
            value := FakeValueForMethodDictionaryTest new.
        
            listOfAssociations add: (Association key: key value: value).
                   
            res := md at: key put: value.
            self assert: res identityEquals: value.
            self assert: md size equals: i ].

        listOfAssociations do: [ : anAssociation |
           self assert: (md at: anAssociation key) identityEquals: anAssociation value ].
    ]

    testAt [

        | md res key value |
        key := FakeObjectForMethodDictionaryTest new.
        value := FakeValueForMethodDictionaryTest new.

        md := MethodDictionary new.
        self should: [ md at: key ] raise: SystemExceptions.NotFound.

        md at: key put: value.

        res := md at: key.
        self assert: res identityEquals: value.

        2 to: 1000 do: [ : i |
            key := FakeObjectForMethodDictionaryTest new.
            value := FakeValueForMethodDictionaryTest new.

            md at: key put: value.
            res := md at: key.
            self assert: res identityEquals: value. ].
    ]

    testCopy [

        | md1 md2 value listOfAssociations |
        listOfAssociations := OrderedCollection new.

        md1 := MethodDictionary new.
        1 to: 1000 do: [ : i |
            | key value |
            key := FakeObjectForMethodDictionaryTest new.
            value := FakeValueForMethodDictionaryTest new.

            listOfAssociations add: (Association key: key value: value).

            md1 at: key put: value ].

        md2 := md1 copy.

        self deny: md1 identityEquals: md2.
        self assert: md1 size equals: listOfAssociations size.
        self assert: md1 size equals: md2 size.
        listOfAssociations do: [ : anAssociation |
            self assert: (md2 at: anAssociation key) identityEquals: anAssociation value ].

        value := FakeValueForMethodDictionaryTest new.

        md2 at: listOfAssociations first key put: value.

        " a change in one dictionary doesn't impact the other dictionary "
        self deny: (md1 at: listOfAssociations first key) identityEquals: (md2 at: listOfAssociations first key).

        value := FakeValueForMethodDictionaryTest new.

        md1 at: listOfAssociations first key put: value.
 
        " a change in one dictionary doesn't impact the other dictionary "
        self deny: (md1 at: listOfAssociations first key) identityEquals: (md2 at: listOfAssociations first key).
    ]

    testCollect [

        | md1 reverseMd1 md2 value listOfAssociations listOfAssociations2 |
        listOfAssociations := OrderedCollection new.
        listOfAssociations2 := OrderedCollection new.

        md1 := MethodDictionary new.
        reverseMd1 := IdentityDictionary new.

        1 to: 1000 do: [ : i |
            | key value |
            key := FakeObjectForMethodDictionaryTest new.
            value := FakeValueForMethodDictionaryTest new.

            listOfAssociations add: (Association key: key value: value).

            md1 at: key put: value.
            reverseMd1 at: value put: key ].

        md2 := md1 collect: [ : aValue |
            | newValue |
            newValue := FakeValueForMethodDictionaryTest new.

            listOfAssociations2 add: (Association key: (reverseMd1 at: aValue) value: newValue).

            newValue ].

        self assert: md1 size equals: listOfAssociations size.
        listOfAssociations do: [ : anAssociation |
            self assert: (md1 at: anAssociation key) identityEquals: anAssociation value ].

        self assert: md2 size equals: listOfAssociations2 size.
        listOfAssociations2 do: [ : anAssociation |
            self assert: (md2 at: anAssociation key) identityEquals: anAssociation value ].
    ]

]
