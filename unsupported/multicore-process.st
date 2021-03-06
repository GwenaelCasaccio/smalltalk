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
  processList := Array new: 9.
  1 to: 9 do: [ : i | processList at: i put: Semaphore new ].

  ps := ProcessorScheduler basicNew.

  process := Process basicNew.
  process setBlock: [ 100 timesRepeat: [ #[ 1 2 3 ] do: [ : i | i + 2 ] ]. 2+2. Array new: 10. 1 basicPrint.  [100 timesRepeat: [ #[ 1 2 3 ] do: [ : i | i + 2 ] ]. 2+2. Array new: 10. 1 basicPrint. [100 timesRepeat: [ #[ 1 2 3 ] do: [ : i | i + 2 ] ]. 2+2. Array new: 10. 1 basicPrint. [1 timesRepeat: [ #[ 1 2 3 ] do: [ : i | i + 2 ] ]. 2+2. Array new: 10. 1 basicPrint. [1 timesRepeat: [ #[ 1 2 3 ] do: [ : i | i + 2 ] ]. 2+2. Array new: 10. 1 basicPrint. ] forkWithoutFinalize ] forkWithoutFinalize ] forkWithoutFinalize ] forkWithoutFinalize. ps killThread ].
  process instVarNamed: #priority put: 4.
  process instVarNamed: #name put: 'Threaded Process 1'.

  (processList at: 4) addFirst: process.

  " GC Protecting the new Processor "
  Smalltalk at: #Foo put: ps.

  ps instVarAt:1 put: processList.
  "ps instVarAt:2 put: process. it has no active process "
  ps instVarAt:3 put: OrderedCollection new.
  ps instVarAt:4 put: 40.
  ps instVarAt:5 put: Semaphore new. 
  ps newThread: process.

  "1 to: 2 do: [ : i | 1 timesRepeat: [ #[ 1 2 3 ] do: [ : i | i + 2 ] ]. 2+2. Array new: 10. 1 basicPrint. ObjectMemory scavenge ].
  ObjectMemory abort."
  [ true ] whileTrue.
]
