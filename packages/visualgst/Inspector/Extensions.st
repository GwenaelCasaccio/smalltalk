"======================================================================
|
| Extensions
|
======================================================================"

"======================================================================
|
| Copyright (c) 2013
| Gwenael Casaccio <gwenael.casaccio@gmail.com>,
|
|
| This file is part of VisualGST.
|
| Permission is hereby granted, free of charge, to any person obtaining
| a copy of this software and associated documentation files (the
| 'Software'), to deal in the Software without restriction, including
| without limitation the rights to use, copy, modify, merge, publish,
| distribute, sublicense, and/or sell copies of the Software, and to
| permit persons to whom the Software is furnished to do so, subject to
| the following conditions:
|
| The above copyright notice and this permission notice shall be
| included in all copies or substantial portions of the Software.
|
| THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
| EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
| MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
| IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
| CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
| TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
| SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
|
======================================================================"

Object extend [

    gtkInspect [
	<category: '*VisualGST'>

	VisualGST.GtkInspector openOn: self
    ]

    inspectorView [
	<category: '*VisualGST'>

	^ VisualGST.GtkObjectInspectorView
    ]
]

CompiledMethod extend [

    inspectorView [
        <category: '*VisualGST'>

        ^ VisualGST.GtkCompiledMethodInspectorView
    ]
]

CompiledBlock extend [

    inspectorView [
        <category: '*VisualGST'>

        ^ VisualGST.GtkCompiledBlockInspectorView
    ]
]

SequenceableCollection extend [

    inspectorView [
        <category: '*VisualGST'>

        ^ VisualGST.GtkSequenceableCollectionInspectorView
    ]
]

Set extend [

    inspectorView [
        <category: '*VisualGST'>

        ^ VisualGST.GtkSetInspectorView
    ]
]

Dictionary extend [

    inspectorView [
        <category: '*VisualGST'>

        ^ VisualGST.GtkDictionaryInspectorView
    ]
]

Character extend [

    inspectorView [
        <category: '*VisualGST'>

        ^ VisualGST.GtkCharacterInspectorView
    ]
]

Integer extend [

    inspectorView [
        <category: '*VisualGST'>

        ^ VisualGST.GtkIntegerInspectorView
    ]
]

Float extend [

    inspectorView [
        <category: '*VisualGST'>

        ^ VisualGST.GtkFloatInspectorView
    ]
]

ContextPart extend [

    inspectorView [
        <category: '*VisualGST'>

        ^ VisualGST.GtkContextInspectorView
    ]

    stackInspectorView [
        <category: '*VisualGST'>

        ^ VisualGST.GtkStackInspectorView
    ]
]

