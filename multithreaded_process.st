Eval [
  | ps1 |

  ps1 := ProcessorScheduler newFromClosure: [
         'ok' basicPrint.
         [ 
           [ 'ok1' basicPrint.
           'ok2' basicPrint. ] value.

         'ok3' basicPrint.
         ] fork.
         'ok' basicPrint.
         [ true ] whileTrue ].

  ps1 newThread.

  (Delay forSeconds: 100) wait.
]

