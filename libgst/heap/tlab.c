#include <gstpriv.h>

/* Each threads has a TLAB entry of 100 Ko */
static const uint16_t TLAB_ENTRY_SIZE = 100 * 1024 / SIZEOF_OOP;

size_t get_total_of_tlab(gst_heap_t *heap) {
  return (heap->meta_inf.free_space / SIZEOF_OOP) / TLAB_ENTRY_SIZE;
}

void gst_tlab_init_for_heap(gst_heap_t *heap) {
  gst_heap_t *current_heap = NULL;

  if (!heap) {
    nomemory(1);
    return;
  }

  current_heap = heap;

  while (current_heap) {
    gst_tlab_init_for_local_heap(current_heap);
    current_heap = current_heap->meta_inf.next_heap_area;
  }
}

void gst_tlab_init_for_local_heap(gst_heap_t *heap) {
  gst_tlab_t *tlab;
  size_t nb_of_tlab;

  if (!heap) {
    nomemory(1);
    return;
  }

  if (heap->meta_inf.reserved_for_allocator) {
    nomemory(1);
    return;
  }

  nb_of_tlab = get_total_of_tlab(heap);

  tlab = xcalloc(nb_of_tlab, sizeof(*tlab));

  heap->meta_inf.reserved_for_allocator = tlab;

  gst_tlab_reset_for_local_heap(heap);
}

void gst_tlab_reset_for_local_heap(gst_heap_t *heap) {
  gst_tlab_t *tlab;
  size_t nb_of_tlab;
  size_t last_space_reminder;

  if (!heap) {
    nomemory(1);
    return;
  }

  if (!heap->meta_inf.reserved_for_allocator) {
    nomemory(1);
    return;
  }

  nb_of_tlab = get_total_of_tlab(heap);
  last_space_reminder = (heap->meta_inf.free_space / SIZEOF_OOP) % TLAB_ENTRY_SIZE;
  tlab = heap->meta_inf.reserved_for_allocator;

  for (size_t i = 0; i < nb_of_tlab; i++) {
    tlab[i].thread_id = UINT16_MAX;
    tlab[i].position = &heap->oop[i * TLAB_ENTRY_SIZE];
    tlab[i].end_of_buffer = &heap->oop[(i + 1) * TLAB_ENTRY_SIZE];
  }

  /* TODO Skipp if too small */
  if (last_space_reminder != 0) {
    tlab[nb_of_tlab - 1].thread_id = UINT16_MAX;
    tlab[nb_of_tlab - 1].position = &heap->oop[(nb_of_tlab - 1) * TLAB_ENTRY_SIZE];
    tlab[nb_of_tlab - 1].end_of_buffer = &heap->oop[((nb_of_tlab - 1) * TLAB_ENTRY_SIZE) + last_space_reminder];
  }
}

gst_tlab_t *gst_allocate_in_heap(gst_heap_t *heap, uint16_t current_thread_id) {

  if (!heap) {
    nomemory(1);
    return NULL;
  }

  do {
    gst_tlab_t *tlab;
    const size_t nb_of_tlab = get_total_of_tlab(heap);

    if (!(tlab = heap->meta_inf.reserved_for_allocator)) {
      nomemory(1);
      return NULL;
    }

    for (size_t i = 0; i < nb_of_tlab; i++) {
      uint16_t expected = UINT16_MAX;
      if (atomic_compare_exchange_strong(&tlab[i].thread_id, &expected, current_thread_id) == true) {
        return &tlab[i];
      }
    }

    heap = heap->meta_inf.next_heap_area;

  } while (NULL != heap);

  return NULL;
}

OOP *gst_allocate_in_lab(gst_heap_t *heap, gst_tlab_t **tlab, uint16_t current_thread_id, size_t number_of_words) {

 start:

  {
    OOP *position = (*tlab)->position;
    OOP *allocated = (*tlab)->position + number_of_words;

    if (UNCOMMON (allocated >= (*tlab)->end_of_buffer)) {
      *tlab = gst_allocate_in_heap(heap, current_thread_id);

      if (UNCOMMON (NULL == tlab)) {
        _gst_scavenge();
         *tlab = gst_allocate_in_heap(heap, current_thread_id);
      }

      goto start;
    }

    (*tlab)->position = allocated;

    return position;
  }
}
