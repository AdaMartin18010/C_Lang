/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\03_C23_Core_Features.md
 * Line: 521
 * Language: c
 * Block ID: 040fbbba
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 步骤 1: 启用 C23 编译
// gcc -std=c23 -Wall -Wextra

// 步骤 2: 替换 NULL 为 nullptr
// sed -i 's/\bNULL\b/nullptr/g' *.c

// 步骤 3: 使用 constexpr 替代复杂宏
// 之前:
#define BUFFER_SIZE (1024 * 1024)
// 之后:
constexpr size_t BUFFER_SIZE = 1024 * 1024;

// 步骤 4: 使用 typeof 提高代码泛型性
// 之前:
#define INIT_TO_ZERO(ptr, type) memset(ptr, 0, sizeof(type))
// 之后:
#define INIT_TO_ZERO(ptr) memset(ptr, 0, sizeof(typeof(*(ptr))))

// 步骤 5: 启用检查整数运算
// 之前:
int sum = a + b;  // 可能溢出
// 之后:
int sum;
if (ckd_add(&sum, a, b)) {
    handle_overflow();
}
