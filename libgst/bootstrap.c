/******************************************************************************
 * Copyright (C) 2022 GST.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 ******************************************************************************/

#include "gstpriv.h"

OOP _gst_processor_scheduler_class = NULL;
OOP _gst_directed_message_class = NULL;
OOP _gst_magnitude_class = NULL;
OOP _gst_time_class = NULL;
OOP _gst_date_class = NULL;
OOP _gst_link_class = NULL;
OOP _gst_iterable_class = NULL;
OOP _gst_sequenceable_collection_class = NULL;
OOP _gst_linked_list_class = NULL;
OOP _gst_weak_array_class = NULL;
OOP _gst_ordered_collection_class = NULL;
OOP _gst_sorted_collection_class = NULL;
OOP _gst_hashed_collection_class = NULL;
OOP _gst_set_class = NULL;
OOP _gst_weak_set_class = NULL;
OOP _gst_identity_set_class = NULL;
OOP _gst_weak_identity_set_class = NULL;
OOP _gst_weak_key_dictionary_class = NULL;
OOP _gst_weak_key_identity_dictionary_class = NULL;
OOP _gst_lookup_table_class = NULL;
OOP _gst_weak_value_lookup_table_class = NULL;
OOP _gst_weak_value_identity_dictionary_class = NULL;
OOP _gst_stream_class = NULL;
OOP _gst_positionable_stream_class = NULL;
OOP _gst_read_stream_class = NULL;
OOP _gst_write_stream_class = NULL;
OOP _gst_read_write_stream_class = NULL;
OOP _gst_memory_class = NULL;

/* this must be big enough that the Smalltalk dictionary does not have to
   grow between the time gst_dictionary is loaded and the time the kernel is
   initialized.  Otherwise some of the methods needed to grow the dictionary
   might not be defined yet!!  */
#define INITIAL_SMALLTALK_SIZE 512

static const char *feature_strings[] = {
#ifdef ENABLE_DLD
    "DLD",
#endif
    NULL};

typedef struct class_definition {
  OOP *classVar;
  OOP *superClassPtr;
  intptr_t instanceSpec;
  int numFixedFields;
  const char *name;
  const char *instVarNames;
  const char *classVarNames;
  const char *sharedPoolNames;
} class_definition;

/* The class definition structure.  From this structure, the initial
   set of Smalltalk classes are defined.  */

#define GST_PROCESSOR_SCHEDULER_NUMBER_INSTANCE_VARIABLES 9

