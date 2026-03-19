/*
 * Auto-generated from: 12_Practice_Exercises\09_Security_Coding.md
 * Line: 200
 * Language: c
 * Block ID: 3eb2f239
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 加密安全随机数
bool secure_random_bytes(void *buf, size_t len);
uint32_t secure_random_uint32(void);
uint64_t secure_random_uint64(void);

// 随机打乱数组（Fisher-Yates）
void secure_shuffle(void *array, size_t n, size_t size);
