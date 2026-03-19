/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\09_MISRA_C_2023_Rules_9.md
 * Line: 91
 * Language: c
 * Block ID: e576e1dc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - RAII模式 */
struct file_handle {
    FILE *fp;
};

struct file_handle *file_open(const char *path)
{
    struct file_handle *h = malloc(sizeof(*h));
    if (h == NULL) {
        return NULL;
    }
    h->fp = fopen(path, "r");
    if (h->fp == NULL) {
        free(h);
        return NULL;
    }
    return h;
}

void file_close(struct file_handle *h)
{
    if (h != NULL) {
        if (h->fp != NULL) {
            fclose(h->fp);
        }
        free(h);
    }
}

/* ✅ 合规 - 对称的资源管理 */
int acquire_resources(struct resources *res)
{
    res->fd = open_device();
    if (res->fd < 0) {
        return -1;
    }

    res->buffer = malloc(BUF_SIZE);
    if (res->buffer == NULL) {
        close_device(res->fd);  /* 释放已获取的资源 */
        return -1;
    }

    return 0;
}

void release_resources(struct resources *res)
{
    if (res->buffer != NULL) {
        free(res->buffer);
        res->buffer = NULL;
    }
    if (res->fd >= 0) {
        close_device(res->fd);
        res->fd = -1;
    }
}
