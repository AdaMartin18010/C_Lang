/*
 * 字符串化与标记粘贴宏示例
 * 编译: gcc -Wall -Wextra -o stringify_concat stringify_concat.c
 */
#include <stdio.h>

#define STR(x)  #x
#define XSTR(x) STR(x)

#define CAT(a, b) a##b
#define XCAT(a, b) CAT(a, b)

/* 自动生成寄存器定义 */
#define DEF_REG(name, addr) \
    static volatile unsigned *CAT(REG_, name) = (volatile unsigned *)(addr)

/* 自动生成结构体字段 */
#define DEF_FIELD(type, name) type name

/* 调试打印宏 */
#define dbg_print(var) printf("%s = %d (at %s:%d)\n", #var, var, __FILE__, __LINE__)

/* 编译期断言 */
#define STATIC_ASSERT(expr) \
    typedef char XCAT(static_assert_, __LINE__)[(expr) ? 1 : -1]

int main(void) {
    printf("STR(Hello) = %s\n", STR(Hello));
    printf("XSTR(__LINE__) = %s\n", XSTR(__LINE__));
    
    int XCAT(my, _var) = 42;
    printf("my_var = %d\n", my_var);
    
    int value = 123;
    dbg_print(value);
    
    STATIC_ASSERT(sizeof(int) == 4);
    
    /* 自动注册 */
    DEF_REG(CTRL, 0x40000000); (void)REG_CTRL;
    DEF_REG(STATUS, 0x40000004); (void)REG_STATUS;
    
    printf("Register addresses defined at compile time\n");
    return 0;
}
