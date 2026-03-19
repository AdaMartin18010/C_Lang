/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering\08_Code_Review_Checklist.md
 * Line: 835
 * Language: c
 * Block ID: 500e95c1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误示例：频繁的小内存分配
void process_packets_slow(packet_t *packets, int n) {
    for (int i = 0; i < n; i++) {
        // 每个包都分配内存，开销巨大
        buffer_t *buf = malloc(sizeof(buffer_t));
        buf->data = malloc(packets[i].size);

        process(buf);

        free(buf->data);
        free(buf);
    }
}

// ✅ 优化后：内存池
#define POOL_SIZE 1024

typedef struct {
    char data[8192];
    bool used;
} pooled_buffer_t;

static pooled_buffer_t buffer_pool[POOL_SIZE];
static int pool_initialized = 0;

void *pool_alloc(size_t size) {
    if (!pool_initialized) {
        memset(buffer_pool, 0, sizeof(buffer_pool));
        pool_initialized = 1;
    }

    for (int i = 0; i < POOL_SIZE; i++) {
        if (!buffer_pool[i].used && size <= sizeof(buffer_pool[i].data)) {
            buffer_pool[i].used = true;
            return buffer_pool[i].data;
        }
    }
    return malloc(size);  // 池耗尽，回退到malloc
}

void pool_free(void *ptr) {
    for (int i = 0; i < POOL_SIZE; i++) {
        if (buffer_pool[i].data == ptr) {
            buffer_pool[i].used = false;
            return;
        }
    }
    free(ptr);  // 非池内存
}

// ✅ 优化后：批量预分配
void process_packets_fast(packet_t *packets, int n) {
    // 一次性预分配所有需要的内存
    size_t total_size = 0;
    for (int i = 0; i < n; i++) {
        total_size += packets[i].size;
    }

    char *big_buffer = malloc(total_size);
    char *ptr = big_buffer;

    for (int i = 0; i < n; i++) {
        buffer_t buf;
        buf.data = ptr;
        ptr += packets[i].size;

        process(&buf);
    }

    free(big_buffer);
}
