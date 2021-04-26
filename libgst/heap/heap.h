#ifndef GST_NEW_HEAP_H
#define GST_NEW_HEAP_H

#define heap_size 0x200000

typedef struct gst_heap_meta_inf_s {
  struct gst_heap_s *prev_heap_area;
  struct gst_heap_s *next_heap_area;
  size_t free_space;
  void *reserved_for_allocator;
} gst_heap_meta_inf_t;

typedef struct gst_heap_s {
  gst_heap_meta_inf_t meta_inf;
  OOP oop[(heap_size - sizeof(gst_heap_meta_inf_t)) / SIZEOF_OOP];
} gst_heap_t;

extern void _gst_heap_new_area(gst_heap_t **heap, size_t generation_size);

extern void _gst_heap_free_area(gst_heap_t *heap);

_Static_assert(sizeof(gst_heap_t) == heap_size);

#endif /* GST_NEW_HEAP_H */
