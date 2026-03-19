/*
 * Auto-generated from: 12_Practice_Exercises\03_Data_Structure_Projects.md
 * Line: 15
 * Language: c
 * Block ID: 371a9f9c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    int *data;
    size_t size;
    size_t capacity;
} Vector;

// 构造与析构
Vector* vec_create(void);
Vector* vec_create_with_capacity(size_t capacity);
void vec_destroy(Vector *v);

// 元素访问
int vec_get(const Vector *v, size_t index);
void vec_set(Vector *v, size_t index, int value);
int* vec_at(Vector *v, size_t index);  // 返回指针，可修改
int vec_front(const Vector *v);
int vec_back(const Vector *v);

// 容量操作
size_t vec_size(const Vector *v);
size_t vec_capacity(const Vector *v);
bool vec_empty(const Vector *v);
void vec_reserve(Vector *v, size_t new_cap);
void vec_shrink_to_fit(Vector *v);

// 修改操作
void vec_push_back(Vector *v, int value);
void vec_pop_back(Vector *v);
void vec_insert(Vector *v, size_t pos, int value);
void vec_erase(Vector *v, size_t pos);
void vec_clear(Vector *v);

// 其他
void vec_resize(Vector *v, size_t new_size, int fill_value);
void vec_swap(Vector *a, Vector *b);
