#include "gstpriv.h"

void generate_sip_hash_key(OOP keyOOP) {
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

    INSTANCE_VARIABLE(keyOOP, i) = FROM_INT(randomKey);
  }
}
