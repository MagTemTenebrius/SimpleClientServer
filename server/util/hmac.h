
#ifndef _HMAC_H_
#define _HMAC_H_

#include <stdint.h>
#include <stdlib.h>

#define MD5HashSize 16

void hmac_md5(unsigned char *text, unsigned int text_len, unsigned char *key, unsigned int key_len, unsigned char *digest);

#endif