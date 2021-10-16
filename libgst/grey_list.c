#include "gstpriv.h"

void add_to_grey_list(OOP *base, size_t n) {
  grey_area_node *entry = (grey_area_node *)xmalloc(sizeof(grey_area_node));
  entry->base = base;
  entry->n = n;
  entry->oop = NULL;
  entry->next = NULL;
  if (_gst_mem.grey_pages.tail) {
    _gst_mem.grey_pages.tail->next = entry;
  } else {
    _gst_mem.grey_pages.head = entry;
}

  _gst_mem.grey_pages.tail = entry;
}


