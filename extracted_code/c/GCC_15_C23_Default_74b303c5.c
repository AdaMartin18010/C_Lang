/*
 * Auto-generated from: 00_VERSION_TRACKING\GCC_15_C23_Default.md
 * Line: 1220
 * Language: c
 * Block ID: 74b303c5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 问题示例: 编译器优化导致敏感数据残留 */
#include <string.h>
#include <stdlib.h>

void insecure_clear(void) {
    char password[32] = "secret_password_123";

    /* 使用普通 memset */
    memset(password, 0, sizeof(password));

    /* 问题: 如果编译器发现 password 不再被使用，
     * 可能完全优化掉 memset 调用！
     */
}

/* C23 解决方案 */
#include <string.h>

void secure_clear(void) {
    char password[32] = "secret_password_123";

    /* memset_explicit 保证执行，不会被优化 */
    memset_explicit(password, 0, sizeof(password));

    /* 密码数据被安全擦除 */
}
