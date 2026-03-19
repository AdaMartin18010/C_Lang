/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering\08_Code_Review_Checklist.md
 * Line: 558
 * Language: c
 * Block ID: 282c97a3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误示例：SQL注入漏洞
void query_user(const char *username) {
    char query[256];
    // 直接拼接用户输入，SQL注入风险！
    sprintf(query, "SELECT * FROM users WHERE name='%s'", username);
    // 攻击者输入: ' OR '1'='1
    execute_query(query);
}

// ✅ 正确示例：参数化查询（使用SQLite示例）
#include <sqlite3.h>

void query_user_safe(sqlite3 *db, const char *username) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT * FROM users WHERE name=?";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        return;
    }

    // 绑定参数，自动转义
    sqlite3_bind_text(stmt, 1, username, -1, SQLITE_STATIC);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        // 处理结果
    }

    sqlite3_finalize(stmt);
}

// ✅ 备用方案：严格的输入验证
int validate_username(const char *username) {
    if (username == NULL) return -1;

    size_t len = strlen(username);
    if (len == 0 || len > 32) return -1;

    // 只允许字母数字和下划线
    for (size_t i = 0; i < len; i++) {
        char c = username[i];
        if (!isalnum(c) && c != '_') {
            return -1;
        }
    }
    return 0;
}
