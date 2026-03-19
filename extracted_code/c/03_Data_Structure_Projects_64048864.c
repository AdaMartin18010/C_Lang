/*
 * Auto-generated from: 12_Practice_Exercises\03_Data_Structure_Projects.md
 * Line: 120
 * Language: c
 * Block ID: 64048864
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 字符串哈希 (FNV-1a)
uint32_t hash_string(const void *key) {
    const char *str = key;
    uint32_t hash = 2166136261u;
    while (*str) {
        hash ^= (unsigned char)*str++;
        hash *= 16777619;
    }
    return hash;
}

// 整数哈希
uint32_t hash_int(const void *key) {
    int k = *(const int*)key;
    // Thomas Wang's hash
    k = ~k + (k << 15);
    k = k ^ (k >> 12);
    k = k + (k << 2);
    k = k ^ (k >> 4);
    k = k * 2057;
    k = k ^ (k >> 16);
    return k;
}
