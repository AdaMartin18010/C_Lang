/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\06_Coroutine_Fiber.md
 * Line: 442
 * Language: c
 * Block ID: 35f8843c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* C++20协程概念:
 * - co_await: 等待可等待对象
 * - co_yield: 产生值并挂起
 * - co_return: 返回值并结束
 *
 * C实现: 使用函数指针和状态机
 */

typedef struct {
    void* promise;
    void* handle;
    int done;
} coro_handle_t;

typedef struct {
    coro_handle_t handle;
    void* value;
} awaitable_t;

/* co_await */
#define co_await(expr) ({ \
    awaitable_t a = (expr); \
    if (!a.handle.done) { \
        save_state(__LINE__); \
        suspend(); \
        case __LINE__:; \
    } \
    a.value; \
})

/* co_yield */
#define co_yield(value) do { \
    current_promise->value = (value); \
    save_state(__LINE__); \
    suspend(); \
    case __LINE__:; \
} while(0)

/* co_return */
#define co_return(value) do { \
    current_promise->return_value = (value); \
    current_coro->done = 1; \
    return; \
} while(0)
