/*
 * C23 auto 类型推导示例
 * 编译: gcc -std=c23 -o 01_auto_type_inference 01_auto_type_inference.c
 *       clang -std=c23 -o 01_auto_type_inference 01_auto_type_inference.c
 */

#include <stdio.h>
#include <stdlib.h>

int main(void) {
    // C23之前：必须显式声明类型
    int x = 42;
    double y = 3.14159;
    char *str = "Hello, C23!";
    
    // C23：使用 auto 推导类型
    auto a = 42;           // int
    auto b = 3.14159;      // double
    auto c = "Hello";      // const char*
    auto d = 'A';          // char
    auto e = 100L;         // long
    auto f = 1.5f;         // float
    auto g = 100ULL;       // unsigned long long
    
    printf("Types with auto:\n");
    printf("a = %d (int)\n", a);
    printf("b = %f (double)\n", b);
    printf("c = %s (const char*)\n", c);
    printf("d = %c (char)\n", d);
    printf("e = %ld (long)\n", e);
    printf("f = %f (float)\n", f);
    printf("g = %llu (unsigned long long)\n", (unsigned long long)g);
    
    // auto 与复杂类型
    struct Point {
        int x;
        int y;
    };
    
    auto p = (struct Point){10, 20};  // struct Point
    printf("\nPoint: x=%d, y=%d\n", p.x, p.y);
    
    // auto 与函数指针
    auto printf_ptr = printf;  // int (*)(const char *restrict, ...)
    printf_ptr("\nFunction pointer works!\n");
    
    // auto 与 malloc
    auto arr = malloc(10 * sizeof(int));  // void* -> 需要转换
    if (arr) {
        for (int i = 0; i < 10; i++) {
            ((int*)arr)[i] = i * i;
        }
        printf("\nArray values: ");
        for (int i = 0; i < 10; i++) {
            printf("%d ", ((int*)arr)[i]);
        }
        printf("\n");
        free(arr);
    }
    
    return 0;
}
