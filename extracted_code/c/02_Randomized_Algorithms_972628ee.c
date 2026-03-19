/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\05_Quantum_Random_Computing\02_Randomized_Algorithms.md
 * Line: 516
 * Language: c
 * Block ID: 972628ee
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：使用rand()生成密钥
void generate_weak_key(uint8_t *key, size_t len) {
    for (size_t i = 0; i < len; i++) {
        key[i] = rand() % 256;  // 不安全！rand()是可预测的
    }
}

// 正确：使用密码学安全随机
void generate_secure_key(uint8_t *key, size_t len) {
    #ifdef _WIN32
    BCryptGenRandom(NULL, key, len, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
    #else
    FILE *urandom = fopen("/dev/urandom", "rb");
    fread(key, 1, len, urandom);
    fclose(urandom);
    #endif
}
