// Small header for Tom's base64 functions

#ifndef _BASE64_H_
#define _BASE64_H_

#define CRYPT_OK 0
#define CRYPT_INVALID_PACKET 1
#define CRYPT_BUFFER_OVERFLOW 2

#ifndef NULL
#define NULL 0
#endif

#define LTC_ARGCHK(...)

#ifdef __cplusplus
extern "C" {
#endif
    int base64_decode(const unsigned char *in,  unsigned long inlen,
                        unsigned char *out, unsigned long *outlen);
    int base64_encode(const unsigned char *in,  unsigned long inlen,
                        unsigned char *out, unsigned long *outlen);
#ifdef __cplusplus
}
#endif

#endif
