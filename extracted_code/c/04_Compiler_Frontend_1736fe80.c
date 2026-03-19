/*
 * Auto-generated from: 06_Thinking_Representation\04_Case_Studies\04_Compiler_Frontend.md
 * Line: 296
 * Language: c
 * Block ID: 1736fe80
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#define MAX_SYMBOLS 1024

typedef enum {
    SYM_VARIABLE,
    SYM_FUNCTION,
    SYM_TYPE
} symbol_type_t;

typedef struct symbol {
    char*        name;
    symbol_type_t type;
    struct type*  data_type;
    int           scope_level;
    struct symbol* next;  // 链表处理冲突
} symbol_t;

typedef struct {
    symbol_t* table[MAX_SYMBOLS];
    int       current_scope;
} symbol_table_t;

// 哈希函数
uint32_t hash_string(const char* str) {
    uint32_t hash = 2166136261u;
    for (const char* p = str; *p; p++) {
        hash ^= (uint8_t)*p;
        hash *= 16777619;
    }
    return hash % MAX_SYMBOLS;
}

// 添加符号
void symbol_add(symbol_table_t* st, const char* name,
                symbol_type_t type, struct type* data_type) {
    uint32_t h = hash_string(name);

    symbol_t* sym = malloc(sizeof(symbol_t));
    sym->name = strdup(name);
    sym->type = type;
    sym->data_type = data_type;
    sym->scope_level = st->current_scope;
    sym->next = st->table[h];
    st->table[h] = sym;
}

// 查找符号
symbol_t* symbol_lookup(symbol_table_t* st, const char* name) {
    uint32_t h = hash_string(name);

    for (symbol_t* sym = st->table[h]; sym; sym = sym->next) {
        if (strcmp(sym->name, name) == 0) {
            return sym;
        }
    }
    return NULL;
}

// 进入新作用域
void scope_enter(symbol_table_t* st) {
    st->current_scope++;
}

// 退出作用域(清理当前作用域的符号)
void scope_exit(symbol_table_t* st) {
    for (int i = 0; i < MAX_SYMBOLS; i++) {
        symbol_t** current = &st->table[i];
        while (*current) {
            if ((*current)->scope_level == st->current_scope) {
                symbol_t* to_remove = *current;
                *current = (*current)->next;
                free(to_remove->name);
                free(to_remove);
            } else {
                current = &(*current)->next;
            }
        }
    }
    st->current_scope--;
}
