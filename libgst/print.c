/******************************** -*- C -*- ****************************
 *
 *	OOP printing and debugging module
 *
 *
 ***********************************************************************/

/***********************************************************************
 *
 * Copyright 1988,89,90,91,92,94,95,99,2000,2001,2002,2003,2006,2009
 * Free Software Foundation, Inc.
 * Written by Steve Byrne.
 *
 * This file is part of GNU Smalltalk.
 *
 * GNU Smalltalk is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2, or (at your option) any later
 * version.
 *
 * Linking GNU Smalltalk statically or dynamically with other modules is
 * making a combined work based on GNU Smalltalk.  Thus, the terms and
 * conditions of the GNU General Public License cover the whole
 * combination.
 *
 * In addition, as a special exception, the Free Software Foundation
 * give you permission to combine GNU Smalltalk with free software
 * programs or libraries that are released under the GNU LGPL and with
 * independent programs running under the GNU Smalltalk virtual machine.
 *
 * You may copy and distribute such a system following the terms of the
 * GNU GPL for GNU Smalltalk and the licenses of the other code
 * concerned, provided that you include the source code of that other
 * code when and as the GNU GPL requires distribution of source code.
 *
 * Note that people who make modified versions of GNU Smalltalk are not
 * obligated to grant this special exception for their modified
 * versions; it is their choice whether to do so.  The GNU General
 * Public License gives permission to release a modified version without
 * this exception; this exception also makes it possible to release a
 * modified version which carries forward this exception.
 *
 * GNU Smalltalk is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * GNU Smalltalk; see the file COPYING.  If not, write to the Free Software
 * Foundation, 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 ***********************************************************************/

#include "gstpriv.h"
#include <printf.h>

/* Print a Character OOP to a snprintfv stream, FILE.  */
static int print_char_to_stream(FILE *stream, OOP oop);

/* Print a String OOP to a snprintfv stream, FILE.  */
static int print_string_to_stream(FILE *stream, OOP string);

/* Print an Association OOP's key to a snprintfv stream, FILE.  */
static int print_association_key_to_stream(FILE *stream, OOP associationOOP);

/* Print a Class OOP's name to a snprintfv stream, FILE.  */
static int print_class_name_to_stream(FILE *stream, OOP class_oop);

/* Print a brief description of an OOP to a snprintfv stream, FILE.  */
static int print_oop_constructor_to_stream(FILE *stream, OOP oop);

/* The main routine to handle the %O modifier to printf.  %#O prints
   Strings and Symbols without the leading # or the enclosing single
   quotes, while %+O expects that an Association is passed and prints
   its key.  */
static int printf_oop(FILE *stream, const struct printf_info *info,
                      const void *const *args);

static int printf_oop_arginfo(const struct printf_info *info, size_t n,
                              int *argtypes, int *size);

void _gst_print_object(OOP oop) {
  printf("%O", oop);
  fflush(stdout);
}

int print_char_to_stream(FILE *stream, OOP oop) {
  int val = CHAR_OOP_VALUE(oop);
  if (OOP_CLASS(oop) == _gst_char_class && val > 127) {
    return fprintf(stream, "Character value: 16r%02X", val);

  } else if (val >= 32 && val <= 126) {
    return fprintf(stream, "$%c", val);
  } else if (val < 32) {
    return fprintf(stream, "$<%d>", val);
  } else {
    return fprintf(stream, "$<16r%04X>", val);
}
}

int print_string_to_stream(FILE *stream, OOP string) {
  int len;

  len = _gst_string_oop_len(string);
  if (!len) {
    return 0;
}

  return fprintf(stream, "%.*s", len, (char *)(OOP_TO_OBJ(string)->data));
}

int print_association_key_to_stream(FILE *stream, OOP associationOOP) {
  gst_object association;

  if (!IS_OOP(associationOOP) ||
      !is_a_kind_of(OOP_CLASS(associationOOP), _gst_lookup_key_class)) {
    return fprintf(stream, "<non-association %O in association context>",
                   associationOOP);
  }

  association = OOP_TO_OBJ(associationOOP);
  if (OOP_CLASS(OBJ_ASSOCIATION_GET_KEY(association)) != _gst_symbol_class) {
    return fprintf(stream, "<unprintable key type>");
  } else {
    return fprintf(stream, "%#O", OBJ_ASSOCIATION_GET_KEY(association));
}
}

