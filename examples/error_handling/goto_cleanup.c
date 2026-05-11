/*
 * goto cleanup 模式示例
 * 编译: gcc -Wall -Wextra -o goto_cleanup goto_cleanup.c
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int *data;
    size_t len;
    char *name;
} Resource;

Resource *resource_create(const char *name, size_t len) {
    Resource *r = calloc(1, sizeof(Resource));
    if (!r) return NULL;
    
    r->name = strdup(name);
    r->data = calloc(len, sizeof(int));
    r->len = len;
    
    if (!r->name || !r->data) {
        free(r->name);
        free(r->data);
        free(r);
        return NULL;
    }
    return r;
}

void resource_destroy(Resource *r) {
    if (r) {
        free(r->data);
        free(r->name);
        free(r);
    }
}

/* 使用 goto cleanup 处理多资源 */
int process_resources(void) {
    Resource *r1 = NULL;
    Resource *r2 = NULL;
    FILE *fp = NULL;
    int result = -1;

    r1 = resource_create("buffer1", 100);
    if (!r1) {
        fprintf(stderr, "Failed to create r1\n");
        goto cleanup;
    }

    r2 = resource_create("buffer2", 200);
    if (!r2) {
        fprintf(stderr, "Failed to create r2\n");
        goto cleanup;
    }

    fp = fopen("output.txt", "w");
    if (!fp) {
        fprintf(stderr, "Failed to open file\n");
        goto cleanup;
    }

    /* 正常处理 */
    fprintf(fp, "r1: len=%zu, r2: len=%zu\n", r1->len, r2->len);
    result = 0;

cleanup:
    /* 所有资源统一释放 — free(NULL) 是安全的 */
    if (fp) fclose(fp);
    resource_destroy(r2);
    resource_destroy(r1);
    return result;
}

int main(void) {
    int rc = process_resources();
    printf("process_resources returned: %d\n", rc);
    return rc;
}
