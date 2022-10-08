Object subclass: KeyHash [

    <comment: 'I am the keys used by the Sip Hashing function.
               Both keys should be SmallInteger and do not forget
               if you update the keys the dictionaries, set, the
               symbol literals, ... should be rehashed.
               The format of the class could not be changed the hash primitive
               expect two slots with both integers objects.'>
    <category: 'Language-Implementation'>

    | key0 key1 |

    KeyHash class >> key: aKey0SmallInteger and: aKey1SmallInteger [

        ^ self new key: aKey0SmallInteger and: aKey1SmallInteger;
                   yourself
    ]

    key: aKey0SmallInteger and: aKey1SmallInteger [
        <category: 'initialization'>

        aKey0SmallInteger isInteger ifFalse: [ ^ SystemExceptions.WrongClass signalOn: aKey0SmallInteger
                                                                             mustBe: SmallInteger ].
        aKey1SmallInteger isInteger ifFalse: [ ^ SystemExceptions.WrongClass signalOn: aKey0SmallInteger
                                                                             mustBe: SmallInteger ].
        key0 := aKey0SmallInteger.
        key1 := aKey1SmallInteger.
        self makeReadOnly: true.
    ]
]

