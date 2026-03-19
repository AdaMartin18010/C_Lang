/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_If_Declaration_Depth.md
 * Line: 447
 * Language: c
 * Block ID: 07f1cba1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 传统方式：临时变量污染整个函数
int process_data_traditional(const char* input) {
    int len = strlen(input);  // len 的作用域过大
    if (len == 0) {
        return ERROR_EMPTY;
    }

    char* buffer = malloc(len + 1);  // buffer 的作用域也过大
    if (buffer == NULL) {
        return ERROR_NOMEM;
    }

    strcpy(buffer, input);
    // ... 处理 buffer ...

    free(buffer);
    return SUCCESS;
    // len 和 buffer 在函数剩余部分仍然可见
}

// C2y 方式：精确控制作用域
int process_data_c2y(const char* input) {
    if (int len = strlen(input); len == 0) {
        return ERROR_EMPTY;
    }
    // len 在这里不可见！

    if (char* buffer = malloc(strlen(input) + 1); buffer == NULL) {
        return ERROR_NOMEM;
    } else {
        strcpy(buffer, input);
        // ... 处理 buffer ...
        free(buffer);
    }
    // buffer 在这里不可见！

    return SUCCESS;
}
