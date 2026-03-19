/*
 * Auto-generated from: 07_Modern_Toolchain\07_Concurrency_Parallelism\06_Coroutine_Fiber.md
 * Line: 51
 * Language: c
 * Block ID: 04297d73
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 协程基本操作 */

/* 创建协程 */
coroutine_t* co_create(void (*fn)(void*), void* arg, size_t stack_size);

/* 恢复执行 */
void co_resume(coroutine_t* co);

/* 挂起当前协程 */
void co_yield(void);

/* 切换到调度器 */
void co_return_to_scheduler(void);

/* 状态查询 */
co_status_t co_status(coroutine_t* co);
