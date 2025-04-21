#ifndef CRYPTO_H
#define CRYPTO_H

#include <openssl/evp.h>
#include <openssl/rand.h>


//Encrption Functions
void generate_aes_key_iv();
int aes_encrypt(const unsigned char *plaintxt, int plaintxt_len, unsigned char *ciphertxt);
int aes_decrypt(const unsigned char *ciphertxt, int ciphertxt_len, unsigned char *plaintxt);

//Cipher variables
// For testing these are hard coded keys and IVs
extern unsigned char aes_key[16];
extern unsigned char aes_iv[16];

#endif // CRYPTO_H

