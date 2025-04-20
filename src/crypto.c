#include "crypto.h"

/********************/
/* Begin generate_aes_key_iv() */
/*********************/
void generate_aes_key_iv(){
    RAND_bytes(aes_key, sizeof(aes_key));
    RAND_bytes(aes_iv, sizeof(aes_iv));
}
/* End generate_aes_key_iv() */
/*****************************************************************/

/**********************/
/* Begin aes_encrypt()*/
/**********************/
int aes_encrypt(const unsigned char *plaintxt, int plaintxt_len, unsigned char *ciphertxt){
    EVP_CIPHER_CTX *ctx;
    int len, ciphertxt_len;

    // intialize the cipher context
    if(!(ctx = EVP_CIPHER_CTX_new())){
        perror("!! CTX not initialized");
    }
    // initialize the encryptiong operation
    if(EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, aes_key, aes_iv) != 1){
        perror("!! Encrypt Op not Initialized");
    }
    // Provide message to be encrypted
    if(EVP_EncryptUpdate(ctx, ciphertxt, &len, plaintxt, plaintxt_len) != 1){
        perror("!! Error encrypting message");
    }
    ciphertxt_len = len;
    // Finalize encryption
    if(EVP_EncryptFinal_ex(ctx, ciphertxt + len, &len) != 1){
        perror("!!Error Finalizing encryption");
    }
    ciphertxt_len += len;

    // Clean up
    EVP_CIPHER_CTX_free(ctx);
    return ciphertxt_len;

}
/* End aes_encrypt()*/
/*****************************************************************************/

/**********************/
/* Begin aes_decrypt()*/
/**********************/
int aes_decrypt(const unsigned char *ciphertxt, int ciphertxt_len, unsigned char *plaintxt){
    EVP_CIPHER_CTX *ctx;
    int len, plaintxt_len;

    // intialize the cipher context
    if(!(ctx = EVP_CIPHER_CTX_new())){
        perror("!! CTX not initialized");
    }
    // initialize the decryption operation
    if(EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, aes_key, aes_iv) != 1){
        perror("!! Decrypt Op not Initialized");
    }
    // Provide message to be decrypted
    if(EVP_DecryptUpdate(ctx, plaintxt, &len, ciphertxt, ciphertxt_len) != 1){
        perror("!! Error decrypting message");
    }
    plaintxt_len = len;

    // Finalize encryption
    if(EVP_DecryptFinal_ex(ctx, plaintxt + len, &len) != 1){
        perror("!! Error Finalizing decryption");
    }
    plaintxt_len += len;

    // Clean up
    EVP_CIPHER_CTX_free(ctx);
    return plaintxt_len;

}
/* End aes_decrypt()*/
/*****************************************************************************/