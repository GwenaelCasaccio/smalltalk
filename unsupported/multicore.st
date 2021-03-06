Eval [
[
  | ps1 ps2 ps3 ps4 |

  ps1 := ProcessorScheduler newFromClosure: [
      100 timesRepeat: [
          #[ 1 2 3 ] do: [ : i | i + 2 ].
          1 + 1.
          Array new: 10.
          1 basicPrint.
          ].
          ps1 killThread ].

  " GC Protecting the new Processor "
  Smalltalk at: #Foo1 put: ps1.

  ps2 := ProcessorScheduler newFromClosure: [
      100 timesRepeat: [
          #[ 1 2 3 ] do: [ : i | i + 2 ].
          1 + 1.
          Array new: 10.
          2 basicPrint.
          ].
          ps2 killThread ].

  Smalltalk at: #Foo2 put: ps2.

  ps3 := ProcessorScheduler newFromClosure: [
      100 timesRepeat: [
          #[ 1 2 3 ] do: [ : i | i + 2 ].
          1 + 1.
          Array new: 10.
          3 basicPrint.
          ].
          ps3 killThread ].

  Smalltalk at: #Foo3 put: ps3.

  ps4 := ProcessorScheduler newFromClosure: [
      100 timesRepeat: [
          #[ 1 2 3 ] do: [ : i | i + 2 ].
          1 + 1.
          Array new: 10.
          4 basicPrint.
          ].
          ps4 killThread ].

  Smalltalk at: #Foo4 put: ps4.

  ps1 newThread.
  ps2 newThread.
  ps3 newThread.
  ps4 newThread.

  1 to: 200 do: [ : i |
    100 timesRepeat: [
        #[ 1 2 3 ] do: [ : i | i + 1 ]
        ].
    2+2.
    Array new: 10.
    0 basicPrint.
  ].
  [ true ] whileTrue.
  ] value
]
