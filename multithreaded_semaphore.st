Eval [
  | semaphore ps1 ps2 ps3 ps4 |

  semaphore := Semaphore new.

  ps1 := ProcessorScheduler newFromClosure: [
         'ok' basicPrint.
         [
           'ok1' basicPrint.
           semaphore wait.
           'ok3' basicPrint.
         ] forkWithoutFinalize.
         'ok2' basicPrint.
         [ true ] whileTrue ].

  " GC Protecting the new Processor "
  Smalltalk at: #Foo1 put: ps1.

  ps2 := ProcessorScheduler newFromClosure: [
         'ok' basicPrint.
         [
           'ok1' basicPrint.
           semaphore wait.
           'ok3' basicPrint.
         ] forkWithoutFinalize.
         'ok2' basicPrint.
         [ true ] whileTrue ].

  Smalltalk at: #Foo2 put: ps2.

  ps3 := ProcessorScheduler newFromClosure: [
         'ok' basicPrint.
         [
           'ok1' basicPrint.
           semaphore wait.
           'ok3' basicPrint.
         ] forkWithoutFinalize.
         'ok2' basicPrint.
         [ true ] whileTrue ].

  Smalltalk at: #Foo3 put: ps3.

  ps4 := ProcessorScheduler newFromClosure: [
         'ok' basicPrint.
         [
           'ok1' basicPrint.
           semaphore wait.
           'ok3' basicPrint.
         ] forkWithoutFinalize.
         'ok2' basicPrint.
         [ true ] whileTrue ].

  Smalltalk at: #Foo4 put: ps4.


  ps1 newThread.
  ps2 newThread.
  ps3 newThread.
  ps4 newThread.

  'Delay ok' basicPrint.

  4 timesRepeat: [ semaphore signal ].

  [ true ] whileTrue
]

