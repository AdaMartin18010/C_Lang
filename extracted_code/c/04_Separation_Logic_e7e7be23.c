/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\03_Verification_Energy\04_Separation_Logic.md
 * Line: 375
 * Language: c
 * Block ID: e7e7be23
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file sl_assertion.c
 * @brief 分离逻辑断言实现
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "separation_logic.h"

/**
 * @brief 创建emp断言
 */
AssertNode_t* SL_CreateEmp(void) {
    AssertNode_t *node = (AssertNode_t *)calloc(1, sizeof(AssertNode_t));
    if (NULL != node) {
        node->type = ASSERT_EMP;
    }
    return node;
}

/**
 * @brief 创建点断言 l ↦ v
 */
AssertNode_t* SL_CreatePointsTo(ExprNode_t *loc, ExprNode_t *val) {
    AssertNode_t *node = (AssertNode_t *)calloc(1, sizeof(AssertNode_t));
    if (NULL != node) {
        node->type = ASSERT_POINTS_TO;
        node->expr_left = loc;
        node->expr_right = val;
    }
    return node;
}

/**
 * @brief 创建分离合取 P * Q
 */
AssertNode_t* SL_CreateSepConj(AssertNode_t *left, AssertNode_t *right) {
    AssertNode_t *node = (AssertNode_t *)calloc(1, sizeof(AssertNode_t));
    if (NULL != node) {
        node->type = ASSERT_SEP_CONJ;
        node->left = left;
        node->right = right;
    }
    return node;
}

/**
 * @brief 创建分离蕴含 P -* Q
 */
AssertNode_t* SL_CreateSepImpl(AssertNode_t *left, AssertNode_t *right) {
    AssertNode_t *node = (AssertNode_t *)calloc(1, sizeof(AssertNode_t));
    if (NULL != node) {
        node->type = ASSERT_SEP_IMPL;
        node->left = left;
        node->right = right;
    }
    return node;
}

/**
 * @brief 创建经典合取 P ∧ Q
 */
AssertNode_t* SL_CreateConj(AssertNode_t *left, AssertNode_t *right) {
    AssertNode_t *node = (AssertNode_t *)calloc(1, sizeof(AssertNode_t));
    if (NULL != node) {
        node->type = ASSERT_CONJ;
        node->left = left;
        node->right = right;
    }
    return node;
}

/**
 * @brief 创建析取 P ∨ Q
 */
AssertNode_t* SL_CreateDisj(AssertNode_t *left, AssertNode_t *right) {
    AssertNode_t *node = (AssertNode_t *)calloc(1, sizeof(AssertNode_t));
    if (NULL != node) {
        node->type = ASSERT_DISJ;
        node->left = left;
        node->right = right;
    }
    return node;
}

/**
 * @brief 创建否定 ¬P
 */
AssertNode_t* SL_CreateNeg(AssertNode_t *assertion) {
    AssertNode_t *node = (AssertNode_t *)calloc(1, sizeof(AssertNode_t));
    if (NULL != node) {
        node->type = ASSERT_NEG;
        node->left = assertion;
    }
    return node;
}

/**
 * @brief 创建存在量词 ∃x.P
 */
AssertNode_t* SL_CreateExists(const char *var, AssertNode_t *body) {
    AssertNode_t *node = (AssertNode_t *)calloc(1, sizeof(AssertNode_t));
    if (NULL != node) {
        node->type = ASSERT_EXISTS;
        (void)strncpy(node->var_name, var, 31U);
        node->var_name[31U] = '\0';
        node->left = body;
    }
    return node;
}

/**
 * @brief 创建相等断言 l = l'
 */
AssertNode_t* SL_CreateEq(ExprNode_t *left, ExprNode_t *right) {
    AssertNode_t *node = (AssertNode_t *)calloc(1, sizeof(AssertNode_t));
    if (NULL != node) {
        node->type = ASSERT_EQ;
        node->expr_left = left;
        node->expr_right = right;
    }
    return node;
}

/**
 * @brief 求值表达式
 */
