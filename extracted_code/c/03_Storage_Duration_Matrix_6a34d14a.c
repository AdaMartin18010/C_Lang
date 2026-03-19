/*
 * Auto-generated from: 06_Thinking_Representation\02_Comparison_Matrices\03_Storage_Duration_Matrix.md
 * Line: 255
 * Language: c
 * Block ID: 6a34d14a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 好的实践：自动 + 分配
void process_large_data(void) {
    int local_count = 0;                    // 自动存储期
    char* buffer = malloc(BUFFER_SIZE);     // 分配存储期

    if (!buffer) return;

    // 使用 buffer ...
    local_count = process(buffer);

    free(buffer);  // 及时释放
    buffer = NULL; // 防止悬垂
}

// 好的实践：静态 + 线程
static _Thread_local char* thread_buffer = NULL;
void thread_safe_buffer(void) {
    if (!thread_buffer) {
        thread_buffer = malloc(THREAD_BUFFER_SIZE);
    }
    // 使用 thread_buffer ...
}

// 好的实践：RAII模式（C风格）
typedef struct {
    FILE* file;
    char* buffer;
} Resource;

Resource* resource_open(const char* path) {
    Resource* r = malloc(sizeof(Resource));
    r->file = fopen(path, "r");
    r->buffer = malloc(BUF_SIZE);
    return r;
}

void resource_close(Resource* r) {
    if (r) {
        free(r->buffer);
        fclose(r->file);
        free(r);
    }
}
