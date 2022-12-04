Object subclass: OSProcess [

    | program arguments stdin stdout stderr pid returnCode |

    OSProcess class >> pipe [

        ^ FileDescriptor basicNew setFD: -2;
                                  yourself
    ]

    OSProcess class >> null [

        ^ FileDescriptor basicNew setFD: -1;
                                  yourself
    ]

    OSProcess class >> program: aString arguments: anArray [
        <category: 'instance creation'>

        ^ (self basicNew)
            program: aString;
            arguments: anArray;
            stderr: self pipe;
            stdout: self pipe;
            stdin:  self pipe;
            yourself
    ]

    OSProcess class >> program: aString arguments: anArray stdin: aStdInFD stdout: aStdOutFD stderr: aStdErrFD [
        <category: 'instance creation'>

        ^ (self basicNew)
            program: aString;
            stderr: aStdErrFD;
            stdout:  aStdOutFD;
            stdin:  aStdInFD;
            yourself
    ]

    program: aString [
        <category: 'accessing'>

        program := aString
    ]


    program [
        <category: 'accessing'>

        ^ program
    ]

    arguments: anArray [
        <category: 'accessing'>

        arguments := anArray
    ]

    arguments [
        <category: 'accessing'>

        ^ arguments
    ]

    stdin: aStdIn [
        <category: 'accessing'>

        stdin := aStdIn.
    ]

    stdin [
        <category: 'accessing'>

        ^ stdin
    ]
    
    stdout [
        <category: 'accessing'>

        ^ stdout
    ]

    stdout: aStdOut [
        <category: 'accessing'>

        stdout := aStdOut.
    ]

    stderr [
        <category: 'accessing'>

        ^ stderr
    ]

    stderr: aStdErr [
        <category: 'accessing'>

        stderr := aStdErr.
    ]

    execute [

        <primitive: VMpr_OSProcess_exec>
    ]
]
