/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\03_Verification_Energy\04_Separation_Logic.md
 * Line: 799
 * Language: c
 * Block ID: 0445d924
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file sl_list_example.c
 * @brief 链表操作的分离逻辑验证示例
 */

/**
 * @brief 链表节点结构
 */
typedef struct ListNode {
    int64_t data;
    struct ListNode *next;
} ListNode_t;

/**
 * @brief 创建链表断言 list(x, [a1, a2, ..., an])
 *
 * 断言语义:
 *   list(x, [])       ≡ x = NULL
 *   list(x, a::L)     ≡ ∃y. x ↦ (a, y) * list(y, L)
 */
AssertNode_t* create_list_assertion(const char *head_var,
                                     const int64_t *values,
                                     uint32_t count) {
    if (count == 0U) {
        /* 空列表: head = NULL */
        /* 简化为 emp */
        return SL_CreateEmp();
    }

    /* 非空列表: head ↦ (values[0], next) * list(next, values[1:]) */
    /* 简化实现：使用分离合取表示节点链 */
    ExprNode_t *head = SL_CreateVarExpr(head_var);
    ExprNode_t *val = SL_CreateConstExpr(values[0]);
    AssertNode_t *first_node = SL_CreatePointsTo(head, val);

    if (count > 1U) {
        /* 递归创建尾部 */
        AssertNode_t *tail = create_list_assertion("next", values + 1, count - 1);
        return SL_CreateSepConj(first_node, tail);
    }

    return first_node;
}

/**
 * @brief 示例: 验证列表插入操作
 *
 * 前置条件: list(x, α)
 * 操作:     在头部插入元素v
 * 后置条件: list(y, v::α)
 */
void example_list_insert(void) {
    int64_t original_list[] = {2, 3, 4};
    int64_t expected_list[] = {1, 2, 3, 4};

    AssertNode_t *pre, *post;

    /* 前置: list(x, [2, 3, 4]) */
    pre = create_list_assertion("x", original_list, 3);

    /* 后置: list(y, [1, 2, 3, 4]) */
    post = create_list_assertion("y", expected_list, 4);

    printf("列表插入验证示例已创建\n");

    SL_FreeAssertion(pre);
    SL_FreeAssertion(post);
}

/**
 * @brief 框架规则应用示例
 *
 * 展示如何使用框架规则进行局部推理:
 *   {P} C {Q}
 *   -----------------
 *   {P * R} C {Q * R}
 */
void example_frame_rule(void) {
    HeapModel_t heap = {0};
    Environment_t env = {0};

    /* 初始堆: [100 ↦ 5] * [200 ↦ 10] */
    heap.cells[0].address = 100;
    heap.cells[0].value.data.integer = 5;
    heap.cells[0].is_allocated = true;

    heap.cells[1].address = 200;
    heap.cells[1].value.data.integer = 10;
    heap.cells[1].is_allocated = true;
    heap.count = 2;

    /* 命令C只修改[100]位置: [100] := 7 */
    heap.cells[0].value.data.integer = 7;

    /* 根据框架规则，[200 ↦ 10]保持不变 */
    /* 验证框架断言 */
    {
        ExprNode_t *loc200 = SL_CreateConstExpr(200);
        ExprNode_t *val10 = SL_CreateConstExpr(10);
        AssertNode_t *frame = SL_CreatePointsTo(loc200, val10);

        /* 只考虑第二块内存 */
        HeapModel_t sub_heap = {0};
        sub_heap.cells[0] = heap.cells[1];
        sub_heap.count = 1;

        VerificationResult_t result = SL_VerifyAssertion(frame, &sub_heap, &env);
        printf("框架规则验证: %s\n", result.is_valid ? "通过" : "失败");

        SL_FreeAssertion(frame);
    }
}
