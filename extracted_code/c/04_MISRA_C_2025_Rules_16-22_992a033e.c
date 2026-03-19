/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\04_MISRA_C_2025_Rules_16-22.md
 * Line: 499
 * Language: c
 * Block ID: 992a033e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 符合规范 - 使用经过验证的库 */
#include <openssl/evp.h>

/* 使用OpenSSL而不是自定义加密 */
EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, key, iv);
/* ... */
