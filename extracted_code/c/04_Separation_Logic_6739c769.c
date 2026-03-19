/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\03_Verification_Energy\04_Separation_Logic.md
 * Line: 678
 * Language: c
 * Block ID: 6739c769
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @brief 释放断言树
 */
void SL_FreeAssertion(AssertNode_t *assertion) {
    if (NULL == assertion) {
        return;
    }

    SL_FreeAssertion(assertion->left);
    SL_FreeAssertion(assertion->right);

    if (NULL != assertion->expr_left) {
        SL_FreeExpression(assertion->expr_left);
    }
    if (NULL != assertion->expr_right) {
        SL_FreeExpression(assertion->expr_right);
    }

    free(assertion);
}

/**
 * @brief 释放表达式树
 */
void SL_FreeExpression(ExprNode_t *expr) {
    if (NULL == expr) {
        return;
    }

    SL_FreeExpression(expr->left);
    SL_FreeExpression(expr->right);

    free(expr);
}

/**
 * @brief 创建变量表达式
 */
ExprNode_t* SL_CreateVarExpr(const char *name) {
    ExprNode_t *node = (ExprNode_t *)calloc(1, sizeof(ExprNode_t));
    if (NULL != node) {
        node->type = EXPR_VAR;
        (void)strncpy(node->var_name, name, 31U);
        node->var_name[31U] = '\0';
    }
    return node;
}

/**
 * @brief 创建常量表达式
 */
ExprNode_t* SL_CreateConstExpr(int64_t value) {
    ExprNode_t *node = (ExprNode_t *)calloc(1, sizeof(ExprNode_t));
    if (NULL != node) {
        node->type = EXPR_CONST;
        node->value.type = VAL_INTEGER;
        node->value.data.integer = value;
    }
    return node;
}
