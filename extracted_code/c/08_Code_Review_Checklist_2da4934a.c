/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering\08_Code_Review_Checklist.md
 * Line: 1324
 * Language: c
 * Block ID: 2da4934a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file comment_example.c
 * @brief 注释规范示例文件
 * @author Your Name
 * @date 2025-03-09
 * @version 1.0.0
 *
 * @copyright Copyright (c) 2025
 *
 * @details
 * 这是一个详细的文件说明，可以包含：
 * - 文件用途和功能概述
 * - 依赖关系和前提条件
 * - 使用示例
 * - 注意事项
 *
 * @code
 * // 使用示例代码
 * int result = example_function(10, 20);
 * @endcode
 */

#ifndef COMMENT_EXAMPLE_C
#define COMMENT_EXAMPLE_C

#include <stdio.h>
#include <stdbool.h>

/* ========== 宏定义注释 ========== */

/** 缓冲区最大大小（字节） */
#define MAX_BUFFER_SIZE 4096

/**
 * @def DEFAULT_TIMEOUT_MS
 * @brief 默认超时时间（毫秒）
 * @note 最小值为100ms，最大值为30000ms
 */
#define DEFAULT_TIMEOUT_MS 5000

/**
 * @brief 安全的除法宏
 * @param x 被除数
 * @param y 除数（不能为0）
 * @return 除法结果，如果y为0返回0
 */
#define SAFE_DIVIDE(x, y) ((y) != 0 ? (x) / (y) : 0)

/* ========== 类型注释 ========== */

/**
 * @brief 学生信息结构体
 * @details 包含学生的基本信息和成绩
 */
typedef struct {
    char name[64];      /**< 学生姓名（UTF-8编码） */
    int id;             /**< 学号，唯一标识 */
    float score;        /**< 成绩，范围0.0-100.0 */
    bool is_active;     /**< 是否在读 */
} student_t;

/**
 * @brief 操作结果状态码
 */
typedef enum {
    STATUS_OK = 0,              /**< 操作成功 */
    STATUS_ERROR_PARAM = -1,    /**< 参数错误 */
    STATUS_ERROR_MEMORY = -2,   /**< 内存不足 */
    STATUS_ERROR_IO = -3        /**< IO错误 */
} status_t;

/* ========== 函数注释 ========== */

/**
 * @brief 计算两个整数的最大值
 *
 * @param a 第一个整数
 * @param b 第二个整数
 * @return int 较大的那个整数
 *
 * @code
 * int max = max_int(5, 3);  // 返回 5
 * @endcode
 */
static inline int max_int(int a, int b)
{
    return (a > b) ? a : b;
}

/**
 * @brief 初始化学生结构体
 *
 * @param[out] student 指向学生结构体的指针
 * @param[in] name 学生姓名
 * @param[in] id 学号
 * @return status_t 操作状态
 *
 * @retval STATUS_OK 初始化成功
 * @retval STATUS_ERROR_PARAM 参数无效（NULL指针）
 *
 * @note 姓名会被截断到63个字符
 * @warning 调用者负责确保name是有效的UTF-8字符串
 *
 * @code
 * student_t s;
 * status_t st = student_init(&s, "张三", 2024001);
 * if (st != STATUS_OK) {
 *     handle_error(st);
 * }
 * @endcode
 */
status_t student_init(student_t *student, const char *name, int id);

/**
 * @brief 从文件加载学生列表
 *
 * @param filename 文件路径
 * @param students 输出参数，学生数组指针的地址
 * @param count 输出参数，学生数量
 * @return status_t 操作状态
 *
 * @post 成功时，*students指向动态分配的内存，需要调用者释放
 *
 * @code
 * student_t *list = NULL;
 * int n = 0;
 * status_t st = load_students("data.txt", &list, &n);
 * if (st == STATUS_OK) {
 *     // 使用 list...
 *     free(list);
 * }
 * @endcode
 */
status_t load_students(const char *filename,
                        student_t **students,
                        int *count);

/* ========== 实现中的注释 ========== */

status_t student_init(student_t *student, const char *name, int id)
{
    /* 参数有效性检查 */
    if (student == NULL || name == NULL) {
        return STATUS_ERROR_PARAM;
    }

    /* 清空结构体，避免残留数据 */
    memset(student, 0, sizeof(*student));

    /* 拷贝姓名，确保不溢出 */
    strncpy(student->name, name, sizeof(student->name) - 1);
    student->name[sizeof(student->name) - 1] = '\0';  /* 强制null终止 */

    /* 设置其他字段 */
    student->id = id;
    student->score = 0.0f;
    student->is_active = true;

    return STATUS_OK;
}

/* ========== TODO/FIXME 注释 ========== */

/**
 * @todo 添加成绩验证函数，检查成绩范围
 * @todo 实现学生信息持久化功能
 * @todo 添加线程安全支持（当前非线程安全）
 * @body 需要使用互斥锁保护全局学生列表
 */

/**
 * FIXME: 当前实现O(n²)复杂度，需要优化
 * 建议使用哈希表存储学号到索引的映射
 * 预计优化后复杂度可降至O(n log n)
 *
 * 相关Issue: #123
 */
void find_duplicate_ids_slow(student_t *students, int count)
{
    for (int i = 0; i < count; i++) {
        for (int j = i + 1; j < count; j++) {
            if (students[i].id == students[j].id) {
                printf("Duplicate: %d\n", students[i].id);
            }
        }
    }
}

/* ========== 行内注释 ========== */

void inline_comments_example(void)
{
    int counter = 0;    /* 简单的行尾注释 */

    /*
     * 对于复杂逻辑，使用块注释
     * 解释为什么这样做，而不是做什么
     * （代码本身应该说明"做什么"）
     */
    int result = counter++ +    /* 先使用，后递增 */
                 counter-- ;    /* 先使用，后递减 */

    /* 避免无意义的注释： */
    /* ❌ int x = 5; // 将5赋值给x */
    /* ✅ int x = 5; // 默认超时时间，单位秒 */

    (void)result;
}

#endif /* COMMENT_EXAMPLE_C */
