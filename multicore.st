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
    suspendedContext := BlockContext allocateFrom: [ 
      1 to: 10000 do: [ : i | aBlock value ].
      1 to: 200 do: [ : i | Object new basicPrint].
      ObjectMemory scavenge.
      'out' basicPrint.
      ObjectMemory abort ].

  ]
]

Eval [
  processList := Array new: 9.
  1 to: 9 do: [ : i | processList at: i put: Semaphore new ].

  process := Process basicNew.
  process instVarAt: 1 put: process.
  process setBlock: [ Object new. ObjectMemory scavenge ].
  process instVarAt: 3 put: 4.

  ps := ProcessorScheduler basicNew.
  ps instVarAt:1 put: processList.
  "ps instVarAt:2 put: process. it has no active process "
  ps instVarAt:3 put: OrderedCollection new.
  ps instVarAt:4 put: 40.
  ps instVarAt:5 put: Semaphore new. 
  ps newThread: process.

  " GC Protecting the new Processor "
  Smalltalk at: #Foo put: ps.

  1 to: 10000 do: [ : i | Object new ].
  ObjectMemory abort.
  [ true ] whileTrue.
]
