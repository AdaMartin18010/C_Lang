/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Memory_Constrained\01_Memory_Constrained_Design.md
 * Line: 961
 * Language: c
 * Block ID: 0fc0bece
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ============================================================================
 * 环形缓冲区 (Ring Buffer) - 高效的FIFO数据结构
 * ============================================================================ */

/* ring_buffer.h */
#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* 环形缓冲区结构 */
typedef struct {
    uint8_t *buffer;          /* 缓冲区基地址 */
    uint16_t size;            /* 缓冲区大小 */
    volatile uint16_t head;   /* 写入位置 */
    volatile uint16_t tail;   /* 读取位置 */
    uint16_t count;           /* 当前元素数 */
} ring_buffer_t;

/* 初始化环形缓冲区 */
void rb_init(ring_buffer_t *rb, uint8_t *buffer, uint16_t size);

/* 清空缓冲区 */
void rb_reset(ring_buffer_t *rb);

/* 写入单个字节 */
bool rb_put(ring_buffer_t *rb, uint8_t data);

/* 读取单个字节 */
bool rb_get(ring_buffer_t *rb, uint8_t *data);

/* 批量写入 */
uint16_t rb_write(ring_buffer_t *rb, const uint8_t *data, uint16_t len);

/* 批量读取 */
uint16_t rb_read(ring_buffer_t *rb, uint8_t *data, uint16_t len);

/* 查看但不读取 */
bool rb_peek(ring_buffer_t *rb, uint8_t *data);

/* 获取可用空间 */
uint16_t rb_free(const ring_buffer_t *rb);

/* 获取已用空间 */
uint16_t rb_used(const ring_buffer_t *rb);

/* 检查是否为空 */
static inline bool rb_is_empty(const ring_buffer_t *rb) {
    return rb->count == 0;
}

/* 检查是否已满 */
static inline bool rb_is_full(const ring_buffer_t *rb) {
    return rb->count >= rb->size;
}

#endif /* RING_BUFFER_H */

/* ring_buffer.c */
#include "ring_buffer.h"

/* 初始化 */
void rb_init(ring_buffer_t *rb, uint8_t *buffer, uint16_t size)
{
    rb->buffer = buffer;
    rb->size = size;
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
}

/* 清空 */
void rb_reset(ring_buffer_t *rb)
{
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
}

/* 写入单个字节 */
bool rb_put(ring_buffer_t *rb, uint8_t data)
{
    if (rb->count >= rb->size) {
        return false;  /* 已满 */
    }

    rb->buffer[rb->head] = data;
    rb->head = (rb->head + 1) % rb->size;
    rb->count++;
    return true;
}

/* 读取单个字节 */
bool rb_get(ring_buffer_t *rb, uint8_t *data)
{
    if (rb->count == 0) {
        return false;  /* 已空 */
    }

    *data = rb->buffer[rb->tail];
    rb->tail = (rb->tail + 1) % rb->size;
    rb->count--;
    return true;
}

/* 批量写入 */
uint16_t rb_write(ring_buffer_t *rb, const uint8_t *data, uint16_t len)
{
    uint16_t written = 0;

    while (written < len && rb->count < rb->size) {
        rb->buffer[rb->head] = data[written++];
        rb->head = (rb->head + 1) % rb->size;
        rb->count++;
    }

    return written;
}

/* 批量读取 */
uint16_t rb_read(ring_buffer_t *rb, uint8_t *data, uint16_t len)
{
    uint16_t read = 0;

    while (read < len && rb->count > 0) {
        data[read++] = rb->buffer[rb->tail];
        rb->tail = (rb->tail + 1) % rb->size;
        rb->count--;
    }

    return read;
}

/* 查看但不读取 */
bool rb_peek(ring_buffer_t *rb, uint8_t *data)
{
    if (rb->count == 0) {
        return false;
    }

    *data = rb->buffer[rb->tail];
    return true;
}

/* 获取可用空间 */
uint16_t rb_free(const ring_buffer_t *rb)
{
    return rb->size - rb->count;
}

/* 获取已用空间 */
uint16_t rb_used(const ring_buffer_t *rb)
{
    return rb->count;
}

/* ============================================================================
 * 线程安全变体 (用于中断环境)
 * ============================================================================ */

/* 用于ISR的写入 - 中断安全版本 */
bool rb_put_isr(ring_buffer_t *rb, uint8_t data)
{
    /* 中断安全：只修改head和count */
    /* 假设只有一个生产者（中断）和一个消费者（主循环） */

    uint16_t next_head = (rb->head + 1) % rb->size;

    /* 检查是否已满 */
    if (next_head == rb->tail) {
        return false;
    }

    rb->buffer[rb->head] = data;
    rb->head = next_head;
    /* count更新可能有竞态，但在此模型下可接受 */

    return true;
}

/* DMA友好的环形缓冲区 */
typedef struct {
    uint8_t *buffer;
    uint16_t size;
    volatile uint16_t write_idx;  /* 由生产者更新 */
    volatile uint16_t read_idx;   /* 由消费者更新 */
} dma_ring_buffer_t;

void dma_rb_init(dma_ring_buffer_t *rb, uint8_t *buffer, uint16_t size)
{
    rb->buffer = buffer;
    rb->size = size;
    rb->write_idx = 0;
    rb->read_idx = 0;
}

/* 获取连续可写空间 (用于DMA) */
uint16_t dma_rb_write_prepare(dma_ring_buffer_t *rb, uint8_t **ptr)
{
    uint16_t free;

    if (rb->write_idx >= rb->read_idx) {
        /* 写到缓冲区末尾 */
        free = rb->size - rb->write_idx;
        /* 如果有空间，也可以绕回到开头写 */
        if (rb->read_idx > 0) {
            free = rb->size - rb->write_idx;
        }
    } else {
        free = rb->read_idx - rb->write_idx - 1;
    }

    *ptr = &rb->buffer[rb->write_idx];
    return free;
}

/* 提交写入 (DMA完成后调用) */
void dma_rb_write_commit(dma_ring_buffer_t *rb, uint16_t len)
{
    rb->write_idx = (rb->write_idx + len) % rb->size;
}
