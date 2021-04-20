#ifndef GST_TLAB_H
#define GST_TLAB_H

typedef struct gst_tlab_s {
  _Atomic(uint16_t) thread_id;
  OOP *position;
  OOP *end_of_buffer;
} gst_tlab_t;

void gst_tlab_init_for_heap(gst_heap_t *heap);

void gst_tlab_init_for_local_heap(gst_heap_t *heap);

gst_tlab_t *gst_allocate_in_heap(gst_heap_t *heap, uint16_t current_thread_id);

OOP *gst_allocate_in_lab(gst_heap_t *heap, gst_tlab_t **tlab, uint16_t current_thread_id, size_t number_of_words);

void gst_free_in_heap(gst_heap_t *heap, gst_tlab_t *tlab);

#endif /* GST_TLAB_H */
