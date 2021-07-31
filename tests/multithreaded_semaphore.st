Eval [
  | semaphore ps1 ps2 ps3 ps4 state |

  semaphore := Semaphore new.
  state := Array new: 4.
  state atAllPut: 0.

  ps1 := ProcessorScheduler newFromClosure: [
         [
           state at: 1 put: (state at: 1) + 1.
           semaphore wait.
           state at: 1 put: (state at: 1) + 1.
         ] forkWithoutFinalize.
         [ (state at: 1) = 1 ] whileTrue.
         state at: 1 put: (state at: 1) + 1.
         ps1 killThread ].

  " GC Protecting the new Processor "
  Smalltalk at: #Foo1 put: ps1.

  ps2 := ProcessorScheduler newFromClosure: [
         [
           state at: 2 put: (state at: 2) + 1.
           semaphore wait.
           state at: 2 put: (state at: 2) + 1.
         ] forkWithoutFinalize.
         [ (state at: 2) = 1 ] whileTrue.
         state at: 2 put: (state at: 2) + 1.
         ps2 killThread ].

  Smalltalk at: #Foo2 put: ps2.

  ps3 := ProcessorScheduler newFromClosure: [
         [
           state at: 3 put: (state at: 3) + 1.
           semaphore wait.
           state at: 3 put: (state at: 3) + 1.
         ] forkWithoutFinalize.
         [ (state at: 3) = 1 ] whileTrue.
         state at: 3 put: (state at: 3) + 1.
         ps3 killThread ].

  Smalltalk at: #Foo3 put: ps3.

  ps4 := ProcessorScheduler newFromClosure: [
         [
           state at: 4 put: (state at: 4) + 1.
           semaphore wait.
           state at: 4 put: (state at: 4) + 1.
         ] forkWithoutFinalize.
         [ (state at: 3) = 1 ] whileTrue.
         state at: 4 put: (state at: 4) + 1.
         ps4 killThread ].

  Smalltalk at: #Foo4 put: ps4.

  ps1 newThread.
  ps2 newThread.
  ps3 newThread.
  ps4 newThread.

  4 timesRepeat: [ semaphore signal ].

  (Delay forSeconds: 10) wait.

  state allSatisfy: [ : i | i = 3 ].
]

