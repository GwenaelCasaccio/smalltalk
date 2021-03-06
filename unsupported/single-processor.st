BlockContext class extend [

  allocateFrom: aBlockClosure [

    | context |
    context := BlockContext basicNew: 100.
    context instVarNamed: #outerContext put: aBlockClosure outerContext.
    context instVarNamed: #parent put: nil.
    context instVarNamed: #nativeIP put: 0.
    context instVarNamed: #ip put: 0.
    context instVarNamed: #sp put: -1.
    context instVarNamed: #receiver put: aBlockClosure receiver.
    context instVarNamed: #method put: (aBlockClosure block).
    ^ context
  ]
]

Process extend [
  setBlock: aBlock [
    suspendedContext := BlockContext allocateFrom: [ aBlock value ].
  ]
]

Eval [
     Processor tracing.
  [] forkWithoutFinalize.
  ObjectMemory abort. 
]
