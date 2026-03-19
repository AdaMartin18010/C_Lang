/*
 * Auto-generated from: templates\README.md
 * Line: 328
 * Language: c
 * Block ID: 24d11140
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @def MAX_BUFFER_SIZE
 * @brief 最大缓冲区大小
 *
 * 定义了内部缓冲区的最大字节数。
 * 此值影响内存使用和最大处理能力。
 *
 * @note 修改此值需要重新编译所有依赖模块
 */
#define MAX_BUFFER_SIZE 4096

/**
 * @def SAFE_FREE(ptr)
 * @brief 安全释放内存宏
 * @param ptr 要释放的指针
 *
 * 释放指针指向的内存，并将指针置为NULL，防止悬空指针。
 *
 * @par 使用示例:
 * @code
 * char *buffer = malloc(100);
 * // 使用 buffer
 * SAFE_FREE(buffer);  // buffer 现在是 NULL
 * @endcode
 */
#define SAFE_FREE(ptr) do { \
    free(ptr); \
    (ptr) = NULL; \
} while(0)
