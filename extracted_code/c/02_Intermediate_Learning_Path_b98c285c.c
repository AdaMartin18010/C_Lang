/*
 * Auto-generated from: 06_Thinking_Representation\06_Learning_Paths\02_Intermediate_Learning_Path.md
 * Line: 158
 * Language: c
 * Block ID: b98c285c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 实现自己的字符串类型
typedef struct {
    char *data;
    size_t length;
    size_t capacity;
} MyString;

// 必须实现
MyString* str_create(const char *initial);
void str_destroy(MyString *str);
int str_append(MyString *str, const char *suffix);
int str_insert(MyString *str, size_t pos, const char *substring);
int str_replace(MyString *str, const char *old, const char *new);
char** str_split(MyString *str, const char *delimiter, size_t *count);
