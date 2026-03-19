/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\03_Verification_Energy\04_Separation_Logic.md
 * Line: 236
 * Language: c
 * Block ID: e01d6d19
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file separation_logic.h
 * @brief 分离逻辑验证器C语言实现
 * @version 1.0.0
 */

#ifndef SEPARATION_LOGIC_H
#define SEPARATION_LOGIC_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* 最大堆大小 */
#define MAX_HEAP_SIZE           (1024U)
#define MAX_VARS                (64U)
#define MAX_ASSERTIONS          (256U)
#define MAX_EXPR_DEPTH          (32U)

/* 值类型 */
typedef enum {
    VAL_INTEGER = 0,            /* 整数 */
    VAL_POINTER = 1,            /* 指针 */
    VAL_LOCATION = 2            /* 内存位置 */
} ValueType_t;

/* 值 */
typedef struct {
    ValueType_t type;
    union {
        int64_t integer;
        void *pointer;
        uint64_t location;
    } data;
} Value_t;

/* 表达式类型 */
typedef enum {
    EXPR_CONST = 0,             /* 常量 */
    EXPR_VAR = 1,               /* 变量 */
    EXPR_ADD = 2,               /* 加法 */
    EXPR_SUB = 3,               /* 减法 */
    EXPR_EQ = 4,                /* 相等 */
    EXPR_NEQ = 5,               /* 不等 */
    EXPR_LT = 6,                /* 小于 */
    EXPR_DEREF = 7,             /* 解引用 */
    EXPR_ADDR = 8               /* 取地址 */
} ExprType_t;

/* 表达式节点 */
typedef struct ExprNode {
    ExprType_t type;
    Value_t value;              /* 用于常量 */
    char var_name[32];          /* 用于变量 */
    struct ExprNode *left;
    struct ExprNode *right;
} ExprNode_t;

/* 断言类型 */
typedef enum {
    ASSERT_EMP = 0,             /* 空堆 emp */
    ASSERT_POINTS_TO = 1,       /* l ↦ v */
    ASSERT_SEP_CONJ = 2,        /* P * Q */
    ASSERT_SEP_IMPL = 3,        /* P -* Q */
    ASSERT_CONJ = 4,            /* P ∧ Q */
    ASSERT_DISJ = 5,            /* P ∨ Q */
    ASSERT_NEG = 6,             /* ¬P */
    ASSERT_EXISTS = 7,          /* ∃x.P */
    ASSERT_FORALL = 8,          /* ∀x.P */
    ASSERT_EQ = 9,              /* l = l' */
    ASSERT_TRUE = 10,           /* true */
    ASSERT_FALSE = 11           /* false */
} AssertType_t;

/* 断言节点 */
typedef struct AssertNode {
    AssertType_t type;
    char var_name[32];          /* 用于量词 */
    struct AssertNode *left;
    struct AssertNode *right;
    ExprNode_t *expr_left;      /* 用于点断言 */
    ExprNode_t *expr_right;
} AssertNode_t;

/* 堆单元 */
typedef struct {
    uint64_t address;
    Value_t value;
    bool is_allocated;
} HeapCell_t;

/* 堆模型 */
typedef struct {
    HeapCell_t cells[MAX_HEAP_SIZE];
    uint32_t count;
} HeapModel_t;

/* 变量环境 */
typedef struct {
    char names[MAX_VARS][32];
    Value_t values[MAX_VARS];
    uint32_t count;
} Environment_t;

/* 验证结果 */
typedef struct {
    bool is_valid;
    char message[256];
    uint32_t steps;
} VerificationResult_t;

/* 函数声明 */
AssertNode_t* SL_CreateEmp(void);
AssertNode_t* SL_CreatePointsTo(ExprNode_t *loc, ExprNode_t *val);
AssertNode_t* SL_CreateSepConj(AssertNode_t *left, AssertNode_t *right);
AssertNode_t* SL_CreateSepImpl(AssertNode_t *left, AssertNode_t *right);
AssertNode_t* SL_CreateConj(AssertNode_t *left, AssertNode_t *right);
AssertNode_t* SL_CreateDisj(AssertNode_t *left, AssertNode_t *right);
AssertNode_t* SL_CreateNeg(AssertNode_t *assertion);
AssertNode_t* SL_CreateExists(const char *var, AssertNode_t *body);
AssertNode_t* SL_CreateEq(ExprNode_t *left, ExprNode_t *right);

VerificationResult_t SL_VerifyAssertion(AssertNode_t *assertion,
                                         HeapModel_t *heap,
                                         Environment_t *env);
VerificationResult_t SL_VerifyHoareTriple(AssertNode_t *pre,
                                           void *command,
                                           AssertNode_t *post,
                                           HeapModel_t *heap);

void SL_FreeAssertion(AssertNode_t *assertion);
void SL_FreeExpression(ExprNode_t *expr);

#endif /* SEPARATION_LOGIC_H */
