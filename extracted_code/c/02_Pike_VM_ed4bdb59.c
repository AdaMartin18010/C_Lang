/*
 * Auto-generated from: 03_System_Technology_Domains\02_Regex_Engine\02_Pike_VM.md
 * Line: 287
 * Language: c
 * Block ID: ed4bdb59
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 每个SPLIT都创建新线程，可能导致线程数指数增长
// 解决方案：使用visited集合去重

bool visited[MAX_INST];  // 每轮重置

void add_thread(ThreadList *list, Thread t) {
    if (visited[t.pc - prog]) return;
    visited[t.pc - prog] = true;
    list->threads[list->n++] = t;
}
