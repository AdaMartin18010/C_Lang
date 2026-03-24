/*
 * C23 标准属性语法示例
 * 编译: gcc -std=c23 -o 05_attributes_syntax 05_attributes_syntax.c
 *       clang -std=c23 -o 05_attributes_syntax 05_attributes_syntax.c
 */

#include <stdio.h>
#include <stdlib.h>

// [[deprecated]] - 标记废弃函数
[[deprecated("Use new_function() instead")]]
void old_function(void) {
    printf("This is the old function\n");
}

void new_function(void) {
    printf("This is the new function\n");
}

// [[nodiscard]] - 忽略返回值警告
[[nodiscard]]
int allocate_resource(void) {
    static int counter = 0;
    return ++counter;
}

// [[maybe_unused]] - 抑制未使用警告
[[maybe_unused]]
static void internal_helper(void) {
    // 可能暂时不使用
}

// [[fallthrough]] - 显式case穿透
void process_option(int option) {
    switch (option) {
        case 1:
            printf("Option 1\n");
            [[fallthrough]];  // 显式穿透
        case 2:
            printf("Option 2 (or fell through from 1)\n");
            break;
        case 3:
            printf("Option 3\n");
            break;
        default:
            printf("Unknown option\n");
    }
}

// 属性在声明中的位置
struct [[maybe_unused]] InternalStruct {
    int data;
};

// 多个属性
[[nodiscard, deprecated("Use allocate_v2 instead")]]
void *allocate_memory(size_t size) {
    return malloc(size);
}

// 属性与函数参数
void process_buffer(
    char * [[maybe_unused]] buffer,
    size_t size
) {
    // buffer 可能暂时不使用
    printf("Processing buffer of size %zu\n", size);
}

// 条件属性（编译器特定扩展）
#if defined(__GNUC__)
[[gnu::hot]]  // GCC特定属性
void hot_function(void) {
    printf("Hot function (frequently called)\n");
}
#endif

int main(void) {
    printf("C23 Attributes Demo\n\n");
    
    // 使用新函数（推荐）
    new_function();
    
    // 使用废弃函数（会产生警告）
    // old_function();  // 警告：old_function is deprecated
    
    // 使用 nodiscard 函数
    int resource = allocate_resource();  // OK
    printf("Allocated resource: %d\n", resource);
    
    // allocate_resource();  // 警告：ignoring return value
    
    // switch fallthrough
    printf("\nSwitch fallthrough demo:\n");
    process_option(1);
    printf("\n");
    process_option(2);
    
    // maybe_unused 示例
    process_buffer(NULL, 100);
    
    printf("\nAttributes demo complete!\n");
    
    return 0;
}
