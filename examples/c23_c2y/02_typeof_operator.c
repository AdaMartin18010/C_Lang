/*
 * C23 typeof 操作符示例
 * 编译: gcc -std=c23 -o 02_typeof_operator 02_typeof_operator.c
 *       clang -std=c23 -o 02_typeof_operator 02_typeof_operator.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

// 使用 typeof 创建泛型宏
#define swap(a, b) do {          \
    typeof(a) _tmp = (a);        \
    (a) = (b);                   \
    (b) = _tmp;                  \
} while(0)

// 泛型最大值宏
#define max(a, b) ({             \
    typeof(a) _a = (a);          \
    typeof(b) _b = (b);          \
    _a > _b ? _a : _b;           \
})

// 安全内存分配
#define malloc_array(type, n) ({                    \
    typeof(type) *_ptr = malloc((n) * sizeof(type)); \
    if (_ptr) {                                      \
        for (size_t _i = 0; _i < (n); _i++) {        \
            _ptr[_i] = (typeof(type)){0};            \
        }                                            \
    }                                                \
    _ptr;                                            \
})

int main(void) {
    // 基本用法
    int x = 42;
    typeof(x) y = 100;  // y 是 int
    
    printf("x = %d, y = %d\n", x, y);
    
    // 复杂类型推导
    const int ci = 100;
    typeof(ci) ci2 = 200;  // const int
    
    int *ptr = &x;
    typeof(ptr) ptr2 = &y;  // int*
    
    // 使用泛型 swap
    int a = 10, b = 20;
    printf("\nBefore swap: a=%d, b=%d\n", a, b);
    swap(a, b);
    printf("After swap: a=%d, b=%n", a, b);
    
    double d1 = 3.14, d2 = 2.71;
    printf("\nBefore swap: d1=%f, d2=%f\n", d1, d2);
    swap(d1, d2);
    printf("After swap: d1=%f, d2=%f\n", d1, d2);
    
    // 使用泛型 max
    int m1 = max(10, 20);
    double m2 = max(3.14, 2.71);
    
    printf("\nmax(10, 20) = %d\n", m1);
    printf("max(3.14, 2.71) = %f\n", m2);
    
    // 使用 malloc_array
    int *arr = malloc_array(int, 5);
    if (arr) {
        printf("\nAllocated array (zero-initialized): ");
        for (int i = 0; i < 5; i++) {
            printf("%d ", arr[i]);
        }
        printf("\n");
        free(arr);
    }
    
    // typeof 与表达式
    typeof(1 + 2.0) result;  // double
    result = 3.14159;
    printf("\ntypeof(1 + 2.0) result = %f\n", result);
    
    return 0;
}
