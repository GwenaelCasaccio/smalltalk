#include "gstpriv.h"

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
      int errsv = errno;

      if (errsv == EAGAIN) {
        goto restart;
      }

      perror("error while trying to generate random number for sip hash");
      nomemory(true);
      return;
    }

    INSTANCE_VARIABLE(_gst_key_hash_oop, i) = FROM_INT(randomKey);
  }
}
