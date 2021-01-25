/* Some stack operations */
#define VM_UNCHECKED_PUSH_OOP(oop) \
  (*++sp = (oop))

#define VM_UNCHECKED_SET_TOP(oop) \
  (*sp = (oop))

#ifndef OPTIMIZE
#define VM_PUSH_OOP(oop)                                 \
  do {                                                   \
    OOP __pushOOP = (oop);                               \
    if (IS_OOP (__pushOOP) && !IS_OOP_VALID (__pushOOP)) \
      abort ();                                          \
    VM_UNCHECKED_PUSH_OOP (__pushOOP);                   \
  } while (0)
#else
#define VM_PUSH_OOP(oop)                        \
  do {                                          \
    OOP __pushOOP = (oop);                      \
    VM_UNCHECKED_PUSH_OOP (__pushOOP);          \
  } while (0)
#endif

#define VM_POP_OOP()                            \
  (*sp--)

#define VM_POP_N_OOPS(n)                        \
  (sp -= (n))

#define VM_UNPOP(n)                             \
  (sp += (n))

#define VM_STACKTOP()                           \
  (*sp)

#ifndef OPTIMIZE
#define VM_SET_STACKTOP(oop)                             \
  do {                                                   \
    OOP __pushOOP = (oop);                               \
    if (IS_OOP (__pushOOP) && !IS_OOP_VALID (__pushOOP)) \
      abort ();                                          \
    VM_UNCHECKED_SET_TOP(__pushOOP);                     \
  } while (0)
#else
#define VM_SET_STACKTOP(oop)                    \
  do {                                          \
    OOP __pushOOP = (oop);                      \
    VM_UNCHECKED_SET_TOP(__pushOOP);            \
  } while (0)
#endif

#define VM_SET_STACKTOP_INT(i)                  \
  VM_UNCHECKED_SET_TOP(FROM_INT(i))

#define VM_SET_STACKTOP_BOOLEAN(exp)                            \
  VM_UNCHECKED_SET_TOP((exp) ? _gst_true_oop : _gst_false_oop)

#define VM_STACK_AT(i)                          \
  (sp[-(i)])

/* using STACK_AT is correct: numArgs == 0 means that there's just the
 * receiver on the stack, at 0.  numArgs = 1 means that at location 0 is
 * the arg, location 1 is the receiver.  And so on.  */
#define VM_SEND_MESSAGE(sendSelector, sendArgs)                      \
  do {                                                               \
    OOP _receiver;                                                   \
    _receiver = VM_STACK_AT(sendArgs);                               \
    _gst_send_message_internal(sendSelector, sendArgs, _receiver,    \
                               OOP_INT_CLASS(_receiver));            \
  } while (0)

