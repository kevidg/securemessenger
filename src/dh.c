/* dh.c is the Diffie-Hellman key exchange implementation for the secure messenger app
    Attr: Jay Patel*/
#include "dh.h"
#include <openssl/bn.h>
#include <openssl/crypto.h>
#include <openssl/err.h>
#include <stdio.h>
#include <string.h>

// Convert hex string to BIGNUM
static BIGNUM *hex2bn(const char *hex) {
    BIGNUM *bn = NULL;
    BN_hex2bn(&bn, hex);
    return bn;
}

int generate_dh_keys(dh_keys_t *keys) {
    if (!keys) return -1;
    
    BIGNUM *p = hex2bn(DH_P_HEX);
    BIGNUM *g = BN_new();
    BIGNUM *priv_key = BN_new();
    BIGNUM *pub_key = BN_new();
    BN_CTX *ctx = BN_CTX_new();
    int ret = -1;

    if (!p || !g || !priv_key || !pub_key || !ctx) {
        printf("Failed to allocate BIGNUMs\n");
        goto cleanup;
    }

    // Set generator
    if (!BN_set_word(g, DH_G)) {
        printf("Failed to set generator\n");
        goto cleanup;
    }

    // Generate private key with 256 bytes (2048 bits)
    if (!BN_rand(priv_key, 2048, -1, 0)) {
        printf("Failed to generate private key\n");
        goto cleanup;
    }

    // Ensure private key is in valid range (2 <= priv_key <= p-2)
    BIGNUM *p_minus_2 = BN_new();
    if (!p_minus_2) {
        printf("Failed to allocate p_minus_2\n");
        goto cleanup;
    }
    if (!BN_sub(p_minus_2, p, BN_value_one())) {
        BN_free(p_minus_2);
        printf("Failed to compute p-1\n");
        goto cleanup;
    }
    if (!BN_sub(p_minus_2, p_minus_2, BN_value_one())) {
        BN_free(p_minus_2);
        printf("Failed to compute p-2\n");
        goto cleanup;
    }

    // Reduce private key if needed
    if (BN_cmp(priv_key, p_minus_2) >= 0) {
        if (!BN_mod(priv_key, priv_key, p_minus_2, ctx)) {
            BN_free(p_minus_2);
            printf("Failed to reduce private key\n");
            goto cleanup;
        }
    }
    BN_free(p_minus_2);

    // Add 2 to ensure private key >= 2
    if (!BN_add(priv_key, priv_key, BN_value_one())) {
        printf("Failed to adjust private key\n");
        goto cleanup;
    }
    if (!BN_add(priv_key, priv_key, BN_value_one())) {
        printf("Failed to adjust private key\n");
        goto cleanup;
    }

    // Compute public key: g^priv_key mod p
    if (!BN_mod_exp(pub_key, g, priv_key, p, ctx)) {
        printf("Failed to compute public key\n");
        goto cleanup;
    }

    // Clear and resize buffers - now using larger buffers
    memset(keys->private_key, 0, sizeof(keys->private_key));
    memset(keys->public_key, 0, sizeof(keys->public_key));
    memset(keys->shared_secret, 0, sizeof(keys->shared_secret));
    
    int priv_len = BN_num_bytes(priv_key);
    int pub_len = BN_num_bytes(pub_key);
    
    // Debug output for key sizes
    printf("Debug: priv_len=%d, pub_len=%d, priv_buffer=%zu, pub_buffer=%zu\n", 
           priv_len, pub_len, sizeof(keys->private_key), sizeof(keys->public_key));
    
    // Check buffer sizes
    // Cast priv_len and pub_len to (size_t) which is the same type as returned by sizeof()
    // This keeps the signedness the same
    if ((size_t)priv_len > sizeof(keys->private_key) || (size_t)pub_len > sizeof(keys->public_key)) {
        printf("Key too large: priv_len=%d, pub_len=%d, priv_buffer=%zu, pub_buffer=%zu\n", 
               priv_len, pub_len, sizeof(keys->private_key), sizeof(keys->public_key));
        goto cleanup;
    }
    
    // Copy keys with right alignment
    BN_bn2bin(priv_key, keys->private_key + (sizeof(keys->private_key) - priv_len));
    BN_bn2bin(pub_key, keys->public_key + (sizeof(keys->public_key) - pub_len));
    
    ret = 0;

cleanup:
    BN_clear_free(priv_key);
    BN_free(pub_key);
    BN_free(p);
    BN_free(g);
    BN_CTX_free(ctx);
    return ret;
}

int compute_shared_secret(dh_keys_t *keys, const unsigned char *other_public, size_t pubkey_len) {
    if (!keys || !other_public || pubkey_len != sizeof(keys->public_key)) {
        printf("Invalid parameters: pubkey_len=%zu, expected=%zu\n", 
               pubkey_len, sizeof(keys->public_key));
        return -1;
    }
    
    BIGNUM *p = hex2bn(DH_P_HEX);
    BIGNUM *priv_key = BN_bin2bn(keys->private_key, 256, NULL);  // Private key is 256 bytes
    BIGNUM *pub_key = BN_bin2bn(other_public, pubkey_len, NULL); // Public key is 384 bytes
    BIGNUM *shared = BN_new();
    BN_CTX *ctx = BN_CTX_new();
    int ret = -1;

    if (!p || !priv_key || !pub_key || !shared || !ctx) {
        printf("Failed to allocate BIGNUMs for shared secret\n");
        goto cleanup;
    }

    // Add debug output
    printf("Debug: Computing shared secret...\n");
    printf("Debug: Private key length: %d\n", BN_num_bytes(priv_key));
    printf("Debug: Public key length: %d\n", BN_num_bytes(pub_key));

    // Compute shared secret
    if (!BN_mod_exp(shared, pub_key, priv_key, p, ctx)) {
        printf("Failed to compute shared secret: %s\n", 
               ERR_error_string(ERR_get_error(), NULL));
        goto cleanup;
    }

    // Convert to binary (right-aligned)
    memset(keys->shared_secret, 0, sizeof(keys->shared_secret));
    int shared_len = BN_num_bytes(shared);
    
    printf("Debug: Shared secret length: %d, buffer size: %zu\n", 
           shared_len, sizeof(keys->shared_secret));
    
    if ((size_t)shared_len > sizeof(keys->shared_secret)) {
        printf("Shared secret too large: %d > %zu\n", 
               shared_len, sizeof(keys->shared_secret));
        goto cleanup;
    }
    
    BN_bn2bin(shared, keys->shared_secret + (sizeof(keys->shared_secret) - shared_len));
    ret = 0;

cleanup:
    BN_clear_free(priv_key);  // Secure erase for private key
    BN_free(pub_key);
    BN_free(shared);
    BN_free(p);
    BN_CTX_free(ctx);
    return ret;
}

void secure_zero(void *ptr, size_t size) {
    OPENSSL_cleanse(ptr, size);
}
