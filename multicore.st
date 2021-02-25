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
      1 to: 200 do: [ : i | aBlock value ].
      ObjectMemory globalGarbageCollect.
      'out' basicPrint.
      ObjectMemory abort ].

  ]
]

Eval [
 processList := Array new: 9.
  1 to: 9 do: [ : i | processList at: i put: Semaphore new ].

  ps := ProcessorScheduler basicNew.

  process := Process basicNew.
  process instVarAt: 1 put: process.
  process setBlock: [ 100 timesRepeat: [ #[ 1 2 3 ] do: [ : i | i + 2 ]. 1+1. Array new: 10. 2 basicPrint. "ObjectMemory scavenge."].  ps killThread ].
  process instVarAt: 3 put: 4.

  " GC Protecting the new Processor "
  Smalltalk at: #Foo put: ps.

  ps instVarAt:1 put: processList.
  "ps instVarAt:2 put: process. it has no active process "
  ps instVarAt:3 put: OrderedCollection new.
  ps instVarAt:4 put: 40.
  ps instVarAt:5 put: Semaphore new. 



 processList2 := Array new: 9.
  1 to: 9 do: [ : i | processList2 at: i put: Semaphore new ].

  ps2 := ProcessorScheduler basicNew.

  process2 := Process basicNew.
  process2 instVarAt: 1 put: process2.
  process2 setBlock: [ 100 timesRepeat: [ #[ 1 2 3 ] do: [ : i | i + 2 ]. 1+1. Array new: 10. 3 basicPrint. "ObjectMemory scavenge."].  ps killThread ].
  process2 instVarAt: 3 put: 4.

  " GC Protecting the new Processor "
  Smalltalk at: #Foo2 put: ps2.

  ps2 instVarAt:1 put: processList2.
  "ps instVarAt:2 put: process. it has no active process "
  ps2 instVarAt:3 put: OrderedCollection new.
  ps2 instVarAt:4 put: 40.
  ps2 instVarAt:5 put: Semaphore new. 


 processList3 := Array new: 9.
  1 to: 9 do: [ : i | processList3 at: i put: Semaphore new ].

  ps3 := ProcessorScheduler basicNew.

  process3 := Process basicNew.
  process3 instVarAt: 1 put: process3.
  process3 setBlock: [ 100 timesRepeat: [ #[ 1 2 3 ] do: [ : i | i + 2 ]. 1+1. Array new: 10. 4 basicPrint. "ObjectMemory scavenge."].  ps killThread ].
  process3 instVarAt: 3 put: 4.

  " GC Protecting the new Processor "
  Smalltalk at: #Foo3 put: ps3.

  ps3 instVarAt:1 put: processList3.
  "ps instVarAt:2 put: process. it has no active process "
  ps3 instVarAt:3 put: OrderedCollection new.
  ps3 instVarAt:4 put: 40.
  ps3 instVarAt:5 put: Semaphore new. 









   processList4 := Array new: 9.
  1 to: 9 do: [ : i | processList4 at: i put: Semaphore new ].

  ps4 := ProcessorScheduler basicNew.

  process4 := Process basicNew.
  process4 instVarAt: 1 put: process4.
  process4 setBlock: [ 100 timesRepeat: [ #[ 1 2 3 ] do: [ : i | i + 2 ]. 1+1. Array new: 10. 5 basicPrint. "ObjectMemory scavenge."].  ps killThread ].
  process3 instVarAt: 3 put: 4.

  " GC Protecting the new Processor "
  Smalltalk at: #Foo4 put: ps4.

  ps4 instVarAt:1 put: processList4.
  "ps instVarAt:2 put: process. it has no active process "
  ps4 instVarAt:3 put: OrderedCollection new.
  ps4 instVarAt:4 put: 40.
  ps4 instVarAt:5 put: Semaphore new. 




  ps newThread: process.
  ps2 newThread: process2.
  ps3 newThread: process3.
  ps4 newThread: process4.
  "Processor tracing."

  1 to: 200 do: [ : i | 100 timesRepeat: [ #[ 1 2 3 ] do: [ : i | i + 2 ] ]. 2+2. Array new: 10. 1 basicPrint. "ObjectMemory scavenge" ].
  "ObjectMemory abort."
  [ true ] whileTrue.
]
