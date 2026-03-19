/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Memory_Constrained\README.md
 * Line: 421
 * Language: c
 * Block ID: 159408f7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ring_buffer.c
#include "ring_buffer.h"

void rb_init(ring_buffer_t *rb, uint8_t *buffer, uint16_t size) {
    rb->buffer = buffer;
    rb->size = size;
    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;
}

bool rb_put(ring_buffer_t *rb, uint8_t data) {
    if (rb->count >= rb->size) return false;

    rb->buffer[rb->head] = data;
    rb->head = (rb->head + 1) % rb->size;
    rb->count++;
    return true;
}

bool rb_get(ring_buffer_t *rb, uint8_t *data) {
    if (rb->count == 0) return false;

    *data = rb->buffer[rb->tail];
    rb->tail = (rb->tail + 1) % rb->size;
    rb->count--;
    return true;
}

uint16_t rb_available(const ring_buffer_t *rb) {
    return rb->count;
}
