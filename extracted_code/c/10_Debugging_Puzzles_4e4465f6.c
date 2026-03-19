/*
 * Auto-generated from: 12_Practice_Exercises\10_Debugging_Puzzles.md
 * Line: 300
 * Language: c
 * Block ID: 4e4465f6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 方案1: 明确所有权
void process(const int *p) {  // 不释放，只读取
    // 只读操作
}

// 方案2: 释放后置NULL
void process(int *p) {
    free(p);
}

int main() {
    int *data = malloc(sizeof(int) * 10);
    process(data);
    data = NULL;  // 避免悬挂指针
    // ...
    free(data);  // 现在安全了（free(NULL)是安全的）
}
