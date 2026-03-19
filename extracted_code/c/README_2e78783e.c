/*
 * Auto-generated from: 06_Thinking_Representation\04_Case_Studies\README.md
 * Line: 799
 * Language: c
 * Block ID: 2e78783e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 原始代码：直接依赖数据库连接
// 难以测试，无法 mock

typedef struct {
    int id;
    char name[256];
    double balance;
} account_t;

int transfer_funds(int from_id, int to_id, double amount) {
    // 直接硬编码数据库连接
    MYSQL* conn = mysql_init(NULL);
    mysql_real_connect(conn, "prod.db.server", "user", "pass", "bank", 0, NULL, 0);

    // 检查余额
    char query[256];
    snprintf(query, sizeof(query), "SELECT balance FROM accounts WHERE id=%d", from_id);
    mysql_query(conn, query);

    MYSQL_RES* result = mysql_store_result(conn);
    MYSQL_ROW row = mysql_fetch_row(result);
    double balance = atof(row[0]);
    mysql_free_result(result);

    if (balance < amount) {
        mysql_close(conn);
        return -1;  // 余额不足
    }

    // 执行转账...
    mysql_query(conn, "BEGIN");
    // 更新账户...
    mysql_query(conn, "COMMIT");

    mysql_close(conn);
    return 0;
}
