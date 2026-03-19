/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\02_Memory_Management.md
 * Line: 1653
 * Language: c
 * Block ID: 5aab0dae
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误: 循环内分配，循环外只释放最后一次
void process_items_bad(Item *items, int n) {
    char *buffer;
    for (int i = 0; i < n; i++) {
        buffer = malloc(256);  // 每次迭代分配新内存
        format_item(&items[i], buffer);
    }
    free(buffer);  // 只释放最后一个！
}

// ✅ 正确: 每次迭代释放
void process_items_good(Item *items, int n) {
    for (int i = 0; i < n; i++) {
        char *buffer = malloc(256);
        if (!buffer) continue;
        format_item(&items[i], buffer);
        free(buffer);  // 立即释放
    }
}

// ✅ 更优: 循环外分配一次
void process_items_best(Item *items, int n) {
    char *buffer = malloc(256);
    if (!buffer) return;

    for (int i = 0; i < n; i++) {
        format_item(&items[i], buffer);
    }

    free(buffer);
}
