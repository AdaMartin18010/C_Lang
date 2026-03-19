/*
 * Auto-generated from: 06_Thinking_Representation\01_Decision_Trees\02_Segfault_Troubleshooting.md
 * Line: 497
 * Language: c
 * Block ID: 3ee91cce
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// comprehensive_segfault_handling.c
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <execinfo.h>
#include <unistd.h>

// 自定义信号处理器，打印调用栈
void segfault_handler(int sig) {
    void *array[50];
    size_t size;

    // 获取调用栈
    size = backtrace(array, 50);

    // 打印错误信息
    fprintf(stderr, "\n========================================\n");
    fprintf(stderr, "Segmentation Fault (Signal %d) caught!\n", sig);
    fprintf(stderr, "========================================\n\n");

    // 输出调用栈到stderr
    fprintf(stderr, "Call stack:\n");
    backtrace_symbols_fd(array, size, STDERR_FILENO);

    fprintf(stderr, "\nDebugging tips:\n");
    fprintf(stderr, "1. Run with GDB: gdb %s\n", program_invocation_name);
    fprintf(stderr, "2. Reproduce and check: bt full, info locals\n");
    fprintf(stderr, "3. Compile with ASan: gcc -fsanitize=address\n");

    _exit(1);
}

// 安装信号处理器
__attribute__((constructor))
void setup_segfault_handler(void) {
    signal(SIGSEGV, segfault_handler);
    printf("[*] Segfault handler installed\n");
}

// 安全指针宏
#define SAFE_DEREF(ptr, def) ((ptr) ? (*(ptr)) : (def))
#define CHECK_NULL(ptr) do { \
    if ((ptr) == NULL) { \
        fprintf(stderr, "[ERROR] NULL pointer at %s:%d\n", __FILE__, __LINE__); \
        abort(); \
    } \
} while(0)

// 安全的内存分配
void *safe_malloc(size_t size, const char *file, int line) {
    void *p = malloc(size);
    if (p == NULL && size > 0) {
        fprintf(stderr, "[ERROR] malloc(%zu) failed at %s:%d\n",
                size, file, line);
        abort();
    }
    return p;
}
#define SAFE_MALLOC(size) safe_malloc((size), __FILE__, __LINE__)

int main(void) {
    printf("[*] Program started\n");

    // 示例1: 安全使用可能为NULL的指针
    int *p = NULL;
    int value = SAFE_DEREF(p, -1);  // 安全，不会崩溃
    printf("Safe dereference result: %d\n", value);

    // 示例2: 使用CHECK_NULL提前检测
    int *valid_ptr = SAFE_MALLOC(sizeof(int) * 100);
    CHECK_NULL(valid_ptr);
    valid_ptr[0] = 42;
    printf("Allocated value: %d\n", valid_ptr[0]);

    // 示例3: 故意触发segfault进行测试
    // int *bad = NULL;
    // *bad = 1;  // 取消注释以测试处理器

    free(valid_ptr);
    printf("[*] Program completed successfully\n");
    return 0;
}
