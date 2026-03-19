/*
 * Auto-generated from: 12_Practice_Exercises\09_Security_Coding.md
 * Line: 345
 * Language: c
 * Block ID: ab6945a6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 安全
QueryBuilder *qb = query_create("users");
query_where_int(qb, "id", "=", user_id);
query_where_string(qb, "name", "=", user_name);

// 生成: SELECT * FROM users WHERE id = ? AND name = ?
// 参数: [user_id, user_name]

// 不安全（传统方式）
// sprintf(sql, "SELECT * FROM users WHERE id = %d", user_id);  // 注入风险