static const class_definition class_info[] = {
    {&_gst_object_class, &_gst_nil_oop, GST_ISP_FIXED, 0, "Object", NULL,
     "Dependencies FinalizableObjects", "VMPrimitives"},

    {&_gst_object_memory_class, &_gst_object_class, GST_ISP_FIXED, 34,
     "ObjectMemory",
     "bytesPerOOP bytesPerOTE "
     "edenSize survSpaceSize oldSpaceSize fixedSpaceSize "
     "edenUsedBytes survSpaceUsedBytes oldSpaceUsedBytes "
     "fixedSpaceUsedBytes rememberedTableEntries "
     "numScavenges numGlobalGCs numCompactions numGrowths "
     "numOldOOPs numFixedOOPs numWeakOOPs numOTEs numFreeOTEs "
     "timeBetweenScavenges timeBetweenGlobalGCs timeBetweenGrowths "
     "timeToScavenge timeToCollect timeToCompact "
     "reclaimedBytesPerScavenge tenuredBytesPerScavenge "
     "reclaimedBytesPerGlobalGC reclaimedPercentPerScavenge "
     "allocFailures allocMatches allocSplits allocProbes",
     NULL, NULL},

    {&_gst_message_class, &_gst_object_class, GST_ISP_FIXED, 2, "Message",
     "selector args", NULL, NULL},

    {&_gst_directed_message_class, &_gst_message_class, GST_ISP_FIXED, 1,
     "DirectedMessage", "receiver", NULL, NULL},

    {&_gst_message_lookup_class, &_gst_message_class, GST_ISP_FIXED, 1,
     "MessageLookup", "startingClass", NULL, NULL},

    {&_gst_magnitude_class, &_gst_object_class, GST_ISP_FIXED, 0,
     "Magnitude", NULL, NULL, NULL},

    {&_gst_char_class, &_gst_magnitude_class, GST_ISP_FIXED, 1,
     "Character", "codePoint", "Table UpperTable LowerTable", NULL},

    {&_gst_unicode_character_class, &_gst_char_class, GST_ISP_FIXED, 0,
     "UnicodeCharacter", NULL, NULL, NULL},

    {&_gst_time_class, &_gst_magnitude_class, GST_ISP_FIXED, 1, "Time",
     "seconds", "SecondClockAdjustment ClockOnStartup ClockOnImageSave", NULL},

    {&_gst_date_class, &_gst_magnitude_class, GST_ISP_FIXED, 4, "Date",
     "days day month year", "DayNameDict MonthNameDict", NULL},

    {&_gst_number_class, &_gst_magnitude_class, GST_ISP_FIXED, 0,
     "Number", NULL, NULL, NULL},

    {&_gst_float_class, &_gst_number_class, GST_ISP_UCHAR, 0, "Float",
     NULL, NULL, "CSymbols"},

    {&_gst_floatd_class, &_gst_float_class, GST_ISP_UCHAR, 0, "FloatD",
     NULL, NULL, "CSymbols"},

    {&_gst_floate_class, &_gst_float_class, GST_ISP_UCHAR, 0, "FloatE",
     NULL, NULL, "CSymbols"},

    {&_gst_floatq_class, &_gst_float_class, GST_ISP_UCHAR, 0, "FloatQ",
     NULL, NULL, "CSymbols"},

    {&_gst_fraction_class, &_gst_number_class, GST_ISP_FIXED, 2,
     "Fraction", "numerator denominator", "Zero One", NULL},

    {&_gst_integer_class, &_gst_number_class, GST_ISP_FIXED, 0, "Integer",
     NULL, NULL, "CSymbols"},

    {&gst_small_integer_class, &_gst_integer_class, GST_ISP_FIXED, 0,
     "SmallInteger", NULL, NULL, NULL},

    {&_gst_large_integer_class, &_gst_integer_class, GST_ISP_UCHAR, 0,
     "LargeInteger", NULL,
     "Zero One ZeroBytes OneBytes LeadingZeros TrailingZeros", NULL},

    {&_gst_large_positive_integer_class, &_gst_large_integer_class,
     GST_ISP_UCHAR, 0, "LargePositiveInteger", NULL, NULL, NULL},

    {&_gst_large_zero_integer_class, &_gst_large_positive_integer_class,
     GST_ISP_UCHAR, 0, "LargeZeroInteger", NULL, NULL, NULL},

    {&_gst_large_negative_integer_class, &_gst_large_integer_class,
     GST_ISP_UCHAR, 0, "LargeNegativeInteger", NULL, NULL, NULL},

    {&_gst_lookup_key_class, &_gst_magnitude_class, GST_ISP_FIXED, 1,
     "LookupKey", "key", NULL, NULL},

    {&_gst_deferred_variable_binding_class, &_gst_lookup_key_class,
     GST_ISP_FIXED, 4, "DeferredVariableBinding",
     "class defaultDictionary association path", NULL, NULL},

    {&_gst_association_class, &_gst_lookup_key_class, GST_ISP_FIXED, 1,
     "Association", "value", NULL, NULL},

    {&_gst_homed_association_class, &_gst_association_class, GST_ISP_FIXED,
     1, "HomedAssociation", "environment", NULL, NULL},

    {&_gst_variable_binding_class, &_gst_homed_association_class, GST_ISP_FIXED,
     0, "VariableBinding", NULL, NULL, NULL},

    {&_gst_link_class, &_gst_object_class, GST_ISP_FIXED, 1, "Link",
     "nextLink", NULL, NULL},

    {&_gst_process_class, &_gst_link_class, GST_ISP_FIXED, 8, "Process",
     "suspendedContext priority myList name environment interrupts "
     "interruptLock processorScheduler",
     NULL, NULL},

    {&_gst_callin_process_class, &_gst_process_class, GST_ISP_FIXED, 1,
     "CallinProcess", "returnedValue", NULL, NULL},

    {&_gst_sym_link_class, &_gst_link_class, GST_ISP_FIXED, 1, "SymLink",
     "symbol", NULL, NULL},

    {&_gst_iterable_class, &_gst_object_class, GST_ISP_FIXED, 0,
     "Iterable", NULL, NULL, NULL},

    {&_gst_collection_class, &_gst_iterable_class, GST_ISP_FIXED, 0,
     "Collection", NULL, NULL, NULL},

    {&_gst_sequenceable_collection_class, &_gst_collection_class, GST_ISP_FIXED,
     0, "SequenceableCollection", NULL, NULL, NULL},

    {&_gst_linked_list_class, &_gst_sequenceable_collection_class,
     GST_ISP_FIXED, 2, "LinkedList", "firstLink lastLink", NULL, NULL},

    {&_gst_semaphore_class, &_gst_linked_list_class, GST_ISP_FIXED, 3,
     "Semaphore", "signals name lockThreadId", NULL, NULL},

    {&_gst_arrayed_collection_class, &_gst_sequenceable_collection_class,
     GST_ISP_POINTER, 0, "ArrayedCollection", NULL, NULL, NULL},

    {&_gst_array_class, &_gst_arrayed_collection_class, GST_ISP_POINTER,
     0, "Array", NULL, NULL, NULL},

    {&_gst_weak_array_class, &_gst_array_class, GST_ISP_FIXED, 2,
     "WeakArray", "values nilValues", NULL, NULL},

    {&_gst_character_array_class, &_gst_arrayed_collection_class, GST_ISP_ULONG,
     0, "CharacterArray", NULL, NULL, NULL},

    {&_gst_string_class, &_gst_character_array_class, GST_ISP_CHARACTER,
     0, "String", NULL, NULL, NULL},

    {&_gst_unicode_string_class, &_gst_character_array_class, GST_ISP_UTF32,
     0, "UnicodeString", NULL, NULL, NULL},

    {&_gst_symbol_class, &_gst_string_class, GST_ISP_CHARACTER, 0,
     "Symbol", NULL, NULL, NULL},

    {&_gst_byte_array_class, &_gst_arrayed_collection_class, GST_ISP_UCHAR, 0, "ByteArray", NULL, NULL, "CSymbols"},

    {&_gst_compiled_code_class, &_gst_arrayed_collection_class, GST_ISP_UCHAR, 2, "CompiledCode", "literals header", NULL, NULL},

    {&_gst_compiled_block_class, &_gst_compiled_code_class, GST_ISP_UCHAR,
     1, "CompiledBlock", "method", NULL, NULL},

    {&_gst_compiled_method_class, &_gst_compiled_code_class, GST_ISP_UCHAR, 1, "CompiledMethod", "descriptor ", NULL, NULL},

    {&_gst_interval_class, &_gst_arrayed_collection_class, GST_ISP_FIXED,
     3, "Interval", "start stop step", NULL, NULL},

    {&_gst_ordered_collection_class, &_gst_sequenceable_collection_class,
     GST_ISP_POINTER, 2, "OrderedCollection", "firstIndex lastIndex",
     NULL, NULL},

    {&_gst_sorted_collection_class, &_gst_ordered_collection_class,
     GST_ISP_POINTER, 3, "SortedCollection",
     "lastOrdered sorted sortBlock", "DefaultSortBlock", NULL},

    {&_gst_hashed_collection_class, &_gst_collection_class, GST_ISP_POINTER, 1, "HashedCollection", "tally", NULL, NULL},

    {&_gst_set_class, &_gst_hashed_collection_class, GST_ISP_POINTER, 0,
     "Set", NULL, NULL, NULL},

    {&_gst_weak_set_class, &_gst_set_class, GST_ISP_POINTER, 0,
     "WeakSet", NULL, NULL, NULL},

    {&_gst_identity_set_class, &_gst_set_class, GST_ISP_POINTER, 0,
     "IdentitySet", NULL, NULL, NULL},

    {&_gst_weak_identity_set_class, &_gst_weak_set_class, GST_ISP_POINTER,
     0, "WeakIdentitySet", NULL, NULL, NULL},

    {&_gst_dictionary_class, &_gst_hashed_collection_class, GST_ISP_POINTER,
     0, "Dictionary", NULL, NULL, NULL},

    {&_gst_weak_key_dictionary_class, &_gst_dictionary_class, GST_ISP_POINTER,
     1, "WeakKeyDictionary", "keys", NULL, NULL},

    {&_gst_weak_key_identity_dictionary_class, &_gst_weak_key_dictionary_class,
     GST_ISP_POINTER, 0, "WeakKeyIdentityDictionary", NULL, NULL, NULL},

    {&_gst_lookup_table_class, &_gst_dictionary_class, GST_ISP_POINTER,
     0, "LookupTable", NULL, NULL, NULL},

    {&_gst_weak_value_lookup_table_class, &_gst_lookup_table_class,
     GST_ISP_POINTER, 1, "WeakValueLookupTable", "values", NULL, NULL},

    {&_gst_weak_value_identity_dictionary_class,
     &_gst_weak_value_lookup_table_class, GST_ISP_POINTER, 0,
     "WeakValueIdentityDictionary", NULL, NULL, NULL},

    {&_gst_identity_dictionary_class, &_gst_lookup_table_class, GST_ISP_POINTER,
     0, "IdentityDictionary", NULL, NULL, NULL},

    {&_gst_method_dictionary_class, &_gst_identity_dictionary_class,
     GST_ISP_FIXED, 2, "MethodDictionary", "keys values", NULL, NULL},

    /* These five MUST have the same structure as dictionary; they're
       used interchangeably within the C portion of the system */
    {&_gst_binding_dictionary_class, &_gst_dictionary_class, GST_ISP_POINTER,
     1, "BindingDictionary", "environment", NULL, NULL},

    {&_gst_abstract_namespace_class, &_gst_binding_dictionary_class,
     GST_ISP_POINTER, 3, "AbstractNamespace",
     "name subspaces sharedPools", NULL, NULL},

    {&_gst_root_namespace_class, &_gst_abstract_namespace_class,
     GST_ISP_POINTER, 0, "RootNamespace", NULL, NULL, NULL},

    {&_gst_namespace_class, &_gst_abstract_namespace_class, GST_ISP_POINTER,
     0, "Namespace", NULL, "Current", NULL},

    {&_gst_system_dictionary_class, &_gst_root_namespace_class, GST_ISP_POINTER,
     0, "SystemDictionary", NULL, NULL, NULL},

    {&_gst_stream_class, &_gst_iterable_class, GST_ISP_FIXED, 0,
     "Stream", NULL, NULL, NULL},

    {&_gst_positionable_stream_class, &_gst_stream_class, GST_ISP_FIXED,
     4, "PositionableStream", "collection ptr endPtr access", NULL, NULL},

    {&_gst_read_stream_class, &_gst_positionable_stream_class, GST_ISP_FIXED,
     0, "ReadStream", NULL, NULL, NULL},

    {&_gst_write_stream_class, &_gst_positionable_stream_class, GST_ISP_FIXED,
     0, "WriteStream", NULL, NULL, NULL},

    {&_gst_read_write_stream_class, &_gst_write_stream_class, GST_ISP_FIXED,
     0, "ReadWriteStream", NULL, NULL, NULL},

    {&_gst_file_descriptor_class, &_gst_stream_class, GST_ISP_FIXED, 6,
     "FileDescriptor", "access fd file isPipe atEnd peek", "AllOpenFiles",
     NULL},

    {&_gst_file_stream_class, &_gst_file_descriptor_class, GST_ISP_FIXED,
     5, "FileStream", "collection ptr endPtr writePtr writeEnd",
     "Verbose Record Includes", NULL},

    {&_gst_undefined_object_class, &_gst_object_class, GST_ISP_FIXED, 0,
     "UndefinedObject", NULL, NULL, "Smalltalk"},

    {&_gst_boolean_class, &_gst_object_class, GST_ISP_FIXED, 0, "Boolean",
     NULL, NULL, NULL},

    {&_gst_false_class, &_gst_boolean_class, GST_ISP_FIXED, 1, "False",
     "truthValue", NULL, NULL},

    {&_gst_true_class, &_gst_boolean_class, GST_ISP_FIXED, 1, "True",
     "truthValue", NULL, NULL},

    {&_gst_processor_scheduler_class, &_gst_object_class, GST_ISP_FIXED,
     GST_PROCESSOR_SCHEDULER_NUMBER_INSTANCE_VARIABLES, "ProcessorScheduler",
     "processLists activeProcess idleTasks processTimeslice gcSemaphore "
     "gcArray eventSemaphore vmThreadId lockThreadId",
     NULL, NULL},

    /* Change this, classDescription, or gst_class, and you must change
       the implementaion of new_metaclass some */
    {&_gst_behavior_class, &_gst_object_class, GST_ISP_FIXED, 5,
     "Behavior",
     "superClass methodDictionary instanceSpec subClasses instanceVariables",
     NULL, NULL},

    {&_gst_class_description_class, &_gst_behavior_class, GST_ISP_FIXED,
     0, "ClassDescription", NULL, NULL, NULL},

    {&_gst_class_class, &_gst_class_description_class, GST_ISP_FIXED, 7,
     "Class",
     "name comment category environment classVariables sharedPools "
     "pragmaHandlers",
     NULL, NULL},

    {&_gst_metaclass_class, &_gst_class_description_class, GST_ISP_FIXED,
     1, "Metaclass", "instanceClass", NULL, NULL},

    {&_gst_context_part_class, &_gst_object_class, GST_ISP_POINTER, 6,
     "ContextPart", "parent nativeIP ip sp receiver method ", NULL, NULL},

    {&_gst_method_context_class, &_gst_context_part_class, GST_ISP_POINTER,
     1, "MethodContext", "flags ", NULL, NULL},

    {&_gst_block_context_class, &_gst_context_part_class, GST_ISP_POINTER,
     1, "BlockContext", "outerContext ", NULL, NULL},

    {&_gst_continuation_class, &_gst_object_class, GST_ISP_FIXED, 1,
     "Continuation", "stack ", NULL, NULL},

    {&_gst_block_closure_class, &_gst_object_class, GST_ISP_FIXED, 3,
     "BlockClosure", "outerContext block receiver", NULL, NULL},

    {&_gst_c_object_class, &_gst_object_class, GST_ISP_ULONG, 2,
     "CObject", "type storage", NULL, "CSymbols"},

    {&_gst_c_type_class, &_gst_object_class, GST_ISP_FIXED, 1, "CType",
     "cObjectType", NULL, NULL},

    {&_gst_c_callable_class, &_gst_c_object_class, GST_ISP_ULONG, 2,
     "CCallable", "returnType argTypes", NULL, NULL},

    {&_gst_c_func_descriptor_class, &_gst_c_callable_class, GST_ISP_ULONG,
     1, "CFunctionDescriptor", "cFunctionName", NULL, NULL},

    {&_gst_c_callback_descriptor_class, &_gst_c_callable_class, GST_ISP_ULONG,
     1, "CCallbackDescriptor", "block", NULL, NULL},

    {&_gst_memory_class, &_gst_object_class, GST_ISP_FIXED, 0, "Memory",
     NULL, NULL, NULL},

    {&_gst_method_info_class, &_gst_object_class, GST_ISP_POINTER, 5,
     "MethodInfo", "sourceCode category class selector debugInfo", NULL, NULL},

    {&_gst_file_segment_class, &_gst_object_class, GST_ISP_FIXED, 3,
     "FileSegment", "file startPos size", NULL, NULL},

    {&_gst_debug_information_class, &_gst_object_class, GST_ISP_FIXED, 1,
     "DebugInformation", "variables", NULL, NULL},

    {&_gst_key_hash_class, &_gst_object_class, GST_ISP_FIXED, 2,
     "KeyHash", "key0 key1", NULL, NULL},

    /* Classes not defined here (like Point/Rectangle/RunArray) are
       defined after the kernel has been fully initialized.  */
};