int print_class_name_to_stream(FILE *stream, OOP class_oop) {
  gst_object class;
  class = OOP_TO_OBJ(class_oop);
  if (IS_A_CLASS(class_oop) && !IS_NIL(OBJ_CLASS_GET_NAME(class))) {
    return print_string_to_stream(stream, OBJ_CLASS_GET_NAME(class));
  } else if (IS_A_CLASS(OOP_CLASS(class_oop))) {
    int r;
    r = fprintf(stream, "<unnamed ");
    r += print_class_name_to_stream(stream, OOP_CLASS(class_oop));
    r += fprintf(stream, ">");
    return r;
  } else {
    return fprintf(stream, "<unnamed class>");
}
}

int print_oop_constructor_to_stream(FILE *stream, OOP oop) {
  int r = 0;
  intptr_t instanceSpec;
  OOP class_oop;

  class_oop = OOP_CLASS(oop);
  r += print_class_name_to_stream(stream, class_oop);

  instanceSpec = CLASS_INSTANCE_SPEC(class_oop);
  if (instanceSpec & ISP_ISINDEXABLE) {
    r += fprintf(stream, " new: %zu ", NUM_INDEXABLE_FIELDS(oop));

  } else {
    r += fprintf(stream, " new ");
}

  if (_gst_regression_testing) {
    r += fprintf(stream, "\"<0>\"");
  } else {
    r += fprintf(stream, "\"<%p>\"", oop);
}

  return r;
}

int printf_oop(FILE *stream, const struct printf_info *info,
               const void *const *args) {
  const OOP oop = *((const OOP *)args[0]);

  if (info->showsign) {
    return print_association_key_to_stream(stream, oop);
  }

  if (IS_INT(oop)) {
    return fprintf(stream, "%td", TO_INT(oop));

  } else if (IS_NIL(oop)) {
    return fprintf(stream, "nil");

  } else if (oop == _gst_true_oop) {
    return fprintf(stream, "true");

  } else if (oop == _gst_false_oop) {
    return fprintf(stream, "false");

  } else if (OOP_CLASS(oop) == _gst_char_class ||
           OOP_CLASS(oop) == _gst_unicode_character_class) {
    return print_char_to_stream(stream, oop);

  } else if (OOP_CLASS(oop) == _gst_floatd_class) {
    double f = FLOATD_OOP_VALUE(oop);
    char buf[100], *p;
    p = buf + sprintf(buf, "%#.*g", (_gst_regression_testing ? 6 : DBL_DIG), f);
    for (; p >= buf; p--) {
      if (*p == 'e') {
        *p = 'd';
        break;
      }
}

    return fputs(buf, stream);
  }

  else if (OOP_CLASS(oop) == _gst_floate_class) {
    double f = FLOATE_OOP_VALUE(oop);
    return fprintf(stream, "%#.*g", (_gst_regression_testing ? 6 : FLT_DIG), f);
  }

  else if (OOP_CLASS(oop) == _gst_floatq_class) {
    long double f = FLOATQ_OOP_VALUE(oop);
    char buf[100], *p;
    p = buf +
        sprintf(buf, "%#.*Lg", (_gst_regression_testing ? 6 : LDBL_DIG), f);
    for (; p >= buf; p--) {
      if (*p == 'e') {
        *p = 'q';
        break;
      }
}

    return fputs(buf, stream);
  }

  else if (OOP_CLASS(oop) == _gst_symbol_class) {
    int r = 0;
    if (!info->alt) {
      r += fprintf(stream, "#");
}
    return r + print_string_to_stream(stream, oop);
  }

  else if (OOP_CLASS(oop) == _gst_string_class) {
    int r = 0;
    /* ### have to quote embedded quote chars */
    if (!info->alt) {
      r += fprintf(stream, "'");
}
    r += print_string_to_stream(stream, oop);
    if (!info->alt) {
      r += fprintf(stream, "'");
}
    return r;
  }

  else if (IS_A_METACLASS(oop)) {
    int r = 0;
    OOP class_oop = _gst_find_an_instance(oop);
    if (IS_NIL(class_oop)) {
      r += print_oop_constructor_to_stream(stream, oop);
    } else {
      r += print_class_name_to_stream(stream, class_oop);
      r += fprintf(stream, " class");
    }
    return r;
  }

  else if (IS_A_CLASS(oop)) {
    return print_class_name_to_stream(stream, oop);

  } else {
    return print_oop_constructor_to_stream(stream, oop);
}
}

int printf_oop_arginfo(const struct printf_info *info, size_t n, int *argtypes,
                       int *size) {
  /* We always take exactly one argument and this is a pointer to the
     structure.  */
  if (n > 0) {
    argtypes[0] = PA_POINTER;
    size[0] = sizeof(OOP);
  }
  return 1;
}

void _gst_classify_addr(void *addr) {
  if (IS_INT(addr)) {
    printf("Smalltalk SmallInteger %td\n", TO_INT(addr));

  } else if (IS_OOP_ADDR(addr)) {
    _gst_display_oop(addr);

  } else {
    _gst_display_object(addr);
}

  fflush(stdout);
}

