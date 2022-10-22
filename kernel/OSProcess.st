Object subclass: OSProcess [

    | program arguments stdin stdout stderr pid returnCode |

    OSProcess class >> program: aString arguments: anArray [
        ^(self basicNew)
            program: aString;
            stderr: nil;
            stdout: nil;
            stdin:  nil;
            initialize;
            yourself
    ]

    OSProcess class >> program: aString arguments: anArray stdOut: aStdOutFD stdErr: aStdErrFD stdIn: aStdInFD [
        ^(self basicNew)
            program: aString;
            stderr: aStdErrFD;
            stdout:  aStdOutFD;
            stdin:  aStdInFD;
            initialize;
            yourself
    ]

    execute [

        <primitive: VMpr_OSProcess_exec>
    ]
]
