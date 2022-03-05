#include "gstpriv.h"

uint32_t *tip = 0;

#define READ *tip++;
#define NEXT_BC goto *(gst_byte_codes[*tip++])

gst_object context;

typedef enum {
  LOAD_SELF_REGISTER_BC = 0,
  LOAD_SELF_OUTER_REGISTER_BC,
  LOAD_SELF_IVAR_BC,
  LOAD_LITERAL_REGISTER_BC,
  LOAD_LITERAL_OUTER_REGISTER_BC,
  LOAD_LITERAL_IVAR_BC,
  LOAD_INTEGER_REGISTER_BC,
  LOAD_INTEGER_OUTER_REGISTER_BC,
  LOAD_INTEGER_IVAR_BC,
  MOVE_REGISTER_REGISTER_BC,
  MOVE_OUTER_REGISTER_REGISTER_BC,
  MOVE_IVAR_REGISTER_BC,
  MOVE_REGISTER_OUTER_REGISTER_BC,
  MOVE_OUTER_REGISTER_OUTER_REGISTER_BC,
  MOVE_IVAR_OUTER_REGISTER_BC,
  MOVE_REGISTER_IVAR_BC,
  MOVE_OUTER_REGISTER_IVAR_BC,
  MOVE_IVAR_IVAR_BC,
  LITERAL_SEND_BC,
  SELF_SEND_BC,
  SUPER_SEND_BC,
  REGISTER_SEND_BC,
  OUTER_REGISTER_SEND_BC,
  IVAR_SEND_BC,
  LITERAL_IMMEDIATE_SEND_BC,
  SELF_IMMEDIATE_SEND_BC,
  SUPER_IMMEDIATE_SEND_BC,
  REGISTER_IMMEDIATE_SEND_BC,
  OUTER_REGISTER_IMMEDIATE_SEND_BC,
  IVAR_IMMEDIATE_SEND_BC,
  JUMP_BC,
  REGISTER_JUMP_IF_TRUE_BC,
  OUTER_REGISTER_JUMP_IF_TRUE_BC,
  IVAR_JUMP_IF_TRUE_BC,
  REGISTER_JUMP_IF_FALSE_BC,
  OUTER_REGISTER_JUMP_IF_FALSE_BC,
  IVAR_JUMP_IF_FALSE_BC,
  RETURN_REGISTER_BC,
  RETURN_OUTER_REGISTER_BC,
  RETURN_IVAR_BC,
  RETURN_SELF_BC,
  RETURN_LITERAL_BC,
  NON_LOCAL_RETURN_REGISTER_BC,
  NON_LOCAL_RETURN_OUTER_REGISTER_BC,
  NON_LOCAL_RETURN_IVAR_BC,
  NON_LOCAL_RETURN_SELF_BC,
  NON_LOCAL_RETURN_LITERAL_BC,
  END_OF_INTERPRETER_BC = 255
} _gst_byte_code_t;

