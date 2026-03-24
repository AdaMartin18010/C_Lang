/*
 * C23 nullptr 常量示例
 * 编译: gcc -std=c23 -o 03_nullptr_usage 03_nullptr_usage.c
 *       clang -std=c23 -o 03_nullptr_usage 03_nullptr_usage.c
 */

#include <stdio.h>
#include <stddef.h>

// nullptr 类型安全示例
void process_int(int *p) {
    if (p != nullptr) {
        printf("Processing int: %d\n", *p);
    } else {
        printf("Received nullptr (int)\n");
    }
}

void process_double(double *p) {
    if (p != nullptr) {
        printf("Processing double: %f\n", *p);
    } else {
        printf("Received nullptr (double)\n");
    }
}

// 函数指针示例
void (*get_callback(int option))(void) {
    if (option) {
        return (void (*)(void))0x1234;  // 模拟有效地址
    }
    return nullptr;  // 类型安全的空指针
}

// nullptr_t 类型
void print_pointer_info(nullptr_t p) {
    printf("Received nullptr_t, size: %zu\n", sizeof(p));
}

int main(void) {
    // nullptr 与 NULL 对比
    int *ip = nullptr;      // C23：类型安全
    void *vp = nullptr;     // 可以赋值给任何指针类型
    
    // 旧代码使用 NULL
    int *old_style = NULL;  // 仍然支持，但 nullptr 更推荐
    
    printf("nullptr usage:\n");
    
    // 测试函数
    int value = 42;
    process_int(&value);
    process_int(nullptr);
    
    double dvalue = 3.14;
    process_double(&dvalue);
    process_double(nullptr);
    
    // nullptr 比较
    if (ip == nullptr) {
        printf("\nip is nullptr\n");
    }
    
    // nullptr_t 类型
    nullptr_t np = nullptr;
    print_pointer_info(np);
    
    // 函数指针
    void (*cb)(void) = get_callback(0);
    if (cb == nullptr) {
        printf("\nCallback is nullptr\n");
    }
    
    // 不能与整数比较（类型安全）
    // if (ip == 0) { }  // 警告：比较不同类型的指针和整数
    
    return 0;
}
