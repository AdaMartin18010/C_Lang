/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Memory_Constrained\README.md
 * Line: 397
 * Language: c
 * Block ID: 3e2d4ed7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ring_buffer.h
#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t *buffer;
    uint16_t size;
    volatile uint16_t head;
    volatile uint16_t tail;
    uint16_t count;
} ring_buffer_t;

void rb_init(ring_buffer_t *rb, uint8_t *buffer, uint16_t size);
bool rb_put(ring_buffer_t *rb, uint8_t data);
bool rb_get(ring_buffer_t *rb, uint8_t *data);
uint16_t rb_available(const ring_buffer_t *rb);

#endif