void _gst_display_oop_short(OOP oop) {
  if (IS_OOP_FREE(oop)) {
    printf("%-10p   Free\n", oop);
  } else {
    printf("%-10p   %-10p  %-10s %-10s %-10s\n", oop, OOP_TO_OBJ(oop),
           OOP_GET_FLAGS(oop) & F_CONTEXT
               ? "Context"
               : OOP_GET_FLAGS(oop) & F_WEAK
                     ? "Weak"
                     : OOP_GET_FLAGS(oop) & F_EPHEMERON ? "Ephemeron" : "",

           OOP_GET_FLAGS(oop) & F_FIXED
               ? "Fixed"
               : OOP_GET_FLAGS(oop) & F_LOADED
                     ? "Permanent"
                     : OOP_GET_FLAGS(oop) & F_OLD
                           ? "Old"
                           : OOP_GET_FLAGS(oop) & _gst_mem.active_flag
                                 ? "To-space"
                                 : "From-space",

           IS_EDEN_ADDR(OOP_TO_OBJ(oop))
               ? "Eden"
               : IS_SURVIVOR_ADDR(OOP_TO_OBJ(oop), 0)
                     ? "Surv (Even)"
                     : IS_SURVIVOR_ADDR(OOP_TO_OBJ(oop), 1)
                           ? "Surv (Odd)"
                           : OOP_GET_FLAGS(oop) & F_POOLED
                                 ? "Pooled"
                                 : OOP_GET_FLAGS(oop) & F_REACHABLE
                                       ? "Old/marked"
                                       : "Old");
  }
}

void _gst_display_oop(OOP oop) {
  if (!IS_OOP_ADDR(oop)) {
    printf("Parameter %p does not appear to be an OOP!\n", oop);
    return;
  }

  if (IS_OOP_FREE(oop)) {
    printf("%-10p   Free\n", oop);
  } else {
    printf("%-10p   %-10p  %-10s %-10s %-10s", oop, OOP_TO_OBJ(oop),
           OOP_GET_FLAGS(oop) & F_CONTEXT
               ? "Context"
               : OOP_GET_FLAGS(oop) & F_WEAK
                     ? "Weak"
                     : OOP_GET_FLAGS(oop) & F_EPHEMERON ? "Ephemeron" : "",

           OOP_GET_FLAGS(oop) & F_FIXED
               ? "Fixed"
               : OOP_GET_FLAGS(oop) & F_LOADED
                     ? "Permanent"
                     : OOP_GET_FLAGS(oop) & F_OLD
                           ? "Old"
                           : OOP_GET_FLAGS(oop) & _gst_mem.active_flag
                                 ? "To-space"
                                 : "From-space",

           IS_EDEN_ADDR(OOP_TO_OBJ(oop))
               ? "Eden"
               : IS_SURVIVOR_ADDR(OOP_TO_OBJ(oop), 0)
                     ? "Surv (Even)"
                     : IS_SURVIVOR_ADDR(OOP_TO_OBJ(oop), 1)
                           ? "Surv (Odd)"
                           : OOP_GET_FLAGS(oop) & F_POOLED
                                 ? "Pooled"
                                 : OOP_GET_FLAGS(oop) & F_REACHABLE
                                       ? "Old/marked"
                                       : "Old");

    if (IS_OOP_ADDR(OBJ_CLASS(OOP_TO_OBJ(oop)))) {
      printf("   %O (%O)\n", OBJ_CLASS(OOP_TO_OBJ(oop)),
             OBJ_SIZE (OOP_TO_OBJ(oop)));
    } else {
      printf("   (invalid class)\n");
}
  }
}

void _gst_display_object(gst_object obj) {
  if (IS_OOP_ADDR(obj)) {
    printf("Parameter %p appears to be an OOP!\n", obj);
    return;
  }

  printf("Object at %p (%s)", obj,
         IS_EDEN_ADDR(obj) ? "Eden"
                           : IS_SURVIVOR_ADDR(obj, 0)
                                 ? "Even"
                                 : IS_SURVIVOR_ADDR(obj, 1) ? "Odd" : "Old");

  if (IS_OOP_ADDR(OBJ_CLASS(obj))) {
    printf(", size %O (%zu OOPs), class %O\n", OBJ_SIZE (obj), NUM_OOPS(obj),
           OBJ_CLASS(obj));
  } else {
    printf(", contains invalid data\n");
}
}

void _gst_init_snprintfv() {
  register_printf_specifier('O', printf_oop, printf_oop_arginfo);
}