/* The complete list of "kernel" class and method definitions.  Each
   of these files is loaded, in the order given below.  Their last
   modification dates are compared against that of the image file; if
   any are newer, the image file is ignored, these files are loaded,
   and a new image file is created.

   As a provision for when we'll switch to a shared library, this
   is not an array but a list of consecutive file names.  */
static const char standard_files[] = {
    "Builtins.st\0"
    "SysDict.st\0"
    "Object.st\0"
    "Message.st\0"
    "MessageLookup.st\0"
    "DirMessage.st\0"
    "Boolean.st\0"
    "False.st\0"
    "True.st\0"
    "Magnitude.st\0"
    "LookupKey.st\0"
    "DeferBinding.st\0"
    "Association.st\0"
    "HomedAssoc.st\0"
    "VarBinding.st\0"
    "Integer.st\0"
    "KeyHash.st\0"
    "Date.st\0"
    "Time.st\0"
    "Number.st\0"
    "SmallInt.st\0"
    "Float.st\0"
    "FloatD.st\0"
    "FloatE.st\0"
    "FloatQ.st\0"
    "Fraction.st\0"
    "LargeInt.st\0"
    "Character.st\0"
    "UniChar.st\0"
    "Link.st\0"
    "Process.st\0"
    "CallinProcess.st\0"
    "Iterable.st\0"
    "Collection.st\0"
    "SeqCollect.st\0"
    "LinkedList.st\0"
    "Semaphore.st\0"
    "ArrayColl.st\0"
    "CompildCode.st\0"
    "CompildMeth.st\0"
    "CompiledBlk.st\0"
    "Array.st\0"
    "ByteArray.st\0"
    "ContextPart.st\0"
    "MthContext.st\0"
    "BlkContext.st\0"
    "BlkClosure.st\0"
    "CharArray.st\0"
    "String.st\0"
    "Symbol.st\0"
    "UniString.st\0"
    "Interval.st\0"
    "OrderColl.st\0"
    "SortCollect.st\0"
    "HashedColl.st\0"
    "Set.st\0"
    "IdentitySet.st\0"
    "Dictionary.st\0"
    "LookupTable.st\0"
    "IdentDict.st\0"
    "MethodDict.st\0"
    "BindingDict.st\0"
    "AbstNamespc.st\0"
    "RootNamespc.st\0"
    "Namespace.st\0"
    "Stream.st\0"
    "PosStream.st\0"
    "ReadStream.st\0"
    "WriteStream.st\0"
    "RWStream.st\0"
    "UndefObject.st\0"
    "ProcSched.st\0"
    "Continuation.st\0"
    "Memory.st\0"
    "MethodInfo.st\0"
    "DebugInformation.st\0"
    "FileSegment.st\0"
    "FileDescr.st\0"
    "SymLink.st\0"
    "WeakObjects.st\0"
    "ObjMemory.st\0"
    "KernelInitialize.st\0"
    "Behavior.st\0"
    "ClassDesc.st\0"
    "Class.st\0"
    "Metaclass.st\0"

    /* More core classes */
    "Bag.st\0"
    "MappedColl.st\0"
    "Delay.st\0"
    "SharedQueue.st\0"
    "Random.st\0"
    "RecursionLock.st\0"
    "Transcript.st\0"
    "Point.st\0"
    "Rectangle.st\0"
    "RunArray.st\0"
    "AnsiDates.st\0"
    "ScaledDec.st\0"
    "ValueAdapt.st\0"
    "OtherArrays.st\0"

    /* C call-out facilities */
    "CObject.st\0"
    "CType.st\0"
    "CCallable.st\0"
    "CFuncs.st\0"
    "CCallback.st\0"
    "CStruct.st\0"

    /* Exception handling and ProcessEnvironment */
    "ProcEnv.st\0"
    "ExcHandling.st\0"
    "SysExcept.st\0"

    /* Virtual filesystem layer */
    "FilePath.st\0"
    "File.st\0"
    "Directory.st\0"
    "VFS.st\0"
    "VFSZip.st\0"
    "URL.st\0"
    "FileStream.st\0"

    /* Goodies */
    "DynVariable.st\0"
    "DLD.st\0"
    "Getopt.st\0"
    "Generator.st\0"
    "StreamOps.st\0"
    "Regex.st\0"
    "PkgLoader.st\0"
    "DirPackage.st\0"
    "Autoload.st\0"
    "Initialize.st\0"};

