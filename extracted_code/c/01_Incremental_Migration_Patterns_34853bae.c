/*
 * Auto-generated from: 08_Zig_C_Connection\05_Migration_Methodology\01_Incremental_Migration_Patterns.md
 * Line: 204
 * Language: c
 * Block ID: 34853bae
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C 代码 - 原始单体应用
// file: legacy_system.c

// 模块 A: 用户认证
int authenticate_user(const char* username, const char* password);

// 模块 B: 数据验证
int validate_input(const char* data, size_t len);

// 模块 C: 业务逻辑
int process_request(const char* request, char* response, size_t resp_len);

// 模块 D: 数据持久化
int save_to_db(const char* data);
int load_from_db(const char* key, char* output, size_t out_len);
