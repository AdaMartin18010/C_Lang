/*
 * Auto-generated from: 03_System_Technology_Domains\16_Rust_Interoperability\README.md
 * Line: 153
 * Language: c
 * Block ID: b17d49df
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* myclib.c - C 库实现 */
#include <stdlib.h>
#include <string.h>

int c_add(int a, int b) {
    return a + b;
}

size_t c_strlen(const char *s) {
    return strlen(s);
}

// 回调机制
static void (*g_callback)(void *data, size_t len) = NULL;
static void *g_user_data = NULL;

void c_register_callback(void (*cb)(void *, size_t), void *user_data) {
    g_callback = cb;
    g_user_data = user_data;
}

void c_trigger_callback(const void *data, size_t len) {
    if (g_callback) {
        g_callback(g_user_data, len);
    }
}
