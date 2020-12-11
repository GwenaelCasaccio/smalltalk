#include "libgst/gstpriv.h"
#include "libgst/forward_object.h"
#include "libgst/object_pointer.h"
#include "libgst/oop.h"
#include "libgst/dict.h"

int main() {
  printf ("TEST FROM_INT & TO_INT: ");

#if SIZEOF_OOP == 4
  abort ();
#endif

#if SIZEOF_OOP == 8
  printf ("64 bits\n");
#endif

  printf ("FROM_INT & TO_INT\n");
  if (TO_INT(FROM_INT (0x123456789)) != 0x123456789) {
    return -1;
  }

  printf ("FROM_INT & IS_INT\n");
  if (!IS_INT (FROM_INT (0x123456789))) {
    return -1;
  }

  printf ("FROM_INT && OOP_INT_CLASS\n");
  gst_small_integer_class = (OOP) 0xBABA;
  if (OOP_INT_CLASS(FROM_INT (0X123456789)) != (OOP) 0xBABA) {
    return -1;
  }

  return 0;
}