void bc() {
  static void *gst_byte_codes[256] = {
    [LOAD_SELF_REGISTER_BC] = &&LOAD_SELF_REGISTER,
    [LOAD_SELF_OUTER_REGISTER_BC] = &&LOAD_SELF_OUTER_SCOPE,
    [LOAD_SELF_IVAR_BC] = &&LOAD_SELF_INSTANCE_VARIABLE,

    [LOAD_LITERAL_REGISTER_BC] = &&LOAD_FROM_LITERAL_REGISTER,
    [LOAD_LITERAL_OUTER_REGISTER_BC] = &&LOAD_FROM_LITERAL_OUTER_SCOPE,
    [LOAD_LITERAL_IVAR_BC] = &&LOAD_FROM_LITERAL_INSTANCE_VARIABLE,

    [LOAD_INTEGER_REGISTER_BC] = &&LOAD_INTEGER_REGISTER,
    [LOAD_INTEGER_OUTER_REGISTER_BC] = &&LOAD_INTEGER_OUTER_SCOPE,
    [LOAD_INTEGER_IVAR_BC] = &&LOAD_INTEGER_INSTANCE_VARIABLE,

    [MOVE_REGISTER_REGISTER_BC] = &&MOVE_REGISTER_TO_REGISTER,
    [MOVE_OUTER_REGISTER_REGISTER_BC] = &&MOVE_OUTER_REGISTER_TO_REGISTER,
    [MOVE_IVAR_REGISTER_BC] = &&MOVE_INSTANCE_VARIABLE_TO_REGISTER,

    [MOVE_REGISTER_OUTER_REGISTER_BC] = &&MOVE_REGISTER_TO_OUTER_REGISTER,
    [MOVE_OUTER_REGISTER_OUTER_REGISTER_BC] = &&MOVE_OUTER_REGISTER_TO_OUTER_REGISTER,
    [MOVE_IVAR_OUTER_REGISTER_BC] = &&MOVE_INSTANCE_VARIABLE_TO_OUTER_REGISTER,

    [MOVE_REGISTER_IVAR_BC] = &&MOVE_REGISTER_TO_INSTANCE_VARIABLE,
    [MOVE_OUTER_REGISTER_IVAR_BC] = &&MOVE_OUTER_REGISTER_TO_INSTANCE_VARIABLE,
    [MOVE_IVAR_IVAR_BC] = &&MOVE_INSTANCE_VARIABLE_TO_INSTANCE_VARIABLE,

    [LITERAL_SEND_BC] = &&LITERAL_SEND,
    [SELF_SEND_BC] = &&SELF_SEND,
    [SUPER_SEND_BC] = &&SUPER_SEND,
    [REGISTER_SEND_BC] = &&REGISTER_SEND,
    [OUTER_REGISTER_SEND_BC] = &&OUTER_REGISTER_SEND,
    [IVAR_SEND_BC] = &&INSTANCE_VARIABLE_SEND,

    [LITERAL_IMMEDIATE_SEND_BC] = &&LITERAL_IMMEDIATE_SEND,
    [SELF_IMMEDIATE_SEND_BC] = &&SELF_IMMEDIATE_SEND,
    [SUPER_IMMEDIATE_SEND_BC] = &&SUPER_IMMEDIATE_SEND,
    [REGISTER_IMMEDIATE_SEND_BC] = &&REGISTER_IMMEDIATE_SEND,
    [OUTER_REGISTER_IMMEDIATE_SEND_BC] = &&OUTER_REGISTER_IMMEDIATE_SEND,
    [IVAR_IMMEDIATE_SEND_BC] = &&INSTANCE_VARIABLE_IMMEDIATE_SEND,

    [JUMP_BC] = &&JUMP,

    [REGISTER_JUMP_IF_TRUE_BC] = &&JUMP_IF_TRUE_REGISTER,
    [OUTER_REGISTER_JUMP_IF_TRUE_BC] = &&JUMP_IF_TRUE_OUTER_REGISTER,
    [IVAR_JUMP_IF_TRUE_BC] = &&JUMP_IF_TRUE_INSTANCE_VARIABLE,

    [REGISTER_JUMP_IF_FALSE_BC] = &&JUMP_IF_FALSE_REGISTER,
    [OUTER_REGISTER_JUMP_IF_FALSE_BC] = &&JUMP_IF_FALSE_OUTER_REGISTER,
    [IVAR_JUMP_IF_FALSE_BC] = &&JUMP_IF_FALSE_INSTANCE_VARIABLE,

    [RETURN_REGISTER_BC] = &&RETURN_REGISTER,
    [RETURN_OUTER_REGISTER_BC] = &&RETURN_OUTER_REGISTER,
    [RETURN_IVAR_BC] = &&RETURN_INSTANCE_VARIABLE,
    [RETURN_SELF_BC] = &&RETURN_SELF,
    [RETURN_LITERAL_BC] = &&RETURN_LITERAL,

    [NON_LOCAL_RETURN_REGISTER_BC] = &&NON_LOCAL_RETURN_REGISTER,
    [NON_LOCAL_RETURN_OUTER_REGISTER_BC] = &&NON_LOCAL_RETURN_OUTER_REGISTER,
    [NON_LOCAL_RETURN_IVAR_BC] = &&NON_LOCAL_RETURN_INSTANCE_VARIABLE,
    [NON_LOCAL_RETURN_SELF_BC] = &&NON_LOCAL_RETURN_SELF,
    [NON_LOCAL_RETURN_LITERAL_BC] = &&NON_LOCAL_RETURN_LITERAL,

    &&MAKE_DIRTY_TO_REGISTER,

    &&LINE_NUMBER,

    &&INVALID,

    [END_OF_INTERPRETER_BC] = &&END_OF_INTERPRETER
  };

  NEXT_BC;
  
 LOAD_SELF_REGISTER: {
    uint32_t register_idx = READ;
    context->data[register_idx] = _gst_self[0];
    
    NEXT_BC;
  }

 LOAD_SELF_OUTER_SCOPE: {
    uint32_t scope_idx = READ;
    uint32_t register_idx = READ;
    OOP contextOOP;
    gst_object context;

    context = OOP_TO_OBJ(_gst_this_context_oop[0]);
    do {
      contextOOP = OBJ_BLOCK_CONTEXT_GET_OUTER_CONTEXT(context);
      context = OOP_TO_OBJ(contextOOP);
    } while (--scope_idx);

    context->data[register_idx] = _gst_self[0];
    
    NEXT_BC;
  }

 LOAD_SELF_INSTANCE_VARIABLE: {
    uint32_t ivar_idx = READ;

    INSTANCE_VARIABLE(_gst_self[0], ivar_idx) = _gst_self[0];
      
    NEXT_BC;
  }

 LOAD_FROM_LITERAL_REGISTER: {
    const uint32_t literal_idx = READ;
    const uint32_t register_idx = READ;

    context->data[register_idx] = _gst_literals[0][literal_idx];

    NEXT_BC;
  }

 LOAD_FROM_LITERAL_OUTER_SCOPE: {
    const uint32_t literal_idx = READ;
    uint32_t scope_idx = READ;
    uint32_t register_idx = READ;
    OOP contextOOP;
    gst_object context;

    context = OOP_TO_OBJ(_gst_this_context_oop[0]);
    do {
      contextOOP = OBJ_BLOCK_CONTEXT_GET_OUTER_CONTEXT(context);
      context = OOP_TO_OBJ(contextOOP);
    } while (--scope_idx);
   
    context->data[register_idx] = _gst_literals[0][literal_idx];

    NEXT_BC;
  }

 LOAD_FROM_LITERAL_INSTANCE_VARIABLE: {
    const uint32_t literal_idx = READ;
    const uint32_t ivar_idx = READ;

    INSTANCE_VARIABLE(_gst_self[0], ivar_idx) = _gst_literals[0][literal_idx];

    NEXT_BC;
  }

 LOAD_INTEGER_REGISTER: {
    const int32_t number = READ;
    const uint32_t register_idx = READ;

    context->data[register_idx] = (OOP) (intptr_t) number;
    
    NEXT_BC;
  }

 LOAD_INTEGER_OUTER_SCOPE: {
    const int32_t number = READ;
    uint32_t scope_idx = READ;
    uint32_t register_idx = READ;
    OOP contextOOP;
    gst_object context;

    context = OOP_TO_OBJ(_gst_this_context_oop[0]);
    do {
      contextOOP = OBJ_BLOCK_CONTEXT_GET_OUTER_CONTEXT(context);
      context = OOP_TO_OBJ(contextOOP);
    } while (--scope_idx);
   
    context->data[register_idx] = (OOP) (intptr_t) number;
      
    NEXT_BC;
  }

 LOAD_INTEGER_INSTANCE_VARIABLE: {
    const int32_t number = READ;
    const uint32_t ivar_idx = READ;

    INSTANCE_VARIABLE(_gst_self[0], ivar_idx) = (OOP) (intptr_t) number;

    NEXT_BC;
  }

 MOVE_REGISTER_TO_REGISTER: {
    const uint32_t src_register_idx = READ;
    const uint32_t dst_register_idx = READ;

    OOP_TO_OBJ(_gst_this_context_oop[0])->data[dst_register_idx] = OOP_TO_OBJ(_gst_this_context_oop[0])->data[src_register_idx];

    NEXT_BC;
  }

 MOVE_OUTER_REGISTER_TO_REGISTER: {
    uint32_t scope_idx = READ;
    uint32_t src_register_idx = READ;
    uint32_t dst_register_idx = READ;
    OOP contextOOP;
    gst_object context;

    context = OOP_TO_OBJ(_gst_this_context_oop[0]);

    do {
      contextOOP = OBJ_BLOCK_CONTEXT_GET_OUTER_CONTEXT(context);
      context = OOP_TO_OBJ(contextOOP);
    } while (--scope_idx);
   
    OOP_TO_OBJ(_gst_this_context_oop[0])->data[dst_register_idx] = OOP_TO_OBJ(_gst_this_context_oop[0])->data[src_register_idx];

    NEXT_BC;
  }

 MOVE_INSTANCE_VARIABLE_TO_REGISTER: {
    const uint32_t ivar_idx = READ;
    const uint32_t register_idx = READ;

    OOP_TO_OBJ(_gst_this_context_oop[0])->data[register_idx] = INSTANCE_VARIABLE(_gst_self[0], ivar_idx);

    NEXT_BC;
  }

 MOVE_REGISTER_TO_OUTER_REGISTER: {
    uint32_t src_register_idx = READ;
    uint32_t scope_idx = READ;
    uint32_t dst_register_idx = READ;
    OOP contextOOP;
    gst_object context;

    context = OOP_TO_OBJ(_gst_this_context_oop[0]);

    do {
      contextOOP = OBJ_BLOCK_CONTEXT_GET_OUTER_CONTEXT(context);
      context = OOP_TO_OBJ(contextOOP);
    } while (--scope_idx);
   
    context->data[dst_register_idx] = OOP_TO_OBJ(_gst_this_context_oop[0])->data[src_register_idx];

    NEXT_BC;
  }

 MOVE_OUTER_REGISTER_TO_OUTER_REGISTER: {
    uint32_t src_scope_idx = READ;
    uint32_t src_register_idx = READ;
    uint32_t dst_scope_idx = READ;
    uint32_t dst_register_idx = READ;
    OOP contextOOP;
    gst_object src_context;
    gst_object dst_context;

    src_context = OOP_TO_OBJ(_gst_this_context_oop[0]);

    do {
      contextOOP = OBJ_BLOCK_CONTEXT_GET_OUTER_CONTEXT(src_context);
      src_context = OOP_TO_OBJ(contextOOP);
    } while (--src_scope_idx);
   
    dst_context = OOP_TO_OBJ(_gst_this_context_oop[0]);

    do {
      contextOOP = OBJ_BLOCK_CONTEXT_GET_OUTER_CONTEXT(dst_context);
      dst_context = OOP_TO_OBJ(contextOOP);
    } while (--dst_scope_idx);

     dst_context->data[dst_register_idx] = src_context->data[src_register_idx];

    NEXT_BC;
  }

 MOVE_INSTANCE_VARIABLE_TO_OUTER_REGISTER: {
    uint32_t ivar_idx = READ;
    uint32_t scope_idx = READ;
    uint32_t dst_register_idx = READ;
    OOP contextOOP;
    gst_object context;

    context = OOP_TO_OBJ(_gst_this_context_oop[0]);

    do {
      contextOOP = OBJ_BLOCK_CONTEXT_GET_OUTER_CONTEXT(context);
      context = OOP_TO_OBJ(contextOOP);
    } while (--scope_idx);
   
    context->data[dst_register_idx] = INSTANCE_VARIABLE(_gst_self[0], ivar_idx);

    NEXT_BC;
  }

 MOVE_REGISTER_TO_INSTANCE_VARIABLE: {
    uint32_t ivar_idx = READ;
    uint32_t src_register_idx = READ;

    INSTANCE_VARIABLE(_gst_self[0], ivar_idx) = OOP_TO_OBJ(_gst_this_context_oop[0])->data[src_register_idx];

     NEXT_BC;
  }

 MOVE_OUTER_REGISTER_TO_INSTANCE_VARIABLE: {
    uint32_t scope_idx = READ;
    uint32_t src_register_idx = READ;
    uint32_t ivar_idx = READ;
    OOP contextOOP;
    gst_object context;

    context = OOP_TO_OBJ(_gst_this_context_oop[0]);

    do {
      contextOOP = OBJ_BLOCK_CONTEXT_GET_OUTER_CONTEXT(context);
      context = OOP_TO_OBJ(contextOOP);
    } while (--scope_idx);
   
    INSTANCE_VARIABLE(_gst_self[0], ivar_idx) = context->data[src_register_idx];

    NEXT_BC;
  }

 MOVE_INSTANCE_VARIABLE_TO_INSTANCE_VARIABLE: {
    const uint32_t src_ivar_idx = READ;
    const uint32_t dst_ivar_idx = READ;

    INSTANCE_VARIABLE(_gst_self[0], dst_ivar_idx) = INSTANCE_VARIABLE(_gst_self[0], src_ivar_idx);

    NEXT_BC;
  }

 LITERAL_SEND: {
    const uint32_t literal_idx = READ;
    const uint32_t selector_idx = READ;
    const uint32_t args_idx = READ;
    const OOP receiverOOP = _gst_literals[0][literal_idx];
    const OOP selectorOOP = _gst_literals[0][selector_idx];

    _new_gst_send_message_internal(receiverOOP,
				   OOP_INT_CLASS(receiverOOP),
				   selectorOOP,
				   args_idx);
   
    NEXT_BC;
  }

 SELF_SEND: {
    const uint32_t selector_idx = READ;
    const uint32_t args_idx = READ;
    const OOP receiverOOP = _gst_self[0];
    const OOP selectorOOP = _gst_literals[0][selector_idx];

    _new_gst_send_message_internal(receiverOOP,
			       OOP_INT_CLASS(receiverOOP),
			       selectorOOP,
			       args_idx);

   NEXT_BC;
  }

 SUPER_SEND: {
    const uint32_t class_literal_idx = READ;
    const uint32_t selector_idx = READ;
    const uint32_t args_idx = READ;
    const OOP receiverOOP = _gst_self[0];
    const OOP classOOP = _gst_literals[0][class_literal_idx];
    const OOP selectorOOP = _gst_literals[0][selector_idx];

    _new_gst_send_message_internal(receiverOOP,
				   classOOP,
				   selectorOOP,
				   args_idx);

    NEXT_BC;
  }

 REGISTER_SEND: {
    const uint32_t register_idx = READ;
    const uint32_t selector_idx = READ;
    const uint32_t args_idx = READ;
    const OOP receiverOOP = OOP_TO_OBJ(_gst_this_context_oop[0])->data[register_idx];
    const OOP selectorOOP = _gst_literals[0][selector_idx];

    _new_gst_send_message_internal(receiverOOP,
				   OOP_INT_CLASS(receiverOOP),
				   selectorOOP,
				   args_idx);

    NEXT_BC;
  }

 OUTER_REGISTER_SEND: {
    uint32_t scope_idx = READ;
    const uint32_t register_idx = READ;
    const uint32_t selector_idx = READ;
    const uint32_t args_idx = READ;
    const OOP selectorOOP = _gst_literals[0][selector_idx];
    OOP contextOOP;
    gst_object context;

    context = OOP_TO_OBJ(_gst_this_context_oop[0]);

    do {
      contextOOP = OBJ_BLOCK_CONTEXT_GET_OUTER_CONTEXT(context);
      context = OOP_TO_OBJ(contextOOP);
    } while (--scope_idx);
 
    const OOP receiverOOP = context->data[register_idx];

    _new_gst_send_message_internal(receiverOOP,
			       OOP_INT_CLASS(receiverOOP),
			       selectorOOP,
			       args_idx);

    NEXT_BC;
  }

 INSTANCE_VARIABLE_SEND: {
    const uint32_t ivar_idx = READ;
    const uint32_t selector_idx = READ;
    const uint32_t args_idx = READ;
    const OOP receiverOOP = INSTANCE_VARIABLE(_gst_self[0], ivar_idx);
    const OOP selectorOOP = _gst_literals[0][selector_idx];

    _new_gst_send_message_internal(receiverOOP,
			       OOP_INT_CLASS(receiverOOP),
			       selectorOOP,
			       args_idx);

    NEXT_BC;
  }

 LITERAL_IMMEDIATE_SEND: {
    const uint32_t literal_idx = READ;
    const uint32_t selector_idx = READ;
    const OOP receiverOOP = _gst_literals[0][literal_idx];
    const struct builtin_selector *bs = &_gst_builtin_selectors[selector_idx];

    _new_gst_send_message_internal(receiverOOP,
				   OOP_INT_CLASS(receiverOOP),
				   bs->symbol,
				   (uint32_t)bs->numArgs);
 
    NEXT_BC;
  }

 SELF_IMMEDIATE_SEND: {
    const uint32_t selector_idx = READ;
    const OOP receiverOOP = _gst_self[0];
    const struct builtin_selector *bs = &_gst_builtin_selectors[selector_idx];

    _new_gst_send_message_internal(receiverOOP,
				   OOP_INT_CLASS(receiverOOP),
				   bs->symbol,
				   (uint32_t)bs->numArgs);
     NEXT_BC;
  }

 SUPER_IMMEDIATE_SEND: {
    const uint32_t class_literal_idx = READ;
    const uint32_t selector_idx = READ;
    const OOP receiverOOP = _gst_self[0];
    const OOP classOOP = _gst_literals[0][class_literal_idx];
    const struct builtin_selector *bs = &_gst_builtin_selectors[selector_idx];

    _new_gst_send_message_internal(receiverOOP,
				   classOOP,
				   bs->symbol,
				   (uint32_t)bs->numArgs);
    NEXT_BC;
  }

 REGISTER_IMMEDIATE_SEND: {
    const uint32_t register_idx = READ;
    const uint32_t selector_idx = READ;
    const OOP receiverOOP = context->data[register_idx];
    const struct builtin_selector *bs = &_gst_builtin_selectors[selector_idx];

    _new_gst_send_message_internal(receiverOOP,
			       OOP_INT_CLASS(receiverOOP),
			       bs->symbol,
			       (uint32_t)bs->numArgs);

    NEXT_BC;
  }

 OUTER_REGISTER_IMMEDIATE_SEND: {
    uint32_t scope_idx = READ;
    const uint32_t register_idx = READ;
    const uint32_t selector_idx = READ;
    const struct builtin_selector *bs = &_gst_builtin_selectors[selector_idx];
    OOP contextOOP;
    gst_object context;

    context = OOP_TO_OBJ(_gst_this_context_oop[0]);

    do {
      contextOOP = OBJ_BLOCK_CONTEXT_GET_OUTER_CONTEXT(context);
      context = OOP_TO_OBJ(contextOOP);
    } while (--scope_idx);

    const OOP receiverOOP = context->data[register_idx];

    _new_gst_send_message_internal(receiverOOP,
			       OOP_INT_CLASS(receiverOOP),
			       bs->symbol,
			       (uint32_t)bs->numArgs);

    NEXT_BC;
  }

 INSTANCE_VARIABLE_IMMEDIATE_SEND: {
    const uint32_t ivar_idx = READ;
    const uint32_t selector_idx = READ;
    const OOP receiverOOP = INSTANCE_VARIABLE(_gst_self[0], ivar_idx);
    const struct builtin_selector *bs = &_gst_builtin_selectors[selector_idx];

    _new_gst_send_message_internal(receiverOOP,
			       OOP_INT_CLASS(receiverOOP),
			       bs->symbol,
			       (uint32_t)bs->numArgs);
    
    NEXT_BC;
  }

 JUMP: {
    const int32_t offset = READ;
    tip = tip + offset;
    NEXT_BC;
  }

 JUMP_IF_TRUE_REGISTER: {
    const uint32_t register_idx = READ;
    const int32_t offset = READ;

    if (context->data[register_idx] == _gst_true_oop) {
      tip = tip + offset;
    }
    
    NEXT_BC;
  }

 JUMP_IF_TRUE_OUTER_REGISTER: {
    uint32_t scope_idx = READ;
    const uint32_t register_idx = READ;
    const int32_t offset = READ;
    OOP contextOOP;
    gst_object context;

    context = OOP_TO_OBJ(_gst_this_context_oop[0]);

    do {
      contextOOP = OBJ_BLOCK_CONTEXT_GET_OUTER_CONTEXT(context);
      context = OOP_TO_OBJ(contextOOP);
    } while (--scope_idx);
   
    if (context->data[register_idx] == _gst_true_oop) {
      tip = tip + offset;
    }
    
    NEXT_BC;
  }

 JUMP_IF_TRUE_INSTANCE_VARIABLE: {
    const uint32_t ivar_idx = READ;
    const int32_t offset = READ;

    if (INSTANCE_VARIABLE(_gst_self[0], ivar_idx) == _gst_true_oop) {
      tip = tip + offset;
    }

    NEXT_BC;
  }

 JUMP_IF_FALSE_REGISTER: {
    const uint32_t register_idx = READ;
    const int32_t offset = READ;

    if (context->data[register_idx] == _gst_false_oop) {
      tip = tip + offset;
    }
    
    NEXT_BC;
  }

 JUMP_IF_FALSE_OUTER_REGISTER: {
    uint32_t scope_idx = READ;
    const uint32_t register_idx = READ;
    const int32_t offset = READ;
    OOP contextOOP;
    gst_object context;

    context = OOP_TO_OBJ(_gst_this_context_oop[0]);

    do {
      contextOOP = OBJ_BLOCK_CONTEXT_GET_OUTER_CONTEXT(context);
      context = OOP_TO_OBJ(contextOOP);
    } while (--scope_idx);
   
    if (context->data[register_idx] == _gst_false_oop) {
      tip = tip + offset;
    }
    
    NEXT_BC;
  }

 JUMP_IF_FALSE_INSTANCE_VARIABLE: {
    const uint32_t ivar_idx = READ;
    const int32_t offset = READ;

    if (INSTANCE_VARIABLE(_gst_self[0], ivar_idx) == _gst_false_oop) {
      tip = tip + offset;
    }

    NEXT_BC;
  }

 RETURN_REGISTER: {
    const intptr_t return_register_idx = TO_INT(OOP_TO_OBJ(_gst_this_context_oop[0])->data[7]);
    const uint32_t register_idx = READ;
    const OOP valueOOP = context->data[register_idx];

    unwind_method();
    context->data[return_register_idx] = valueOOP;
    
    NEXT_BC;
  }

 RETURN_OUTER_REGISTER: {
    const intptr_t return_register_idx = TO_INT(OOP_TO_OBJ(_gst_this_context_oop[0])->data[7]);
    uint32_t scope_idx = READ;
    const uint32_t register_idx = READ;
    OOP contextOOP;
    gst_object context;

    context = OOP_TO_OBJ(_gst_this_context_oop[0]);

    do {
      contextOOP = OBJ_BLOCK_CONTEXT_GET_OUTER_CONTEXT(context);
      context = OOP_TO_OBJ(contextOOP);
    } while (--scope_idx);

    const OOP valueOOP = context->data[register_idx];
    unwind_method();
    OOP_TO_OBJ(_gst_this_context_oop[0])->data[return_register_idx] = valueOOP;

    NEXT_BC;
  }

 RETURN_INSTANCE_VARIABLE: {
    const intptr_t return_register_idx = TO_INT(OOP_TO_OBJ(_gst_this_context_oop[0])->data[7]);
    const uint32_t ivar_idx = READ;
    const OOP valueOOP = INSTANCE_VARIABLE(_gst_self[0], ivar_idx);

    unwind_method();
    OOP_TO_OBJ(_gst_this_context_oop[0])->data[return_register_idx] = valueOOP;

    NEXT_BC;
  }

 RETURN_SELF: {
    const intptr_t return_register_idx = TO_INT(context->data[7]);
    const OOP valueOOP = _gst_self[0];

    unwind_method();
    context->data[return_register_idx] = valueOOP;

    NEXT_BC;
  }

 RETURN_LITERAL: {
    const intptr_t return_register_idx = TO_INT(context->data[7]);
    const uint32_t literal_idx = READ;
    const OOP valueOOP = _gst_literals[0][literal_idx];

    unwind_method();
    context->data[return_register_idx] = valueOOP;

    NEXT_BC;
  }

 NON_LOCAL_RETURN_REGISTER: {
    const intptr_t return_register_idx = TO_INT(context->data[7]);
    const uint32_t register_idx = READ;
    const OOP valueOOP = context->data[register_idx];

    if (UNCOMMON (!unwind_method())) {
      _new_gst_send_message_internal(valueOOP,
				     OOP_INT_CLASS(valueOOP),
				     _gst_bad_return_error_symbol,
				     0);
    } else {
      context->data[return_register_idx] = valueOOP;
    }

    NEXT_BC;
  }

 NON_LOCAL_RETURN_OUTER_REGISTER: {
    const intptr_t return_register_idx = TO_INT(context->data[7]);
    uint32_t scope_idx = READ;
    const uint32_t register_idx = READ;
    OOP contextOOP;
    gst_object context;

    context = OOP_TO_OBJ(_gst_this_context_oop[0]);

    do {
      contextOOP = OBJ_BLOCK_CONTEXT_GET_OUTER_CONTEXT(context);
      context = OOP_TO_OBJ(contextOOP);
    } while (--scope_idx);

    const OOP valueOOP = context->data[register_idx];

    if (UNCOMMON (!unwind_method())) {
      _new_gst_send_message_internal(valueOOP,
				     OOP_INT_CLASS(valueOOP),
				     _gst_bad_return_error_symbol,
				     0);
    } else {
      context->data[return_register_idx] = valueOOP;
    }

    NEXT_BC;
  }

 NON_LOCAL_RETURN_INSTANCE_VARIABLE: {
    const intptr_t return_register_idx = TO_INT(context->data[7]);
    const OOP valueOOP = _gst_self[0];

    if (UNCOMMON (!unwind_method())) {
      _new_gst_send_message_internal(valueOOP,
				     OOP_INT_CLASS(valueOOP),
				     _gst_bad_return_error_symbol,
				     0);
    } else {
      context->data[return_register_idx] = valueOOP;
    }

    NEXT_BC;
  }

 NON_LOCAL_RETURN_SELF: {
    const intptr_t return_register_idx = TO_INT(context->data[7]);
    const OOP valueOOP = _gst_self[0];

    if (UNCOMMON (!unwind_method())) {
      _new_gst_send_message_internal(valueOOP,
				     OOP_INT_CLASS(valueOOP),
				     _gst_bad_return_error_symbol,
				     0);
    } else {
      context->data[return_register_idx] = valueOOP;
    }

    NEXT_BC;
  }

 NON_LOCAL_RETURN_LITERAL: {
    const intptr_t return_register_idx = TO_INT(context->data[7]);
    const uint32_t literal_idx = READ;
    const OOP valueOOP = _gst_literals[0][literal_idx];

    if (UNCOMMON (!unwind_method())) {
      _new_gst_send_message_internal(valueOOP,
				     OOP_INT_CLASS(valueOOP),
				     _gst_bad_return_error_symbol,
				     0);
    } else {
      context->data[return_register_idx] = valueOOP;
    }
    
    NEXT_BC;
  }

 MAKE_DIRTY_TO_REGISTER: {
    const uint32_t literal_idx = READ;
    const uint32_t register_idx = READ;
    const OOP compiledBlockOOP = _gst_literals[0][literal_idx];
    
    context->data[register_idx] = _gst_make_block_closure(compiledBlockOOP);

    NEXT_BC;
  }

 LINE_NUMBER: {
    NEXT_BC;
  }

 INVALID: {
    NEXT_BC;
  }


 END_OF_INTERPRETER: {
    return ;
  }
}
