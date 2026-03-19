/*
 * Auto-generated from: 12_Practice_Exercises\02_Pointer_Exercises.md
 * Line: 227
 * Language: c
 * Block ID: e7706b14
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    char *data;
    size_t len;
    size_t capacity;
} String;

// 构造和析构
String* string_new(const char *str);
String* string_new_n(const char *str, size_t n);
void string_free(String *s);

// 修改
void string_append(String *s, const char *str);
void string_append_char(String *s, char c);
void string_insert(String *s, size_t pos, const char *str);
void string_erase(String *s, size_t pos, size_t len);
void string_clear(String *s);

// 查询
const char* string_cstr(const String *s);
size_t string_length(const String *s);
int string_compare(const String *a, const String *b);
String* string_substr(const String *s, size_t pos, size_t len);

// 查找
size_t string_find(const String *s, const char *needle);
size_t string_find_char(const String *s, char c);
