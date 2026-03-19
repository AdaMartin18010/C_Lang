/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\05_Frama_C_LLM_Integration.md
 * Line: 2262
 * Language: c
 * Block ID: 82ae4398
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

size_t strlen(const char *s) {
    size_t len = 0;
    // 需要不变量
    while (s[len] != '\\0') {
        len++;
    }
    return len;
}
