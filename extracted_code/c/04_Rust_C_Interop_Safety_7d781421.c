/*
 * Auto-generated from: 08_Zig_C_Connection\04_Rust_C_Interop_Safety.md
 * Line: 1675
 * Language: c
 * Block ID: 7d781421
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* complex_types.h - C端复杂数据类型 */
#ifndef COMPLEX_TYPES_H
#define COMPLEX_TYPES_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================
   嵌套结构体
   ============================================ */

typedef struct {
    double real;
    double imag;
} Complex;

typedef struct {
    Complex top_left;
    Complex bottom_right;
} Rectangle;

typedef struct {
    Rectangle bounds;
    char* name;
    int id;
} Region;

/* ============================================
   动态数组结构
   ============================================ */

typedef struct {
    double* data;
    size_t capacity;
    size_t length;
} DoubleVector;

/* 向量操作 */
DoubleVector* vector_create(size_t initial_capacity);
void vector_destroy(DoubleVector* vec);
int vector_push(DoubleVector* vec, double value);
double vector_get(const DoubleVector* vec, size_t index);
int vector_set(DoubleVector* vec, size_t index, double value);

/* ============================================
   链表结构
   ============================================ */

typedef struct ListNode {
    int value;
    struct ListNode* next;
} ListNode;

typedef struct {
    ListNode* head;
    size_t length;
} LinkedList;

/* 链表操作 */
LinkedList* list_create(void);
void list_destroy(LinkedList* list);
int list_append(LinkedList* list, int value);
int list_prepend(LinkedList* list, int value);
int list_remove(LinkedList* list, int value);
size_t list_length(const LinkedList* list);
int* list_to_array(const LinkedList* list, size_t* count);

/* ============================================
   不透明指针（Opaque Pointer）模式
   ============================================ */

/* 隐藏实现细节的句柄 */
typedef struct Database* DatabaseHandle;
typedef struct Transaction* TransactionHandle;

/* 数据库操作 */
DatabaseHandle db_open(const char* path);
void db_close(DatabaseHandle db);
int db_execute(DatabaseHandle db, const char* sql);
TransactionHandle db_begin_transaction(DatabaseHandle db);
int db_commit(TransactionHandle tx);
void db_rollback(TransactionHandle tx);

#ifdef __cplusplus
}
#endif

#endif /* COMPLEX_TYPES_H */
