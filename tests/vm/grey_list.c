#include "libgst/gstpriv.h"
#include "libgst/forward_object.h"
#include "libgst/object_pointer.h"
#include "libgst/oop.h"
#include "libgst/dict.h"

int main() {
  OOP forward_object;
  OOP next_forward_object;

  printf ("GREY LIST: ");

  forward_object = malloc (sizeof (*forward_object));

  add_to_grey_list (&forward_object, 4);

  if (!_gst_mem.grey_pages.head) {
    abort ();
  }

  if (_gst_mem.grey_pages.head != _gst_mem.grey_pages.tail) {
    abort ();
  }

  if (_gst_mem.grey_pages.head->base != &forward_object) {
    abort ();
  }

  if (_gst_mem.grey_pages.head->n != 4) {
    abort ();
  }

  if (_gst_mem.grey_pages.head->oop != NULL) {
    abort ();
  }

  if (_gst_mem.grey_pages.head->next != NULL) {
    abort ();
  }

  next_forward_object = malloc (sizeof (*next_forward_object));

  add_to_grey_list (&next_forward_object, 10);

  if (!_gst_mem.grey_pages.head) {
    abort ();
  }

  if (_gst_mem.grey_pages.tail->base != &next_forward_object) {
    abort ();
  }

  if (_gst_mem.grey_pages.tail->n != 10) {
    abort ();
  }

  if (_gst_mem.grey_pages.tail->oop != NULL) {
    abort ();
  }

  if (_gst_mem.grey_pages.tail->next != NULL) {
    abort ();
  }

  if (_gst_mem.grey_pages.head->base != &forward_object) {
    abort ();
  }

  if (_gst_mem.grey_pages.head->n != 4) {
    abort ();
  }

  if (_gst_mem.grey_pages.head->oop != NULL) {
    abort ();
  }

  if (_gst_mem.grey_pages.head->next != _gst_mem.grey_pages.tail) {
    abort ();
  }

  return 0;
}
