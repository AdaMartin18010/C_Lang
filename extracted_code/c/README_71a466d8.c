/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\README.md
 * Line: 828
 * Language: c
 * Block ID: 71a466d8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 1. 使用有意义的命名
 * 2. 保持函数简短（<50 行）
 * 3. 避免魔数，使用具名常量
 * 4. 显式处理所有错误情况
 * 5. 优先使用 size_t 进行尺寸计算
 */

// 好的实践示例
#define MAX_BUFFER_SIZE 4096
#define HASH_TABLE_SIZE 1024

enum ErrorCode {
    ERR_OK = 0,
    ERR_INVALID_PARAM = -1,
    ERR_OUT_OF_MEMORY = -2,
    ERR_IO_ERROR = -3
};

size_t safe_strncpy(char *dest, const char *src, size_t n)
{
    if (!dest || !src || n == 0) {
        return 0;
    }

    size_t i;
    for (i = 0; i < n - 1 && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    dest[i] = '\0';

    return i;
}
