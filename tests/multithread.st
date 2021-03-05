Eval [
  | ps1 ps2 ps3 ps4 |

  ps1 := ProcessorScheduler newFromClosure: [
         'ok' basicPrint.
         ps1 killThread ].

  " GC Protecting the new Processor "
  Smalltalk at: #Foo1 put: ps1.

  ps2 := ProcessorScheduler newFromClosure: [
         'ok' basicPrint.
          ps2 killThread ].

  Smalltalk at: #Foo2 put: ps2.

  ps3 := ProcessorScheduler newFromClosure: [
         'ok' basicPrint.
          ps3 killThread ].

  Smalltalk at: #Foo3 put: ps3.

  ps4 := ProcessorScheduler newFromClosure: [
         'ok' basicPrint.
          ps4 killThread ].

  Smalltalk at: #Foo4 put: ps4.

  ps1 newThread.
  ps2 newThread.
  ps3 newThread.
  ps4 newThread.

  (Delay forSeconds: 10) wait.
]

Eval [
  | ps1 processLists process |

  ps1 := ProcessorScheduler newFromClosure: [
         'ok' basicPrint.
         ps1 killThread ].

  processLists := ps1 instVarNamed: #processLists.
  (processLists class = Array) basicPrint.
  (processLists size = 9) basicPrint.
  (processLists allSatisfy: [ : aSemaphore | aSemaphore class = Semaphore ]) basicPrint.
  (ps1 instVarNamed: #activeProcess) isNil basicPrint.
  (ps1 instVarNamed: #idleTasks) isEmpty basicPrint.
  ((ps1 instVarNamed: #processTimeslice) = 40) basicPrint.
  ((ps1 instVarNamed: #gcSemaphore) class = Semaphore) basicPrint.

  process := (processLists at: 4) first.
  (process instVarNamed: #suspendedContext) isNil not basicPrint.
  ((process instVarNamed: #priority) = 4) basicPrint.
  ((process instVarNamed: #myList) = (processLists at: 4)) basicPrint.
]

