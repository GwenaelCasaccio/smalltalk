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

TestCase subclass: TestKeyHash [

    testKeyHashFormat [

        self assert: KeyHash allInstVarNames size = 2.
    ]

    testKeyHashCannotChangeClass [

        self should: [ KeyHash addInstVarNames: #'aValidName' ] raise: Exception.
    ]

    testKeyHashCannotBeInstanciatedWithoutIntegers [

        self should: [ KeyHash key: $a and: 1 ] raise: Exception.
        self should: [ KeyHash key: 1 and: $a ] raise: Exception.
        self should: [ KeyHash key: $a and: $a ] raise: Exception.
    ] 

    testKeyHashInstanciatedWithIntegers [

        | key |
        key := KeyHash key: 12 and: 23.
        self assert: (key instVarAt: 1) = 12.
        self assert: (key instVarAt: 2) = 23.
        self assert: key isReadOnly.
    ]

    testKeyHashBootstrap [

        self assert: SipHashKey class = KeyHash.
        self assert: (SipHashKey instVarAt: 1) isInteger.
        self assert: (SipHashKey instVarAt: 2) isInteger.
        self assert: SipHashKey isReadOnly.
    ]
]

