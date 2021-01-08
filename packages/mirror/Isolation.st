UndefinedObject extend [

  nameIn: aClass [

    ^ aClass name
  ]
]

Object subclass: MetaLevelKernel [

  | rootNs undeclaredNs metaclass class compiledMethodClass methodDictionaryClass |

  initialize [
    self createNamespace;
         installPrimitives;
         loadKernel;
         fixKernel;
         fixNamespaces
  ]

  createNamespace [

    rootNs := (Namespace basicNew: 24) initialize: 24;
                                       name: #Bootstrap; 
                                       yourself.

    undeclaredNs := (RootNamespace basicNew: 24) initialize: 24;
                                                 name: #Undeclared;
                                                 yourself.

    rootNs at: #Undeclared put: undeclaredNs.
  ]

  installPrimitives [

    VMPrimitives keysAndValuesDo: [ : aPrimitiveName : aPrimitiveIndex |
      rootNs at: aPrimitiveName put: aPrimitiveIndex.
    ].
  ]

  loadKernel [
 
    | currentNs |
    currentNs := Namespace current.
    Namespace current: rootNs.
    ((PackageLoader packageAt: 'Mirror') / 'kernel/kernel.st') fileIn.
    Namespace current: currentNs.
  ]

  fixKernel [

    metaclass := rootNs at: #Metaclass.
    class := rootNs at: #Class.
    compiledMethodClass := rootNs at: #CompiledMethod.
    methodDictionaryClass := rootNs at: #MethodDictionary.

    ' !!! TODO Fix subClasses' printNl.
    ' FIX the root classes ' printNl.

    rootNs keysAndValuesDo: [ : key : value |
      value isClass ifTrue: [
        ('  Class: ', key) displayNl.

        value methodDictionary ifNotNil: [ : methodDictionary | self fixMethodDictionary: methodDictionary ].
        value class methodDictionary ifNotNil: [ : methodDictionary | self fixMethodDictionary: methodDictionary ].

        value class changeClassTo: metaclass.
        (value instVarAt: 1) == nil ifTrue: [ value class instVarAt: 1 put: class ]. ] ].
  ]

  fixMethodDictionary: aMethodDictionary [
    aMethodDictionary do: [ : method | 
      self checkMethodLiteral: (method instVarAt: 1).
      method instVarAt: 3 put: nil;
             changeClassTo: compiledMethodClass ].
      aMethodDictionary changeClassTo: methodDictionaryClass 
  ]

  checkMethodLiteral: anArray [

    | validClasses |
    validClasses := ##( { SmallInteger. True. False. CompiledBlock. BlockClosure. LargeNegativeInteger. LargePositiveInteger. Array. ByteArray. FloatD. FloatE. FloatQ. String. Symbol. Character. VariableBinding } ).
    anArray ifNotNil: [ : anArray | anArray do: [ : anObject |
      (validClasses includes: anObject class) ifFalse: [ ^ self error: 'Invalid type ', anObject class name ]
    ] ]
  ]
]

