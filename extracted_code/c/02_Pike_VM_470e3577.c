/*
 * Auto-generated from: 03_System_Technology_Domains\02_Regex_Engine\02_Pike_VM.md
 * Line: 261
 * Language: c
 * Block ID: 470e3577
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// SPLIT指令的两种变体
#define SPLIT_GREEDY 0    // 优先左分支
#define SPLIT_LAZY 1      // 优先右分支

// 处理优先级
void add_thread(ThreadList *list, Thread t, int priority) {
    if (priority == SPLIT_GREEDY) {
        // 贪婪：放在列表末尾（后处理）
        list->threads[list->n++] = t;
    } else {
        // 非贪婪：放在列表开头（先处理）
        memmove(&list->threads[1], &list->threads[0],
                list->n * sizeof(Thread));
        list->threads[0] = t;
        list->n++;
    }
}