/* Instantiate the OOPs that are created before the first classes
   (true, false, nil, the Smalltalk dictionary, the symbol table
   and Processor, the sole instance of ProcessorScheduler.  */
static void init_proto_oops(void);

static void create_classes_pass1(const class_definition *ci, int n);

static void create_classes_pass2(const class_definition *ci, int n);

static void init_smalltalk_dictionary(void);

static void create_class(const class_definition *ci);

static void create_metaclass(OOP class_oop, int numSubClasses, int numMetaclassSubClasses);

static void init_metaclass(OOP metaclassOOP);

static void init_class(OOP classOOP, const class_definition *ci);

static void add_subclass(OOP superClassOOP, OOP subClassOOP);

static OOP add_smalltalk(const char *globalName, OOP globalValue) {
  NAMESPACE_AT_PUT(_gst_smalltalk_dictionary, _gst_intern_string(globalName),
                   globalValue);

  return globalValue;
}

static OOP namespace_new(int size, const char *name, OOP superspaceOOP) {
  gst_object ns;
  OOP namespaceOOP, classOOP;

  size = new_num_fields(size);
  classOOP =
      IS_NIL(superspaceOOP) ? _gst_root_namespace_class : _gst_namespace_class;

  ns = instantiate_with(classOOP, size, &namespaceOOP);

  OBJ_NAMESPACE_SET_TALLY(ns, FROM_INT(0));
  OBJ_NAMESPACE_SET_SUPER_SPACE(ns, superspaceOOP);
  OBJ_NAMESPACE_SET_SUBSPACES(ns, _gst_nil_oop);
  OBJ_NAMESPACE_SET_NAME(ns, _gst_intern_string(name));

  return (namespaceOOP);
}

