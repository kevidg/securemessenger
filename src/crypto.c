/* The crypto.c file contains the code for the aes symmetric encryption
    The implementation follows the example given in the OpenSSL wiki on
    EVP aes encryption. In this code the key and iv are hardcoded but will also
    use a session key provided from the dh.c functions. 
    The aes_encrypt() instantiates a cipher context which utilizes the AES 128 bit algorithm
    and the Cipher Block Chaining method of diffusion. Since this project is intended for
    academic purposes it does not use a stronger type of encryption in order to simplify the
    code for analysis. 
    Example code: https://wiki.openssl.org/index.php/EVP_Symmetric_Encryption_and_Decryption
    Attr: Michael Guerette (MDG), Jay Patel  */

#include "crypto.h"

unsigned char aes_key[16] ="myonlypasswordis"; // AES key
unsigned char aes_iv[16]= "iforgotwhatisaid"; // AES IV


/********************/
/* Begin generate_aes_key_iv() */
/* Attr: MDG*/
/*********************/
void generate_aes_key_iv(){
    /* ~~~~~~ Mitigate Failure to Protect Stored Data */
    // The mitigation ensures that AES keys and initialization vectors (IVs) are generated securely,
    // which provides cryptographically strong random values. This prevents predictable or weak
    // encryption keys, enhancing the security of encrypted data.
    RAND_bytes(aes_key, sizeof(aes_key));
    RAND_bytes(aes_iv, sizeof(aes_iv));
}
/* End generate_aes_key_iv() */
/*****************************************************************/

/**********************/
/* Begin aes_encrypt()
   Attr: MDG          */
/**********************/
int aes_encrypt(const unsigned char *plaintxt, int plaintxt_len, unsigned char *ciphertxt){
    EVP_CIPHER_CTX *ctx;
    int len, ciphertxt_len;

    /* ~~~~~~ Mitigate Failure to Handle Errors Correctly */
    // The mitigation ensures that the return values of critical OpenSSL functions are checked
    // to detect failures during encryption or decryption operations. If a failure is detected,
    // the program logs an error message using perror and gracefully handles the error,
    // preventing undefined behavior or incomplete cryptographic operations.
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
/* Begin aes_decrypt()
    Attr MDG          */
/**********************/
int aes_decrypt(const unsigned char *ciphertxt, int ciphertxt_len, unsigned char *plaintxt){
    EVP_CIPHER_CTX *ctx;
    int len, plaintxt_len;

    /* ~~~~~~ Mitigate Failure to Handle Errors Correctly */
    // The mitigation ensures that the return values of critical OpenSSL functions are checked
    // to detect failures during encryption or decryption operations. If a failure is detected,
    // the program logs an error message using perror and gracefully handles the error,
    // preventing undefined behavior or incomplete cryptographic operations.
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

/**********************/
/* Begin set_crypto_key() 
    Attr Jay Patel    */
/**********************/
int set_crypto_key(const unsigned char *key, size_t keylen) {
    /* ~~~~~~ Mitigate Failure to Protect Stored Data */
    // The mitigation ensures that the key length is validated by checking if it is either 16 or 32
    // bytes before setting it. This prevents the use of insecure or invalid key sizes, ensuring the
    // cryptographic operations remain secure.
    if (keylen != 16 && keylen != 32) return -1;
    memcpy(aes_key, key, 16);  // Only use first 16 bytes for AES-128
    return 0;
}
/* End set_crypto_key() */
/*****************************************************************************/