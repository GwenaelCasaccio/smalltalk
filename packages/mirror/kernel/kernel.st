nil subclass: Object [

  isClass [

    ^ false
  ]

  instVarAt: anIndex [

    <primitive: VMpr_Object_instVarAt>
  ]

  instVarAt: anIndex put: anObject [

    <primitive: VMpr_Object_instVarAt>
  ]

  basicPrint [

    <primitive: VMpr_Object_basicPrint>
  ]

  = anObject [

    <primitive: VMpr_Object_identity>
  ]

  changeClassTo: aBehavior [

    <primitive: VMpr_Object_changeClassTo>
    'error' basicPrint.
  ]

  class [

    <primitive: VMpr_Object_class>
  ]

  yourself [

    ^ self
  ]

  doesNotUnderstand: aMessage [

    'Not Know Message ' displayNl.
    aMessage selector displayNl.
  ]

  mustBeBoolean [

    | result |
    result := MustBeBoolean signalOn: self.
    result == false ifFalse: [ result := true ].
    ^ result
  ]

  makeReadOnly: aBoolean [
    <primitive: VMpr_Object_makeReadOnly>
  ]

  hash [

    <primitive: VMpr_Object_hash>
  ]
]

Object subclass: Behavior [
  | superClass methodDictionary instanceSpec subClasses instanceVariables |

  superClass: aBehavior [

    superClass := aBehavior.
  ]

  superClass [

    ^ superClass
  ]

  methodDictionary [

    ^ methodDictionary
  ]
]

Behavior subclass: Class [
  | name comment category environment classVariables sharedPools pragmaHandlers |

  isClass [ ^ true ]

  name [ ^ name ]
]

Behavior subclass: Metaclass [
  | instanceClass |

]

Object subclass: String [

  <shape: #character>
]

Object subclass: Array [

  <shape: #pointer>
]

Object subclass: ByteArray [

  <shape: #byte>
]

Object subclass: MethodDictionary [

  | tally mutex |

  <shape: #pointer>
]

Object subclass: CompiledCode [

  | literals header |

  <shape: #byte>
]

CompiledCode subclass: CompiledMethod [

  | descriptor |

  <shape: #byte>
]

CompiledCode subclass: CompiledBlock [

  | method |

  <shape: #byte>
]

Object subclass: VariableBinding [
  | key value environment |

  value [

    ^ value
  ]
]

Object subclass: RootNamespace [

  | tally environment name subspaces sharedPools |

  <shape: #pointer>
]

Object subclass: Namespace [

  | tally environment name subspaces sharedPools |

  <shape: #pointer>
]

Object subclass: MirrorPrimitive [

  MirrorPrimitive class >> on: anObject do: aPrimitiveInteger [

    <primitive: VMpr_Object_primitive>
  ]

  MirrorPrimitive class >> on: anObject with: anArgumentObject1 do: aPrimitiveInteger [

    <primitive: VMpr_Object_primitive>
  ]


  MirrorPrimitive class >> on: anObject with: anArgumentObject1 with: anArgumentObject2 do: aPrimitiveInteger [

    <primitive: VMpr_Object_primitive>
  ]

  MirrorPrimitive class >> on: anObject with: anArgumentObject1 with: anArgumentObject2 with: anArgumentObject3 do: aPrimitiveInteger [

    <primitive: VMpr_Object_primitive>
  ]

  MirrorPrimitive class >> on: anObject with: anArgumentObject1 with: anArgumentObject2 with: anArgumentObject3 with: anArgumentObject4 do: aPrimitiveInteger [

    <primitive: VMpr_Object_primitive>
  ]
]

Object subclass: MirrorBuilder [

  | subject |

  MirrorBuilder class >> on: anObject [

    ^ self basicNew object: anObject;
                    yourself
  ]

  object: anObject [

    subject := anObject
  ]
]

nil subclass: Mirror [

  | subject |

  Mirror class >> on: anObject [

    | mirror |
    mirror := MirrorBuilder on: anObject.
    MirrorPrimitive on: mirror with: Mirror do: VMpr_Object_changeClassTo.
    MirrorPrimitive on: mirror with: true do: VMpr_Object_makeReadOnly.
    ^ mirror
  ]

  doesNotUnderstand: aMessage [

  ]

  instVarAt: anIndex [

    ^ subject instVarAt: anIndex
  ]

  at: anIndex [

    ^ subject basicAt: anIndex
  ]

  class [

    ^ subject class
  ]
]

