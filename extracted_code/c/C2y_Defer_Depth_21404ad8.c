/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 1565
 * Language: c
 * Block ID: 21404ad8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// file: goto_vs_defer.c
// compile: clang -std=c2y goto_vs_defer.c -o goto_vs_defer

#include <stdio.h>
#include <stdlib.h>

// ========== 传统goto方式 ==========
int process_with_goto(const char *filename) {
    FILE *file1 = NULL;
    FILE *file2 = NULL;
    int *buffer = NULL;
    int result = -1;

    file1 = fopen("file1.txt", "r");
    if (!file1) goto cleanup;

    file2 = fopen("file2.txt", "w");
    if (!file2) goto cleanup;

    buffer = malloc(1024);
    if (!buffer) goto cleanup;

    // 模拟多阶段处理
    if (stage1() != 0) goto cleanup;
    if (stage2() != 0) goto cleanup;
    if (stage3() != 0) goto cleanup;

    result = 0;

cleanup:
    if (buffer) free(buffer);
    if (file2) fclose(file2);
    if (file1) fclose(file1);
    return result;
}

// ========== C2y defer方式 ==========
int process_with_defer(const char *filename) {
    FILE *file1 = fopen("file1.txt", "r");
    if (!file1) return -1;
    defer fclose(file1);

    FILE *file2 = fopen("file2.txt", "w");
    if (!file2) return -1;
    defer fclose(file2);

    int *buffer = malloc(1024);
    if (!buffer) return -1;
    defer free(buffer);

    // 模拟多阶段处理
    if (stage1() != 0) return -1;  // 自动清理
    if (stage2() != 0) return -1;  // 自动清理
    if (stage3() != 0) return -1;  // 自动清理

    return 0;  // 自动清理
}

// 辅助函数
int stage1(void) {
    printf("Stage 1 executing...\n");
    return 0;
}

int stage2(void) {
    printf("Stage 2 executing...\n");
    return 0;
}

int stage3(void) {
    printf("Stage 3 executing...\n");
    return 0;
}

// ========== 对比分析 ==========
/*
goto方式的问题：
1. 资源清理代码集中在一处，与资源获取相距较远
2. 容易忘记在cleanup标签中添加新的资源释放
3. 需要维护错误码和资源状态的对应关系
4. 跳转标签打乱了代码的正常阅读顺序
5. 在大型函数中，goto目标可能很远

defer方式的优势：
1. 资源释放紧跟资源获取，代码内聚性高
2. 添加新资源时自然添加对应的defer
3. 无需维护错误码和资源状态的对应关系
4. 代码按正常顺序阅读
5. 函数结构扁平化，减少嵌套
*/

int main(void) {
    printf("=== Processing with goto ===\n");
    process_with_goto("test.txt");

    printf("\n=== Processing with defer ===\n");
    process_with_defer("test.txt");

    return 0;
}
