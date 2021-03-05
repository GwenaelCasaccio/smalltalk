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
