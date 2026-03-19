/*
 * Auto-generated from: 03_System_Technology_Domains\01_Virtual_Machine_Interpreter\README.md
 * Line: 866
 * Language: c
 * Block ID: 8ce2d2c0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file stacklang.c
 * @brief 简单栈式语言实现
 *
 * 语法示例:
 *   1 2 +           → 3
 *   5 3 - 4 *       → 8 (计算: (5-3)*4)
 *   : square dup * ;  (定义平方函数)
 *   5 square        → 25
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

#define STACK_SIZE 256
#define WORD_MAX 32
#define DICT_SIZE 256

/* 词类型 */
typedef enum {
    WORD_NUMBER,
    WORD_PRIMITIVE,
    WORD_USER
} WordType;

/* 原语函数指针 */
typedef void (*PrimitiveFunc)(void);

/* 词典条目 */
typedef struct Word {
    char name[WORD_MAX];
    WordType type;
    union {
        int value;              /* WORD_NUMBER */
        PrimitiveFunc prim;     /* WORD_PRIMITIVE */
        struct Word *code;      /* WORD_USER: 指向代码数组 */
    } data;
    struct Word *next;
} Word;

/* 虚拟机状态 */
typedef struct {
    int stack[STACK_SIZE];
    int sp;

    Word *dictionary;           /* 词典链表头 */
    Word dict_storage[DICT_SIZE];
    int dict_count;

    int compiling;              /* 编译模式标志 */
    Word *compile_buf[256];     /* 编译缓冲区 */
    int compile_len;
} ForthVM;

/* 创建VM */
ForthVM* vm_create(void) {
    ForthVM *vm = calloc(1, sizeof(ForthVM));
    return vm;
}

/* 栈操作 */
void push(ForthVM *vm, int n) {
    if (vm->sp >= STACK_SIZE) {
        fprintf(stderr, "Stack overflow\n");
        exit(1);
    }
    vm->stack[vm->sp++] = n;
}

int pop(ForthVM *vm) {
    if (vm->sp <= 0) {
        fprintf(stderr, "Stack underflow\n");
        exit(1);
    }
    return vm->stack[--vm->sp];
}

/* 查找词 */
Word* find_word(ForthVM *vm, const char *name) {
    for (Word *w = vm->dictionary; w != NULL; w = w->next) {
        if (strcmp(w->name, name) == 0) {
            return w;
        }
    }
    return NULL;
}

/* 添加原语 */
void add_primitive(ForthVM *vm, const char *name, PrimitiveFunc func) {
    Word *w = &vm->dict_storage[vm->dict_count++];
    strcpy(w->name, name);
    w->type = WORD_PRIMITIVE;
    w->data.prim = func;
    w->next = vm->dictionary;
    vm->dictionary = w;
}

/* 原语实现 */
void prim_add(ForthVM *vm) {
    int b = pop(vm);
    int a = pop(vm);
    push(vm, a + b);
}

void prim_sub(ForthVM *vm) {
    int b = pop(vm);
    int a = pop(vm);
    push(vm, a - b);
}

void prim_mul(ForthVM *vm) {
    int b = pop(vm);
    int a = pop(vm);
    push(vm, a * b);
}

void prim_dup(ForthVM *vm) {
    int a = pop(vm);
    push(vm, a);
    push(vm, a);
}

void prim_swap(ForthVM *vm) {
    int b = pop(vm);
    int a = pop(vm);
    push(vm, b);
    push(vm, a);
}

void prim_dot(ForthVM *vm) {
    printf("%d ", pop(vm));
}

void prim_emit(ForthVM *vm) {
    putchar(pop(vm));
}

void prim_cr(ForthVM *vm) {
    putchar('\n');
}

/* 执行词 */
void execute(ForthVM *vm, Word *w);

void execute(ForthVM *vm, Word *w) {
    switch (w->type) {
        case WORD_NUMBER:
            push(vm, w->data.value);
            break;
        case WORD_PRIMITIVE:
            w->data.prim(vm);
            break;
        case WORD_USER: {
            /* 执行用户定义词 */
            Word **code = w->data.code;
            while (*code != NULL) {
                execute(vm, *code);
                code++;
            }
            break;
        }
    }
}

/* 初始化词典 */
void init_dictionary(ForthVM *vm) {
    add_primitive(vm, "+", prim_add);
    add_primitive(vm, "-", prim_sub);
    add_primitive(vm, "*", prim_mul);
    add_primitive(vm, "dup", prim_dup);
    add_primitive(vm, "swap", prim_swap);
    add_primitive(vm, ".", prim_dot);
    add_primitive(vm, "emit", prim_emit);
    add_primitive(vm, "cr", prim_cr);
}

/* 解析并执行输入 */
void interpret(ForthVM *vm, const char *input) {
    char token[WORD_MAX];
    int pos = 0;

    while (sscanf(input + pos, "%31s", token) == 1) {
        pos += strlen(token);
        while (input[pos] == ' ') pos++;

        /* 检查是否是数字 */
        int num;
        if (sscanf(token, "%d", &num) == 1 && isdigit(token[0])) {
            if (vm->compiling) {
                /* 编译模式：添加到代码 */
                Word *w = &vm->dict_storage[vm->dict_count++];
                strcpy(w->name, "LIT");
                w->type = WORD_NUMBER;
                w->data.value = num;
                vm->compile_buf[vm->compile_len++] = w;
            } else {
                push(vm, num);
            }
            continue;
        }

        /* 查找词典 */
        Word *w = find_word(vm, token);
        if (w) {
            if (vm->compiling && w->type != WORD_PRIMITIVE) {
                /* 编译用户定义词 */
                vm->compile_buf[vm->compile_len++] = w;
            } else {
                execute(vm, w);
            }
        } else {
            fprintf(stderr, "Unknown word: %s\n", token);
        }
    }
}

/* 主函数 */
int main(void) {
    ForthVM *vm = vm_create();
    init_dictionary(vm);

    printf("StackLang 0.1 - Simple Stack-Based Language\n");
    printf("Examples: 1 2 + . cr    (prints 3 and newline)\n");
    printf("          5 3 - 4 * .   (prints 8)\n\n");

    char line[256];
    while (1) {
        printf("> ");
        if (!fgets(line, sizeof(line), stdin)) break;

        if (strncmp(line, "bye", 3) == 0) break;

        interpret(vm, line);
    }

    printf("\nGoodbye!\n");
    free(vm);
    return 0;
}
