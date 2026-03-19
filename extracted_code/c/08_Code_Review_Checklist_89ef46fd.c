/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering\08_Code_Review_Checklist.md
 * Line: 2094
 * Language: c
 * Block ID: 89ef46fd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file test_coverage_example.c
 * @brief 测试覆盖率示例
 *
 * 覆盖率目标：
 * - 行覆盖率 >= 80%
 * - 分支覆盖率 >= 70%
 * - 函数覆盖率 >= 90%
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/* ========== 被测函数 ========== */

/**
 * @brief 解析HTTP Content-Type头
 * @param header 原始header值
 * @param type 输出参数，存储MIME类型
 * @param type_size type缓冲区大小
 * @param charset 输出参数，存储字符集（可为NULL）
 * @param charset_size charset缓冲区大小
 * @return 0成功，-1失败
 *
 * 测试要点：
 * 1. 正常输入："application/json"
 * 2. 带charset："text/html; charset=utf-8"
 * 3. 空输入：NULL或""
 * 4. 超长输入
 * 5. 特殊字符
 */
int parse_content_type(const char *header,
                       char *type, size_t type_size,
                       char *charset, size_t charset_size)
{
    /* 输入验证 */
    if (header == NULL || type == NULL || type_size == 0) {
        return -1;
    }

    /* 跳过前导空白 */
    while (*header == ' ' || *header == '\t') {
        header++;
    }

    /* 提取MIME类型 */
    const char *sep = strchr(header, ';');
    size_t type_len;
    if (sep != NULL) {
        type_len = sep - header;
    } else {
        type_len = strlen(header);
    }

    /* 去除尾部空白 */
    while (type_len > 0 && (header[type_len-1] == ' ' ||
                            header[type_len-1] == '\t')) {
        type_len--;
    }

    /* 检查长度 */
    if (type_len >= type_size) {
        return -1;
    }

    memcpy(type, header, type_len);
    type[type_len] = '\0';

    /* 提取charset（如果需要） */
    if (charset != NULL && charset_size > 0 && sep != NULL) {
        charset[0] = '\0';

        sep++;  /* 跳过; */
        while (*sep == ' ' || *sep == '\t') sep++;

        /* 查找charset= */
        const char *cs = strstr(sep, "charset=");
        if (cs != NULL) {
            cs += 8;  /* 跳过"charset=" */

            /* 跳过引号 */
            if (*cs == '"') cs++;

            size_t cs_len = 0;
            while (cs[cs_len] && cs[cs_len] != '"' &&
                   cs[cs_len] != ' ' && cs[cs_len] != ';') {
                cs_len++;
            }

            if (cs_len < charset_size) {
                memcpy(charset, cs, cs_len);
                charset[cs_len] = '\0';
            }
        }
    }

    return 0;
}

/* ========== 单元测试 ========== */

#ifdef UNIT_TEST

#include <assert.h>

static void test_parse_basic(void)
{
    char type[64], charset[32];

    /* 测试1: 基本类型 */
    memset(type, 0, sizeof(type));
    assert(parse_content_type("application/json", type, sizeof(type),
                              NULL, 0) == 0);
    assert(strcmp(type, "application/json") == 0);
    printf("✓ Test 1 passed: basic type\n");
}

static void test_parse_with_charset(void)
{
    char type[64], charset[32];

    /* 测试2: 带charset */
    memset(type, 0, sizeof(type));
    memset(charset, 0, sizeof(charset));
    assert(parse_content_type("text/html; charset=utf-8",
                              type, sizeof(type),
                              charset, sizeof(charset)) == 0);
    assert(strcmp(type, "text/html") == 0);
    assert(strcmp(charset, "utf-8") == 0);
    printf("✓ Test 2 passed: with charset\n");
}

static void test_parse_whitespace(void)
{
    char type[64], charset[32];

    /* 测试3: 带空白字符 */
    memset(type, 0, sizeof(type));
    memset(charset, 0, sizeof(charset));
    assert(parse_content_type("  text/plain ; charset = \"UTF-8\"  ",
                              type, sizeof(type),
                              charset, sizeof(charset)) == 0);
    assert(strcmp(type, "text/plain") == 0);
    assert(strcmp(charset, "UTF-8") == 0);
    printf("✓ Test 3 passed: whitespace handling\n");
}

static void test_parse_invalid(void)
{
    char type[64];

    /* 测试4: NULL输入 */
    assert(parse_content_type(NULL, type, sizeof(type), NULL, 0) == -1);

    /* 测试5: 空字符串 */
    assert(parse_content_type("", type, sizeof(type), NULL, 0) == 0);
    assert(strcmp(type, "") == 0);

    printf("✓ Test 4&5 passed: invalid inputs\n");
}

static void test_parse_edge_cases(void)
{
    char type[64], charset[32];

    /* 测试6: 超长类型 */
    char long_type[256];
    memset(long_type, 'a', sizeof(long_type) - 1);
    long_type[sizeof(long_type) - 1] = '\0';
    assert(parse_content_type(long_type, type, 10, NULL, 0) == -1);

    /* 测试7: 无charset但请求提取 */
    memset(charset, 0, sizeof(charset));
    assert(parse_content_type("image/png", type, sizeof(type),
                              charset, sizeof(charset)) == 0);
    assert(charset[0] == '\0');

    printf("✓ Test 6&7 passed: edge cases\n");
}

int main(void)
{
    printf("Running unit tests...\n\n");

    test_parse_basic();
    test_parse_with_charset();
    test_parse_whitespace();
    test_parse_invalid();
    test_parse_edge_cases();

    printf("\n✅ All tests passed!\n");
    return 0;
}

#endif /* UNIT_TEST */
