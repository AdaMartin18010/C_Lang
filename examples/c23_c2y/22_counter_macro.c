/*
 * C2y __COUNTER__ 宏示例
 * 编译: gcc -std=c2y -o 22_counter_macro 22_counter_macro.c
 *       clang -std=c2y -o 22_counter_macro 22_counter_macro.c
 * 注意: __COUNTER__ 是 GCC/Clang 长期支持的扩展，C2y 计划标准化 (N3636)
 */

#include <stdio.h>

/* 生成唯一的标识符 */
#define CONCAT(a, b) a##b
#define UNIQUE_NAME(prefix) CONCAT(prefix, __COUNTER__)

/* 自动生成测试函数注册 */
#define TEST_CASE(name) \
    static void UNIQUE_NAME(test_)(void); \
    static void UNIQUE_NAME(test_)(void)

/* 示例：自动枚举生成 */
#define ENUM_ITEM(name) name = __COUNTER__,

enum AutoEnum {
    ENUM_ITEM(RED)
    ENUM_ITEM(GREEN)
    ENUM_ITEM(BLUE)
    ENUM_ITEM(YELLOW)
};

/* 示例：静态初始化计数器 */
#define REGISTER_INIT(priority) \
    static int UNIQUE_NAME(init_) __attribute__((used)) = priority

TEST_CASE(example1) {
    printf("Test case 1 executed\n");
}

TEST_CASE(example2) {
    printf("Test case 2 executed\n");
}

int main(void) {
    printf("__COUNTER__ in C2y (N3636)\n");
    printf("==========================\n");

    printf("AutoEnum values:\n");
    printf("  RED = %d\n", RED);
    printf("  GREEN = %d\n", GREEN);
    printf("  BLUE = %d\n", BLUE);
    printf("  YELLOW = %d\n", YELLOW);

    /* 每次使用 __COUNTER__ 都会递增 */
    printf("\nCounter values:\n");
    printf("  Counter A: %d\n", __COUNTER__);
    printf("  Counter B: %d\n", __COUNTER__);
    printf("  Counter C: %d\n", __COUNTER__);

    return 0;
}
