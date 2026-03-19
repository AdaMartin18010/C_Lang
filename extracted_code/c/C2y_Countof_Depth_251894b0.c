/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Countof_Depth.md
 * Line: 435
 * Language: c
 * Block ID: 251894b0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    uint8_t data[1024];
    size_t used;
} Buffer;

void buffer_append(Buffer *buf, uint8_t byte) {
    if (buf->used < _Countof(buf->data)) {
        buf->data[buf->used++] = byte;
    }
}
