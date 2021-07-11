#include "../gstpriv.h"

void _gst_heap_new_area(gst_heap_t **heap, size_t generation_size) {
  size_t number_of_heap;
  gst_heap_t *previous_heap = NULL;

  if (NULL == heap) {
    nomemory(true);
    return ;
  }

  if (NULL != *heap) {
    nomemory(true);
    return ;
  }

  number_of_heap = generation_size / heap_size;

  if (0 != generation_size % heap_size) {
    number_of_heap++;
  }

  for (size_t i = 0; i < number_of_heap; i++) {
    gst_heap_t *new_heap = NULL;

    if (posix_memalign((void **)&new_heap, 0x200000, sizeof(gst_heap_t))) {
      perror("error while allocating heap");
      nomemory(true);

      return;
    }

    new_heap->meta_inf.free_space = sizeof(gst_heap_t) - sizeof(gst_heap_meta_inf_t);
    new_heap->meta_inf.prev_heap_area = previous_heap;
    new_heap->meta_inf.next_heap_area = NULL;
    new_heap->meta_inf.reserved_for_allocator = NULL;

    if (NULL == *heap) {
      *heap = new_heap;
    }

    if (NULL != previous_heap) {
      previous_heap->meta_inf.next_heap_area = new_heap;
    }

    previous_heap = new_heap;
  }
}

void _gst_heap_free_area(gst_heap_t *heap) {

  if (NULL == heap) {
    return ;
  }

  while (heap) {
    gst_heap_t *current = heap;
    heap = heap->meta_inf.next_heap_area;

    free(current);
  }
}
