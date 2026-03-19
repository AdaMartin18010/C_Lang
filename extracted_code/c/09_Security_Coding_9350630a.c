/*
 * Auto-generated from: 12_Practice_Exercises\09_Security_Coding.md
 * Line: 164
 * Language: c
 * Block ID: 9350630a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 密码哈希
bool password_hash(const char *password, char *hash_out, size_t hash_len);

// 验证密码
bool password_verify(const char *password, const char *hash);

// 需要重新哈希（如果参数升级）
bool password_needs_rehash(const char *hash);
