"******************************************************************************
* Copyright (C) 2022 GST.  All rights reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining
* a copy of this software and associated documentation files (the
* ""Software""), to deal in the Software without restriction, including
* without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to
* permit persons to whom the Software is furnished to do so, subject to
* the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED ""AS IS"", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************"

Eval [
    Class initialize.
    Character initialize.
    Boolean initialize.
	  Object initialize.
	  Fraction initialize.
	  LargeInteger initialize.
	  Date initialize.
	  Time initialize.
	  FileDescriptor initialize.
	  Namespace initialize.
	  Processor initialize.
	  SystemDictionary initialize.

    " Ne Pas Oublier dans MEtaclass l'eval "

    " The class is made read-only to forbid class migration (adding or removing instance variable, ...)
      The virtual machine expect two integer instance variables. "
    KeyHash makeReadOnly: true.
]

Stream extend [
    nextLine [
        "Returns a collection of the same type that the stream accesses, containing
         the next line up to the next new-line character.  Returns the entire rest of the
         stream's contents if no new-line character is found."

        <category: 'accessing-reading'>
        | next ws |
        ws := WriteStream on: (self species new: 40).
	
        [ self atEnd or: [ (next := self next) == ##(Character cr) or: [ next == ##(Character nl) or: [ next isNil ] ] ] ] whileFalse: [ ws nextPut: next ].
        next == ##(Character cr) ifTrue: [ self peekFor: ##(Character nl) ].
        ^ ws contents
    ]

]

