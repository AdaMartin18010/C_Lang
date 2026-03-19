/*
 * Auto-generated from: 06_Thinking_Representation\01_Decision_Trees\05_Concurrency_Debug.md
 * Line: 156
 * Language: c
 * Block ID: 54c51a5a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 增加上下文切换概率
void stress_delay(void) {
    usleep(rand() % 1000);  // 随机延迟
    sched_yield();           // 强制让出CPU
}
