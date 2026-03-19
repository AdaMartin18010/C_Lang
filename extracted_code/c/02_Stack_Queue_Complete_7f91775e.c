/*
 * Auto-generated from: 09_Data_Structures_Algorithms\01_Fundamental_Data_Structures\02_Stack_Queue_Complete.md
 * Line: 1263
 * Language: c
 * Block ID: 7f91775e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 使用双端队列检查字符串是否为回文
 */
bool is_palindrome_deque(const char* str) {
    Deque deque;
    deque_init(&deque);

    // 将所有字符入队
    for (const char* p = str; *p != '\0'; p++) {
        if (isalnum(*p)) {
            deque_push_back(&deque, tolower(*p));
        }
    }

    // 两端比较
    while (deque_get_size(&deque) > 1) {
        int front, back;
        deque_pop_front(&deque, &front);
        deque_pop_back(&deque, &back);
        if (front != back) {
            deque_destroy(&deque);
            return false;
        }
    }

    deque_destroy(&deque);
    return true;
}
