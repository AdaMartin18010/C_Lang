/*
 * RAII 模拟示例 (GCC/Clang)
 * 编译: gcc -Wall -Wextra -o raii_simulation raii_simulation.c
 */
#include <stdio.h>
#include <stdlib.h>

static inline void autofree(void *p) {
    free(*(void **)p);
}

static inline void autofclose(FILE **pp) {
    if (*pp) {
        fclose(*pp);
        *pp = NULL;
    }
}

#define AUTO __attribute__((cleanup(autofree)))
#define AUTOCLOSE __attribute__((cleanup(autofclose)))

void process_file_safe(const char *path) {
    AUTO char *buf = malloc(1024);
    AUTOCLOSE FILE *fp = fopen(path, "r");
    
    if (!fp || !buf) {
        printf("Failed to open %s or allocate buffer\n", path);
        return;  /* buf 和 fp 自动释放 */
    }
    
    size_t n = fread(buf, 1, 1024, fp);
    printf("Read %zu bytes from %s\n", n, path);
    /* 函数返回时 buf 自动 free, fp 自动 fclose */
}

/* 不使用 cleanup 属性的可移植版本 */
void process_file_manual(const char *path) {
    char *buf = malloc(1024);
    FILE *fp = fopen(path, "r");
    
    if (!fp || !buf) goto cleanup;
    
    size_t n = fread(buf, 1, 1024, fp);
    printf("Read %zu bytes from %s\n", n, path);

cleanup:
    free(buf);
    if (fp) fclose(fp);
}

int main(void) {
    process_file_safe("goto_cleanup.c");
    process_file_manual("goto_cleanup.c");
    return 0;
}
