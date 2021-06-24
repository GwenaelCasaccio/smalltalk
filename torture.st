Eval [
  | ps1 ps2 ps3 |

  ps1 := ProcessorScheduler newFromClosure: [
        | d a |
          a := 1024 * 1024 * 40.
          d := Array new: a.
          1 to: a do: [ :i |
            d at: i put: (Object new).
            (i \\ 10) = 0 ifTrue: [ 'ps1' basicPrint ]
          ].
         'ok' basicPrint.
         ps1 killThread ].

  Smalltalk at: #Foo1 put: ps1.

  ps2 := ProcessorScheduler newFromClosure: [
        | d a |
          a := 1024 * 1024 * 40.
          d := Array new: a.
          1 to: a do: [ :i |
            d at: i put: (Object new).
            (i \\ 10) = 0 ifTrue: [ 'ps2' basicPrint ]
          ].
          [ true ] whileTrue.
         'ok' basicPrint.
         ps2 killThread ].

  Smalltalk at: #Foo2 put: ps2.

  ps3 := ProcessorScheduler newFromClosure: [
        | d a |
          a := 1024 * 1024 * 40.
          d := Array new: a.
          1 to: a do: [ :i |
            d at: i put: (Object new).
            (i \\ 10) = 0 ifTrue: [ 'ps3' basicPrint ]
          ].
          [ true ] whileTrue.
         'ok' basicPrint.
         ps3 killThread ].

  Smalltalk at: #Foo3 put: ps3.

  ps1 newThread.
  ps2 newThread.
  ps3 newThread.

  [ true ] whileTrue.
]


