/*
 * Auto-generated from: 07_Modern_Toolchain\08_Design_Patterns\03_Behavioral_Patterns.md
 * Line: 167
 * Language: c
 * Block ID: c1d139b5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    void* (*next)(void* iterator);
    bool (*has_next)(void* iterator);
    void (*destroy)(void* iterator);
} iterator_ops_t;

typedef struct {
    iterator_ops_t* ops;
    void* container;
    int current;
} iterator_t;

/* 数组迭代器 */
typedef struct {
    iterator_ops_t ops;
    void** array;
    int size;
    int current;
} array_iterator_t;

void* array_next(void* iter) {
    array_iterator_t* it = iter;
    if (it->current < it->size) {
        return it->array[it->current++];
    }
    return NULL;
}

bool array_has_next(void* iter) {
    array_iterator_t* it = iter;
    return it->current < it->size;
}

iterator_t* array_iterator_create(void** array, int size) {
    array_iterator_t* it = malloc(sizeof(array_iterator_t));
    static iterator_ops_t ops = {
        array_next, array_has_next, free
    };
    it->ops = &ops;
    it->array = array;
    it->size = size;
    it->current = 0;
    return (iterator_t*)it;
}