static bool eval_expression(ExprNode_t *expr, Environment_t *env,
                             Value_t *result) {
    if (NULL == expr) {
        return false;
    }

    switch (expr->type) {
        case EXPR_CONST:
            *result = expr->value;
            return true;

        case EXPR_VAR: {
            uint32_t i;
            for (i = 0U; i < env->count; i++) {
                if (0 == strcmp(env->names[i], expr->var_name)) {
                    *result = env->values[i];
                    return true;
                }
            }
            return false;  /* 变量未定义 */
        }

        case EXPR_ADD: {
            Value_t left_val, right_val;
            if (!eval_expression(expr->left, env, &left_val) ||
                !eval_expression(expr->right, env, &right_val)) {
                return false;
            }
            result->type = VAL_INTEGER;
            result->data.integer = left_val.data.integer + right_val.data.integer;
            return true;
        }

        case EXPR_SUB: {
            Value_t left_val, right_val;
            if (!eval_expression(expr->left, env, &left_val) ||
                !eval_expression(expr->right, env, &right_val)) {
                return false;
            }
            result->type = VAL_INTEGER;
            result->data.integer = left_val.data.integer - right_val.data.integer;
            return true;
        }

        default:
            return false;
    }
}

/**
 * @brief 验证断言
 */
VerificationResult_t SL_VerifyAssertion(AssertNode_t *assertion,
                                         HeapModel_t *heap,
                                         Environment_t *env) {
    VerificationResult_t result = {false, "", 0U};

    if (NULL == assertion) {
        (void)strncpy(result.message, "Null assertion", 255U);
        return result;
    }

    result.steps++;

    switch (assertion->type) {
        case ASSERT_EMP:
            /* emp: 堆必须为空 */
            result.is_valid = (heap->count == 0U);
            if (!result.is_valid) {
                (void)strncpy(result.message, "Heap not empty", 255U);
            }
            break;

        case ASSERT_POINTS_TO: {
            /* l ↦ v: 堆中恰好有一个单元满足 */
            Value_t loc_val, val_val;
            uint32_t i;

            if (!eval_expression(assertion->expr_left, env, &loc_val) ||
                !eval_expression(assertion->expr_right, env, &val_val)) {
                (void)strncpy(result.message, "Expression evaluation failed", 255U);
                break;
            }

            if (heap->count != 1U) {
                result.is_valid = false;
                (void)strncpy(result.message, "Heap size != 1", 255U);
                break;
            }

            result.is_valid = (heap->cells[0].address == loc_val.data.location) &&
                             (heap->cells[0].value.data.integer == val_val.data.integer);
            if (!result.is_valid) {
                (void)strncpy(result.message, "Points-to mismatch", 255U);
            }
            break;
        }

        case ASSERT_SEP_CONJ: {
            /* P * Q: 堆可分割为两部分分别满足P和Q */
            HeapModel_t left_heap = {0};
            HeapModel_t right_heap = {0};
            uint32_t split_point;
            bool found = false;

            /* 尝试所有可能的分割 */
            for (split_point = 0U; split_point <= heap->count; split_point++) {
                uint32_t i;

                /* 分割堆 */
                for (i = 0U; i < split_point; i++) {
                    left_heap.cells[i] = heap->cells[i];
                }
                left_heap.count = split_point;

                for (i = split_point; i < heap->count; i++) {
                    right_heap.cells[i - split_point] = heap->cells[i];
                }
                right_heap.count = heap->count - split_point;

                /* 验证分割 */
                VerificationResult_t left_res =
                    SL_VerifyAssertion(assertion->left, &left_heap, env);
                if (left_res.is_valid) {
                    VerificationResult_t right_res =
                        SL_VerifyAssertion(assertion->right, &right_heap, env);
                    if (right_res.is_valid) {
                        found = true;
                        break;
                    }
                }
            }

            result.is_valid = found;
            if (!found) {
                (void)strncpy(result.message, "Cannot separate heap", 255U);
            }
            break;
        }

        case ASSERT_CONJ: {
            /* P ∧ Q: 两者都必须成立 */
            VerificationResult_t left_res =
                SL_VerifyAssertion(assertion->left, heap, env);
            if (!left_res.is_valid) {
                result = left_res;
                break;
            }

            result = SL_VerifyAssertion(assertion->right, heap, env);
            break;
        }

        case ASSERT_TRUE:
            result.is_valid = true;
            break;

        case ASSERT_FALSE:
            result.is_valid = false;
            (void)strncpy(result.message, "False assertion", 255U);
            break;

        default:
            (void)snprintf(result.message, 255U, "Unsupported assertion type: %d",
                          assertion->type);
            break;
    }

    return result;
}
