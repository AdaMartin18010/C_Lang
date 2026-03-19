/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\07_MISRA_C_2023_Rules_7.md
 * Line: 472
 * Language: c
 * Block ID: c03dc14a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 单一出口 */
void single_exit_example(void)
{
    int *p = malloc(100);
    if (p == NULL) {
        return;
    }

    do {
        if (!init()) {
            break;  /* 跳到清理 */
        }
        if (!process()) {
            break;
        }
        /* 成功 */
    } while (0);

    free(p);  /* 总是执行 */
}

/* ✅ 合规 - goto清理（MISRA允许向前goto）*/
void goto_cleanup_example(void)
{
    int *p1 = NULL;
    int *p2 = NULL;
    int result = -1;

    p1 = malloc(100);
    if (p1 == NULL) {
        goto cleanup;
    }

    p2 = malloc(200);
    if (p2 == NULL) {
        goto cleanup;
    }

    /* 处理 */
    result = 0;

cleanup:
    free(p2);
    free(p1);
    return result;
}

/* ✅ 合规 - 所有权转移 */
struct buffer {
    char *data;
    size_t size;
};

/* 工厂函数 */
struct buffer *buffer_create(size_t size)
{
    struct buffer *b = malloc(sizeof(struct buffer));
    if (b == NULL) {
        return NULL;
    }
    b->data = malloc(size);
    if (b->data == NULL) {
        free(b);
        return NULL;
    }
    b->size = size;
    return b;
}

/* 析构函数 */
void buffer_destroy(struct buffer *b)
{
    if (b != NULL) {
        free(b->data);
        free(b);
    }
}