static void init_primitives_dictionary(void);

void init_primitives_dictionary() {
  OOP primDictionaryOOP =
      _gst_binding_dictionary_new(512, _gst_smalltalk_dictionary);
  int i;

  add_smalltalk("VMPrimitives", primDictionaryOOP);
  for (i = 0; i < NUM_PRIMITIVES; i++) {
    prim_table_entry *pte = _gst_get_primitive_attributes(i);

    if (pte->name) {
      OOP keyOOP = _gst_intern_string(pte->name);
      OOP valueOOP = FROM_INT(i);
      DICTIONARY_AT_PUT(primDictionaryOOP, keyOOP, valueOOP);
    }
  }
}

void generate_sip_hash_key() {
  _gst_key_hash_oop = alloc_oop(NULL, _gst_mem.active_flag);

  const size_t numWords = OBJ_HEADER_SIZE_WORDS + 2;
  gst_object keyHash = _gst_alloc_words(numWords);
  nil_fill(keyHash->data, numWords - OBJ_HEADER_SIZE_WORDS);
  OOP_SET_OBJECT(_gst_key_hash_oop, keyHash);
  _gst_register_oop(_gst_key_hash_oop);

  for (uint8_t i = 0; i < 2; i++) {
    uintptr_t randomKey;

  restart:

    if (getrandom(&randomKey, sizeof(randomKey), 0) != sizeof(randomKey)) {
      const int errsv = errno;

      if (errsv == EAGAIN) {
        goto restart;
      }

      perror("error while trying to generate random number for sip hash");
      nomemory(true);
      return;
    }

    INSTANCE_VARIABLE(_gst_key_hash_oop, i) = FROM_INT(randomKey);
  }

  MAKE_OOP_READONLY(_gst_key_hash_oop, true);
}

