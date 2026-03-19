/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\02_Stack_Queue_Complete.md
 * Line: 391
 * Language: c
 * Block ID: 16f7c244
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 括号匹配检查
 * 使用栈来检查表达式中的括号是否平衡
 *
 * 时间复杂度: O(n)
 * 空间复杂度: O(n)
 */
bool check_brackets(const char* expression) {
    LinkedStack stack;
    linked_stack_init(&stack);

    const char* p = expression;
    while (*p != '\0') {
        if (*p == '(' || *p == '[' || *p == '{') {
            // 左括号入栈
            linked_stack_push(&stack, *p);
        } else if (*p == ')' || *p == ']' || *p == '}') {
            // 右括号，尝试匹配
            if (linked_stack_is_empty(&stack)) {
                linked_stack_destroy(&stack);
                return false;  // 右括号多余
            }

            int top;
            linked_stack_pop(&stack, &top);

            // 检查是否匹配
            if ((*p == ')' && top != '(') ||
                (*p == ']' && top != '[') ||
                (*p == '}' && top != '{')) {
                linked_stack_destroy(&stack);
                return false;  // 不匹配
            }
        }
        p++;
    }

    bool balanced = linked_stack_is_empty(&stack);
    linked_stack_destroy(&stack);
    return balanced;  // 栈为空则完全匹配
}

// 使用示例
void test_brackets() {
    const char* test_cases[] = {
        "(a + b) * (c - d)",           // true
        "[(a + b) * {c - d}]",          // true
        "(a + b]",                      // false
        "((a + b)",                    // false
        "a + b)",                      // false
        "",                             // true
    };

    for (int i = 0; i < 6; i++) {
        bool result = check_brackets(test_cases[i]);
        printf("\"%s\" -> %s\n", test_cases[i], result ? "Valid" : "Invalid");
    }
}
