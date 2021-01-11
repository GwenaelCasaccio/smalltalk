UndefinedObject extend [

  nameIn: aClass [

    ^ aClass name
  ]
]

Object subclass: MetaLevelKernel [

  | rootNs undeclaredNs metaclass class compiledMethodClass methodDictionaryClass mirror |

  initialize [
    self createNamespace;
         installPrimitives;
         loadKernel;
         fixKernel;
         installMirror;
         fixNamespaces
  ]

  mirror [

    ^ mirror
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

    rootNs keysAndValuesDo: [ : key : value |
      value isClass ifTrue: [
        ('  Class: ', key) displayNl.

        value methodDictionary ifNotNil: [ : methodDictionary | self fixMethodDictionary: methodDictionary ].
        value class methodDictionary ifNotNil: [ : methodDictionary | self fixMethodDictionary: methodDictionary ].

        value class changeClassTo: metaclass.
        (value instVarAt: 1) == nil ifTrue: [ (Class instVarAt: 4) ifNotNil: [ : subclasses | Class instVarAt: 4 put: ((subclasses reject: [ : aClass | value class = aClass ])) ].
                                              (rootNs at: #Class) instVarAt: 4 put: (((rootNs at: #Class) instVarAt: 4) 
                                                                                          ifNil: [ { value class } ] 
                                                                                          ifNotNil: [ : subclassArray | subclassArray copyWith: value class ]).
                                              value class instVarAt: 1 put: class ]. ] ].
  ]

  installMirror [

    mirror := rootNs at: #Mirror.
  ]

  fixNamespaces [

    | namespaceClass rootNamespaceClass |
    namespaceClass := rootNs at: #Namespace.
    rootNamespaceClass := rootNs at: #RootNamespace.
    rootNs changeClassTo: namespaceClass.
    undeclaredNs changeClassTo: rootNamespaceClass.
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

