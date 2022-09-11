
#ifndef GST_SIP_HASH_H
#define GST_SIP_HASH_H

extern int halfsiphash(const void *in, const size_t inlen, const void *k,
                       uint8_t *out, const size_t outlen);

extern int siphash(const void *in, const size_t inlen, const void *k,
                   uint8_t *out, const size_t outlen);

#endif /* GST_SIP_HASH_H */

