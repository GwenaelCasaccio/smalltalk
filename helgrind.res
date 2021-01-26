==653381== Helgrind, a thread error detector
==653381== Copyright (C) 2007-2017, and GNU GPL'd, by OpenWorks LLP et al.
==653381== Using Valgrind-3.15.0 and LibVEX; rerun with -h for copyright info
==653381== Command: /home/gwenael/Personnel/smalltalk/.libs/gst multicore.st
==653381== 
gst: Aborted
gst: Error occurred while not in byte code interpreter!!
/home/gwenael/Personnel/smalltalk/libgst/.libs/libgst.so.7(+0xc368d)[0x491368d]
/lib/x86_64-linux-gnu/libc.so.6(+0x46210)[0x4c53210]
/lib/x86_64-linux-gnu/libc.so.6(gsignal+0xcb)[0x4c5318b]
/lib/x86_64-linux-gnu/libc.so.6(abort+0x12b)[0x4c32859]
/home/gwenael/Personnel/smalltalk/libgst/.libs/libgst.so.7(+0x3891a)[0x488891a]
/usr/lib/x86_64-linux-gnu/libsigsegv.so.2(+0xe3c)[0x4dffe3c]
/lib/x86_64-linux-gnu/libc.so.6(+0x46210)[0x4c53210]
/home/gwenael/Personnel/smalltalk/libgst/.libs/libgst.so.7(+0x355c2)[0x48855c2]
/home/gwenael/Personnel/smalltalk/libgst/.libs/libgst.so.7(+0x333ff)[0x48833ff]
/home/gwenael/Personnel/smalltalk/libgst/.libs/libgst.so.7(+0x33489)[0x4883489]
==653381== 
==653381== Process terminating with default action of signal 6 (SIGABRT)
==653381==    at 0x4C5318B: raise (raise.c:51)
==653381==    by 0x4C32858: abort (abort.c:79)
==653381==    by 0x4888919: oldspace_sigsegv_handler (oop.c:885)
==653381==    by 0x4DFFE3B: ??? (in /usr/lib/x86_64-linux-gnu/libsigsegv.so.2.0.5)
==653381==    by 0x4C5320F: ??? (in /lib/x86_64-linux-gnu/libc-2.31.so)
==653381==    by 0x48855C1: _gst_dictionary_add (dict.c:1642)
==653381==    by 0x48833FE: add_smalltalk (dict.c:933)
==653381==    by 0x4883488: init_runtime_objects (dict.c:952)
==653381==    by 0x4884463: _gst_init_dictionary_on_image_load (dict.c:1182)
==653381==    by 0x48AA18A: load_snapshot (save.c:485)
==653381==    by 0x48A9F3A: _gst_load_from_file (save.c:422)
==653381==    by 0x4866262: _gst_initialize (files.c:478)
==653381== 
==653381== Use --history-level=approx or =none to gain increased speed, at
==653381== the cost of reduced accuracy of conflicting-access information
==653381== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
