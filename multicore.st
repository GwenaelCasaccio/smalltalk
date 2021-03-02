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
          ps1 key killThread ].

  " GC Protecting the new Processor "
  Smalltalk at: #Foo1 put: ps1 key.

  ps2 := ProcessorScheduler newFromClosure: [
      100 timesRepeat: [
          #[ 1 2 3 ] do: [ : i | i + 2 ].
          1 + 1.
          Array new: 10.
          2 basicPrint.
          ].
          ps2 key killThread ].

  Smalltalk at: #Foo2 put: ps2 key.

  ps3 := ProcessorScheduler newFromClosure: [
      100 timesRepeat: [
          #[ 1 2 3 ] do: [ : i | i + 2 ].
          1 + 1.
          Array new: 10.
          3 basicPrint.
          ].
          ps3 key killThread ].

  Smalltalk at: #Foo3 put: ps3 key.

  ps4 := ProcessorScheduler newFromClosure: [
      100 timesRepeat: [
          #[ 1 2 3 ] do: [ : i | i + 2 ].
          1 + 1.
          Array new: 10.
          4 basicPrint.
          ].
          ps4 key killThread ].

  Smalltalk at: #Foo4 put: ps4 key.

  ps1 key newThread: ps1 value.
  ps2 key newThread: ps2 value.
  ps3 key newThread: ps3 value.
  ps4 key newThread: ps4 value.

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
