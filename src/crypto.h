#ifndef CRYPTO_H
#define CRYPTO_H

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <string.h>

// Encryption functions
int set_crypto_key(const unsigned char *key, size_t keylen);
void generate_aes_key_iv();
int aes_encrypt(const unsigned char *plaintxt, int plaintxt_len, unsigned char *ciphertxt);
int aes_decrypt(const unsigned char *ciphertxt, int ciphertxt_len, unsigned char *plaintxt);

// Cipher variables
extern unsigned char aes_key[16];
extern unsigned char aes_iv[16];

#endif // CRYPTO_H

