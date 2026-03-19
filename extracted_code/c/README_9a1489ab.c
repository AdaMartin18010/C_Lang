/*
 * Auto-generated from: 07_Modern_Toolchain\03_CI_CD_DevOps\README.md
 * Line: 750
 * Language: c
 * Block ID: 9a1489ab
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// test_integration.c - 集成测试示例
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "database.h"
#include "api_server.h"

// 测试数据库连接
test_database_connection() {
    db_config_t config = {
        .host = "localhost",
        .port = 5432,
        .database = "test_db",
        .user = "test_user",
        .password = "test_pass"
    };

    db_handle_t *db = db_connect(&config);
    ASSERT_NOT_NULL(db);

    // 测试CRUD操作
    db_execute(db, "BEGIN TRANSACTION");

    // 插入测试数据
    int result = db_execute(db,
        "INSERT INTO users (name, email) VALUES ('test', 'test@example.com')");
    ASSERT_EQUAL(1, result);  // 影响1行

    // 查询验证
    result_set_t *rs = db_query(db, "SELECT * FROM users WHERE name='test'");
    ASSERT_NOT_NULL(rs);
    ASSERT_TRUE(rs_next(rs));
    ASSERT_EQUAL_STRING("test@example.com", rs_get_string(rs, "email"));

    // 回滚，保持测试隔离
    db_execute(db, "ROLLBACK");

    db_disconnect(db);
    printf("Database integration test: PASSED\n");
}

// 测试API端点
test_api_endpoints() {
    // 启动测试服务器
    server_t *server = server_start("127.0.0.1", 0);  // 随机端口
    int port = server_get_port(server);

    // 测试GET请求
    http_response_t *resp = http_get(
        format("http://127.0.0.1:%d/api/health", port));
    ASSERT_EQUAL(200, resp->status);
    ASSERT_CONTAINS("healthy", resp->body);

    // 测试POST请求
    const char *json = "{\"name\":\"test\",\"value\":123}";
    resp = http_post(
        format("http://127.0.0.1:%d/api/data", port),
        json, "application/json");
    ASSERT_EQUAL(201, resp->status);

    http_response_free(resp);
    server_stop(server);
    printf("API integration test: PASSED\n");
}

int main() {
    printf("Running integration tests...\n");

    test_database_connection();
    test_api_endpoints();

    printf("All integration tests passed!\n");
    return 0;
}