void init_proto_oops() {
  gst_object smalltalkDictionary;
  gst_object symbolTable, processorScheduler;
  int numWords;

  /* Also finish the creation of the OOPs with reserved indices in
     oop.h */

  /* the symbol table ...  */
  numWords = OBJ_HEADER_SIZE_WORDS + SYMBOL_TABLE_SIZE;
  symbolTable = _gst_alloc_words(numWords);
  OOP_SET_OBJECT(_gst_symbol_table, symbolTable);

  OBJ_SET_CLASS(symbolTable, _gst_array_class);
  nil_fill(symbolTable->data, numWords - OBJ_HEADER_SIZE_WORDS);

  /* 5 is the # of fixed instvars in gst_namespace */
  numWords = OBJ_HEADER_SIZE_WORDS + INITIAL_SMALLTALK_SIZE + 5;

  /* ... now the Smalltalk dictionary ...  */
  smalltalkDictionary = _gst_alloc_words(numWords);
  OOP_SET_OBJECT(_gst_smalltalk_dictionary, smalltalkDictionary);

  OBJ_SET_CLASS(smalltalkDictionary, _gst_system_dictionary_class);
  OBJ_NAMESPACE_SET_TALLY(smalltalkDictionary, FROM_INT(0));
  OBJ_NAMESPACE_SET_NAME(smalltalkDictionary, _gst_smalltalk_namespace_symbol);
  OBJ_NAMESPACE_SET_SUPER_SPACE(smalltalkDictionary, _gst_nil_oop);
  OBJ_NAMESPACE_SET_SUBSPACES(smalltalkDictionary, _gst_nil_oop);
  OBJ_NAMESPACE_SET_SHARED_POOLS(smalltalkDictionary, _gst_nil_oop);
  nil_fill(OBJ_NAMESPACE_ASSOC(smalltalkDictionary), INITIAL_SMALLTALK_SIZE);

  /* ... and finally Processor */
  numWords =
      GST_PROCESSOR_SCHEDULER_NUMBER_INSTANCE_VARIABLES + OBJ_HEADER_SIZE_WORDS;
  processorScheduler = _gst_alloc_words(numWords);
  OOP_SET_OBJECT(_gst_processor_oop[0], processorScheduler);

  OBJ_SET_CLASS(processorScheduler, _gst_processor_scheduler_class);
  nil_fill(processorScheduler->data, numWords - OBJ_HEADER_SIZE_WORDS);
  OBJ_PROCESSOR_SCHEDULER_SET_VM_THREAD_ID(processorScheduler, FROM_INT(0));
}

void _gst_init_dictionary(void) {
  memcpy(_gst_primitive_table, _gst_default_primitive_table,
         sizeof(_gst_primitive_table));

  /* The order of this must match the indices defined in oop.h!! */
  _gst_smalltalk_dictionary = alloc_oop(NULL, _gst_mem.active_flag);
  _gst_processor_oop[0] = alloc_oop(NULL, _gst_mem.active_flag);
  _gst_symbol_table = alloc_oop(NULL, _gst_mem.active_flag);

  generate_sip_hash_key();

  _gst_init_symbols_pass1();

  create_classes_pass1(class_info, sizeof(class_info) / sizeof(class_info[0]));

  /* We can do this now that the classes are defined */
  _gst_init_builtin_objects_classes();

  OBJ_SET_CLASS(OOP_TO_OBJ(_gst_key_hash_oop), _gst_key_hash_class);

  init_proto_oops();
  _gst_init_symbols_pass2();
  init_smalltalk_dictionary();

  create_classes_pass2(class_info, sizeof(class_info) / sizeof(class_info[0]));

  init_runtime_objects();
  _gst_tenure_all_survivors();
}

