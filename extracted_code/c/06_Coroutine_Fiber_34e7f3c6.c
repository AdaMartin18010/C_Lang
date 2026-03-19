/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\06_Coroutine_Fiber.md
 * Line: 393
 * Language: c
 * Block ID: 34e7f3c6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 模拟async/await */

typedef struct {
    void* (*async_fn)(struct async_ctx*);
    void* result;
    int completed;
} async_ctx_t;

#define async_begin(ctx) \
    do { \
        static int __state = 0; \
        switch(__state) { \
            case 0:

#define await(expr) \
    do { \
        __state = __LINE__; \
        ctx->async_fn = (void*(*)(async_ctx_t*))resume_##__LINE__; \
        return (void*)1; \
        case __LINE__: \
            if (!(expr)) return (void*)1; \
    } while(0)

#define async_end \
        } \
        ctx->completed = 1; \
        return NULL; \
    } while(0)

/* 示例 */
void* fetch_data_async(async_ctx_t* ctx) {
    async_begin(ctx);

    await(io_pending(fd));      /* 等待IO就绪 */
    await(read_complete(fd));   /* 等待读取完成 */

    ctx->result = process_data();

    async_end;
}
