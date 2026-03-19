/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\Formal_Verification\Case_Studies.md
 * Line: 247
 * Language: c
 * Block ID: c3a36d09
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// EverCrypt C API
#include <EverCrypt_AEAD.h>

// 支持自动选择最优实现 (C/AVX/AVX2/NEON)
EverCrypt_AEAD_state_s* state;
EverCrypt_AEAD_create_in(EverCrypt_Chacha20_Poly1305, &state, key);

uint8_t ciphertext[1024];
uint8_t tag[16];
EverCrypt_AEAD_encrypt(state,
                       iv, 12,
                       ad, ad_len,
                       plaintext, plaintext_len,
                       ciphertext, tag);
