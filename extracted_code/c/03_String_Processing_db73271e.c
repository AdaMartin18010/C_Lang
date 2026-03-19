/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\03_String_Processing.md
 * Line: 224
 * Language: c
 * Block ID: db73271e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 简单的动态字符串实现
typedef struct {
    char *data;
    size_t len;
    size_t cap;
} String;

String *string_new(void) {
    String *s = malloc(sizeof(String));
    if (!s) return NULL;
    s->cap = 16;
    s->data = malloc(s->cap);
    if (!s->data) { free(s); return NULL; }
    s->data[0] = '\0';
    s->len = 0;
    return s;
}

void string_free(String *s) {
    if (s) {
        free(s->data);
        free(s);
    }
}

bool string_append(String *s, const char *str) {
    if (!s || !str) return false;

    size_t str_len = strlen(str);
    size_t new_len = s->len + str_len;

    if (new_len >= s->cap) {
        size_t new_cap = s->cap;
        while (new_cap <= new_len) new_cap *= 2;
        char *new_data = realloc(s->data, new_cap);
        if (!new_data) return false;
        s->data = new_data;
        s->cap = new_cap;
    }

    memcpy(s->data + s->len, str, str_len + 1);
    s->len = new_len;
    return true;
}

// 使用
String *s = string_new();
string_append(s, "Hello");
string_append(s, " ");
string_append(s, "World");
printf("%s\n", s->data);  // Hello World
string_free(s);
