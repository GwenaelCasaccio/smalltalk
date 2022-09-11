Object subclass: KeyHash [

    | key0 key1 |

    KeyHash class >> key: aKey0SmallInteger and: aKey1SmallInteger [

        ^ self new key: aKey0SmallInteger and: aKey1SmallInteger;
                   yourself
    ]

    key: aKey0SmallInteger and: aKey1SmallInteger [
        <category: 'initialization'>

        key0 := aKey0SmallInteger.
        key1 := aKey1SmallInteger.
    ]
]