void create_classes_pass1(const class_definition *ci, int n) {
  OOP superClassOOP;
  int nilSubclasses;
  gst_object classObj, superclass;

  for (nilSubclasses = 0; n--; ci++) {
    superClassOOP = *ci->superClassPtr;
    create_class(ci);

    if (IS_NIL(superClassOOP)) {
      nilSubclasses++;
    } else {
      superclass = OOP_TO_OBJ(superClassOOP);
      OBJ_BEHAVIOR_SET_SUB_CLASSES(
          superclass,
          FROM_INT(TO_INT(OBJ_BEHAVIOR_GET_SUB_CLASSES(superclass)) + 1));
    }
  }

  /* Object class being a subclass of gst_class is not an apparent link,
     and so the index which is the number of subclasses of the class
     is off by the number of subclasses of nil.  We correct that here.

     On the other hand, we don't want the meta class to have a subclass
     (`Class class' and `Class' are unique in that they don't have the
     same number of subclasses), so since we have the information here,
     we special case the Class class and create its metaclass here.  */
  classObj = OOP_TO_OBJ(_gst_class_class);
  create_metaclass(
      _gst_class_class, TO_INT(OBJ_BEHAVIOR_GET_SUB_CLASSES(classObj)),
      TO_INT(OBJ_BEHAVIOR_GET_SUB_CLASSES(classObj)) + nilSubclasses);
}

void create_classes_pass2(const class_definition *ci, int n) {
  OOP class_oop;
  gst_object class;
  int numSubclasses;

  for (; n--; ci++) {
    class_oop = *ci->classVar;
    class = OOP_TO_OBJ(class_oop);

    if (!OBJ_CLASS(class)) {
      numSubclasses = TO_INT(OBJ_BEHAVIOR_GET_SUB_CLASSES(class));
      create_metaclass(class_oop, numSubclasses, numSubclasses);
    }

    init_metaclass(OBJ_CLASS(class));
    init_class(class_oop, ci);
  }
}

void create_class(const class_definition *ci) {
  gst_object class;
  intptr_t superInstanceSpec;
  OOP classOOP, superClassOOP;
  int numFixedFields;

  numFixedFields = ci->numFixedFields;
  superClassOOP = *ci->superClassPtr;
  if (!IS_NIL(superClassOOP)) {
    /* adjust the number of instance variables to account for
       inheritance */
    superInstanceSpec = CLASS_INSTANCE_SPEC(superClassOOP);
    numFixedFields += superInstanceSpec >> ISP_NUMFIXEDFIELDS;
  }

  class = _gst_alloc_obj((12 + OBJ_HEADER_SIZE_WORDS) * sizeof(OOP), &classOOP);

  OBJ_SET_CLASS(class, NULL);
  OBJ_BEHAVIOR_SET_SUPER_CLASS(class, superClassOOP);
  OBJ_BEHAVIOR_SET_INSTANCE_SPEC(class,
                                 (OOP)(GST_ISP_INTMARK | ci->instanceSpec |
                                       (numFixedFields << ISP_NUMFIXEDFIELDS)));

  OBJ_BEHAVIOR_SET_SUB_CLASSES(class, FROM_INT(0));

  *ci->classVar = classOOP;
}

void create_metaclass(OOP class_oop, int numMetaclassSubClasses,
                      int numSubClasses) {
  gst_object class;
  gst_object metaclass;
  gst_object subClasses;

  class = OOP_TO_OBJ(class_oop);
  metaclass = new_instance(_gst_metaclass_class, &OBJ_CLASS(class));

  OBJ_META_CLASS_SET_INSTANCE_CLASS(metaclass, class_oop);

  subClasses = new_instance_with(_gst_array_class, numSubClasses,
                                 &OBJ_BEHAVIOR_GET_SUB_CLASSES(class));
  if (numSubClasses > 0) {
    subClasses->data[0] = FROM_INT(numSubClasses);
  }

  subClasses = new_instance_with(_gst_array_class, numMetaclassSubClasses,
                                 &OBJ_BEHAVIOR_GET_SUB_CLASSES(metaclass));
  if (numMetaclassSubClasses > 0) {
    subClasses->data[0] = FROM_INT(numMetaclassSubClasses);
  }
}

void init_metaclass(OOP metaclassOOP) {
  gst_object metaclass;
  OOP class_oop, superClassOOP;

  metaclass = OOP_TO_OBJ(metaclassOOP);
  class_oop = OBJ_META_CLASS_GET_INSTANCE_CLASS(metaclass);
  superClassOOP = SUPERCLASS(class_oop);

  if (IS_NIL(superClassOOP)) {
    /* Object case: make this be gst_class to close the circularity */
    OBJ_BEHAVIOR_SET_SUPER_CLASS(metaclass, _gst_class_class);
  } else {
    OBJ_BEHAVIOR_SET_SUPER_CLASS(metaclass, OOP_CLASS(superClassOOP));
  }

  add_subclass(OBJ_BEHAVIOR_GET_SUPER_CLASS(metaclass), metaclassOOP);

  /* the specifications here should match what a class should have:
     instance variable names, the right number of instance variables,
     etc.  We could take three passes, and use the instance variable
     spec for classes once it's established, but it's easier to create
     them here by hand */
  OBJ_BEHAVIOR_SET_INSTANCE_VARIABLES(
      metaclass,
      _gst_make_instance_variable_array(
          _gst_nil_oop, "superClass methodDictionary instanceSpec subClasses "
                        "instanceVariables name comment category environment "
                        "classVariables sharedPools "
                        "pragmaHandlers"));

  OBJ_BEHAVIOR_SET_INSTANCE_SPEC(
      metaclass,
      (OOP)(GST_ISP_INTMARK | GST_ISP_FIXED | (12 << ISP_NUMFIXEDFIELDS)));

  OBJ_BEHAVIOR_SET_METHOD_DICTIONARY(metaclass, _gst_nil_oop);
}

