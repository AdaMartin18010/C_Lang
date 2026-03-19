/*
 * Auto-generated from: 12_Practice_Exercises\08_Concurrency_Parallelism_Labs.md
 * Line: 207
 * Language: c
 * Block ID: 7f96593f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct DoubleBuffer DoubleBuffer;

// 创建双缓冲
DoubleBuffer* dbuf_create(size_t buffer_size);
void dbuf_destroy(DoubleBuffer *db);

// 生产者：获取写入缓冲区
void* dbuf_acquire_write(DoubleBuffer *db);
void dbuf_publish(DoubleBuffer *db);  // 切换缓冲区

// 消费者：获取读取缓冲区
const void* dbuf_acquire_read(DoubleBuffer *db);
void dbuf_release_read(DoubleBuffer *db);
