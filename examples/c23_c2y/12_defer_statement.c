/*
 * C2y defer 语句示例
 * 编译: gcc -std=c2y -o 12_defer_statement 12_defer_statement.c
 *       clang -std=c2y -o 12_defer_statement 12_defer_statement.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// defer 资源管理示例
void process_file(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("fopen");
        return;
    }
    defer fclose(fp);  // 函数退出时自动关闭
    
    char *buffer = malloc(1024);
    if (!buffer) {
        perror("malloc");
        return;
    }
    defer free(buffer);  // 函数退出时自动释放
    
    // 业务逻辑 - 无需担心资源清理
    printf("Reading file: %s\n", filename);
    while (fgets(buffer, 1024, fp)) {
        printf("  %s", buffer);
    }
    
    // 所有资源自动清理
}

// defer 块形式
int complex_operation(void) {
    int *data = malloc(100 * sizeof(int));
    if (!data) return -1;
    
    defer {
        printf("Cleaning up data...\n");
        free(data);
    }
    
    // 初始化数据
    for (int i = 0; i < 100; i++) {
        data[i] = i;
    }
    
    // 某些条件返回
    if (data[0] != 0) {
        return -1;  // defer 块仍然执行
    }
    
    // 正常完成
    return 0;  // defer 块执行
}

// 多资源管理
void multi_resource_example(void) {
    FILE *fp1 = fopen("file1.txt", "w");
    if (!fp1) return;
    defer fclose(fp1);
    
    FILE *fp2 = fopen("file2.txt", "w");
    if (!fp2) return;
    defer fclose(fp2);
    
    int *buffer = malloc(4096);
    if (!buffer) return;
    defer free(buffer);
    
    // 使用所有资源
    fprintf(fp1, "Data for file1\n");
    fprintf(fp2, "Data for file2\n");
    memset(buffer, 0, 4096);
    
    // 所有 defer 按 LIFO 顺序执行
    // 1. free(buffer)
    // 2. fclose(fp2)
    // 3. fclose(fp1)
}

// defer 与错误处理
int process_data(const char *input, const char *output) {
    FILE *in = fopen(input, "r");
    if (!in) return -1;
    defer fclose(in);
    
    FILE *out = fopen(output, "w");
    if (!out) return -1;
    defer fclose(out);
    
    char *buffer = malloc(8192);
    if (!buffer) return -1;
    defer free(buffer);
    
    // 处理数据
    size_t n;
    while ((n = fread(buffer, 1, 8192, in)) > 0) {
        if (fwrite(buffer, 1, n, out) != n) {
            return -1;  // 写入错误 - 所有资源自动清理
        }
    }
    
    if (ferror(in)) {
        return -1;  // 读取错误 - 所有资源自动清理
    }
    
    return 0;  // 成功 - 所有资源自动清理
}

// 互斥锁示例（模拟）
typedef struct { int locked; } mutex_t;

void mutex_init(mutex_t *m) { m->locked = 0; }
void mutex_lock(mutex_t *m) { m->locked = 1; printf("Mutex locked\n"); }
void mutex_unlock(mutex_t *m) { m->locked = 0; printf("Mutex unlocked\n"); }

void critical_section_example(void) {
    mutex_t mtx;
    mutex_init(&mtx);
    mutex_lock(&mtx);
    defer mutex_unlock(&mtx);  // 确保解锁
    
    // 临界区代码
    printf("In critical section\n");
    
    if (1) {
        return;  // 提前返回，mutex 仍然解锁
    }
}

// defer 与循环（注意事项）
void loop_with_defer(void) {
    for (int i = 0; i < 3; i++) {
        FILE *fp = fopen("temp.txt", "a");
        if (!fp) continue;
        
        // 注意：defer 在作用域结束时执行
        // 这里每次循环迭代结束都会执行
        defer fclose(fp);
        
        fprintf(fp, "Iteration %d\n", i);
        
        // defer 在这里执行（循环体结束）
    }
}

int main(void) {
    printf("C2y defer Statement Demo\n\n");
    
    // 创建测试文件
    FILE *test = fopen("test_input.txt", "w");
    if (test) {
        fprintf(test, "Line 1\nLine 2\nLine 3\n");
        fclose(test);
    }
    
    printf("--- File Processing ---\n");
    process_file("test_input.txt");
    
    printf("\n--- Complex Operation ---\n");
    int result = complex_operation();
    printf("Result: %d\n", result);
    
    printf("\n--- Mutex Example ---\n");
    critical_section_example();
    
    printf("\n--- Loop with defer ---\n");
    loop_with_defer();
    
    printf("\n--- Data Processing ---\n");
    result = process_data("test_input.txt", "test_output.txt");
    printf("Process result: %d\n", result);
    
    // 清理
    remove("test_input.txt");
    remove("test_output.txt");
    remove("temp.txt");
    
    printf("\ndefer demo complete!\n");
    
    return 0;
}
