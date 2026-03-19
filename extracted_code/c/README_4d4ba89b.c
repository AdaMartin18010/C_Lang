/*
 * Auto-generated from: 07_Modern_Toolchain\12_Modern_C_Standards\README.md
 * Line: 356
 * Language: c
 * Block ID: 4d4ba89b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 1. 现代属性语法 */
[[nodiscard]] int important_function(void);
[[maybe_unused]] int unused_var;
[[noreturn]] void exit_program(void);
[[deprecated("use new_function instead")]] void old_function(void);

/* 2. 空指针常量 */
int* p = nullptr;  /* 替代 NULL */

/* 3. typeof 操作符 */
typeof(int) x;           /* 等同于 int */
typeof(1 + 2.0) y;       /* 等同于 double */

typedef typeof(int) my_int;  /* 声明类型别名 */

/* typeof 与复合表达式 */
#define max(a, b) ({                    \
    typeof(a) _a = (a);                 \
    typeof(b) _b = (b);                 \
    _a > _b ? _a : _b;                  \
})

/* 4. 改进的枚举 */
enum Color : unsigned char {  /* 指定底层类型 */
    RED, GREEN, BLUE
};

/* 5. constexpr */
constexpr int SIZE = 100;  /* 编译时常量 */

/* 6. 二进制字面量 */
int flags = 0b10101010;

/* 7. 数字分隔符 */
int large = 1'000'000;   /* 单引号分隔 */
int hex = 0xFF'FF'FF'FF;

/* 8. 增强的静态断言 */
static_assert(sizeof(int) == 4);  /* 可省略消息 */

/* 9. 标签属性 */
loop_start: [[unlikely]]
for (int i = 0; i < n; i++) {
    /* 分支预测提示 */
}

/* 10. 清理属性 */
void cleanup_handler(void* p) {
    free(*(void**)p);
}

void use_resource(void) {
    char* buffer [[gnu::cleanup(cleanup_handler)]] = malloc(1024);
    /* buffer 在作用域结束时自动释放 */
}
