/*
 * C2y Elvis 操作符 ?: 示例
 * 编译: gcc -std=c2y -o 21_elvis_operator 21_elvis_operator.c
 *       clang -std=c2y -o 21_elvis_operator 21_elvis_operator.c
 * 注意: Elvis 操作符是 C2y 提案特性 (N3753)，GCC/Clang 扩展已支持
 */

#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int config_port = 0;      // 未配置
    int default_port = 8080;

    /* Elvis 操作符: 如果左操作数非零/非空，则返回左操作数，否则返回右操作数 */
    /* 等价于: config_port ? config_port : default_port */
    /* 但只评估一次 */

    // 基本用法：默认值
    int port = config_port ?: default_port;
    printf("Port (config=0): %d\n", port);  // 输出: 8080

    config_port = 3000;
    port = config_port ?: default_port;
    printf("Port (config=3000): %d\n", port);  // 输出: 3000

    // 指针默认值
    char *name = NULL;
    char *display_name = name ?: "Anonymous";
    printf("Name (NULL): %s\n", display_name);  // 输出: Anonymous

    name = "Alice";
    display_name = name ?: "Anonymous";
    printf("Name (Alice): %s\n", display_name);  // 输出: Alice

    // 与函数返回值结合
    int *get_optional_value(void) {
        static int val = 42;
        /* 某些条件下返回 NULL */
        return NULL;  // 模拟无值
    }

    int safe_value = get_optional_value() ?: -1;
    printf("Safe value: %d\n", safe_value);  // 输出: -1

    return 0;
}