void init_class(OOP class_oop, const class_definition *ci) {
  gst_object class;

  class = OOP_TO_OBJ(class_oop);
  OBJ_CLASS_SET_NAME(class, _gst_intern_string(ci->name));
  add_smalltalk(ci->name, class_oop);

  if (!IS_NIL(OBJ_BEHAVIOR_GET_SUPER_CLASS(class))) {
    add_subclass(OBJ_BEHAVIOR_GET_SUPER_CLASS(class), class_oop);
  }

  OBJ_CLASS_SET_ENVIRONMENT(class, _gst_smalltalk_dictionary);
  OBJ_BEHAVIOR_SET_INSTANCE_VARIABLES(
      class, _gst_make_instance_variable_array(
                 OBJ_BEHAVIOR_GET_SUPER_CLASS(class), ci->instVarNames));
  OBJ_CLASS_SET_CLASS_VARIABLES(
      class, _gst_make_class_variable_dictionary(ci->classVarNames, class_oop));

  OBJ_CLASS_SET_SHARED_POOLS(class, _gst_make_pool_array(ci->sharedPoolNames));

  /* Other fields are set by the Smalltalk code.  */
  OBJ_BEHAVIOR_SET_METHOD_DICTIONARY(class, _gst_nil_oop);
  OBJ_CLASS_SET_COMMENT(class, _gst_nil_oop);
  OBJ_CLASS_SET_CATEGORY(class, _gst_nil_oop);
  OBJ_CLASS_SET_PRAGMA_HANDLERS(class, _gst_nil_oop);
}

void add_subclass(OOP superClassOOP, OOP subClassOOP) {
  gst_object superclass;
  int index;

  superclass = OOP_TO_OBJ(superClassOOP);

#ifndef OPTIMIZE
  if (NUM_WORDS(OOP_TO_OBJ(superclass->subClasses)) == 0) {
    _gst_errorf("Attempt to add subclass to zero sized class");
    abort();
  }
#endif

  index = TO_INT(ARRAY_AT(OBJ_BEHAVIOR_GET_SUB_CLASSES(superclass), 1));
  ARRAY_AT_PUT(OBJ_BEHAVIOR_GET_SUB_CLASSES(superclass), 1,
               FROM_INT(index - 1));
  ARRAY_AT_PUT(OBJ_BEHAVIOR_GET_SUB_CLASSES(superclass), index, subClassOOP);
}

void init_smalltalk_dictionary(void) {
  OOP featuresArrayOOP;
  gst_object featuresArray;
  char fullVersionString[200];
  int i, numFeatures;

  _gst_current_namespace = _gst_smalltalk_dictionary;
  for (numFeatures = 0; feature_strings[numFeatures]; numFeatures++) {
    ;
  }

  featuresArray =
      new_instance_with(_gst_array_class, numFeatures, &featuresArrayOOP);

  for (i = 0; i < numFeatures; i++) {
    featuresArray->data[i] = _gst_intern_string(feature_strings[i]);
  }

  sprintf(fullVersionString, "GNU Smalltalk version %s",
          VERSION PACKAGE_GIT_REVISION);

  add_smalltalk("Smalltalk", _gst_smalltalk_dictionary);
  add_smalltalk("Version", _gst_string_new(fullVersionString));
  add_smalltalk("KernelFilePath", _gst_string_new(_gst_kernel_file_path));
  add_smalltalk("KernelInitialized", _gst_false_oop);
  add_smalltalk("SymbolTable", _gst_symbol_table);
  add_smalltalk("Processor", _gst_processor_oop[0]);
  add_smalltalk("Features", featuresArrayOOP);

  /* Add subspaces */
  add_smalltalk("CSymbols",
                namespace_new(32, "CSymbols", _gst_smalltalk_dictionary));

  init_primitives_dictionary();

  add_smalltalk("Undeclared", namespace_new(32, "Undeclared", _gst_nil_oop));
  add_smalltalk("SystemExceptions", namespace_new(32, "SystemExceptions",
                                                  _gst_smalltalk_dictionary));
  add_smalltalk("NetClients",
                namespace_new(32, "NetClients", _gst_smalltalk_dictionary));
  add_smalltalk("VFS", namespace_new(32, "VFS", _gst_smalltalk_dictionary));

  _gst_init_process_system();
}

int load_standard_files(const char *site_pre_image_file,
                        const char *user_pre_image_file) {
  const char *fileName;

  for (fileName = standard_files; *fileName; fileName += strlen(fileName) + 1) {
    if (!_gst_process_file(fileName, GST_DIR_KERNEL)) {
      _gst_errorf("couldn't load system file '%s': %s", fileName,
                  strerror(errno));
      _gst_errorf("image bootstrap failed, use option --kernel-directory");
      return 1;
    }
  }

  _gst_msg_sendf(NULL, "%v %o relocate", _gst_file_segment_class);

  if (site_pre_image_file) {
    _gst_process_file(site_pre_image_file, GST_DIR_ABS);
  }

  if (user_pre_image_file) {
    _gst_process_file(user_pre_image_file, GST_DIR_ABS);
  }

  return 0;
}
