/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\06_MISRA_C_2023_Rules_6.md
 * Line: 535
 * Language: c
 * Block ID: 1d7e5364
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 简单的for循环 */
for (int i = 0; i < 10; i++) {
    /* i只由for语句修改 */
    process(i);
}

/* ✅ 合规 - 使用独立变量 */
for (int i = 0; i < 10; i++) {
    int step = i;
    if (condition) {
        step++;  /* 修改局部变量 */
    }
    use(step);
}

/* ✅ 合规 - 使用while处理复杂逻辑 */
int pos = 0;
while (pos < max_pos) {
    if (skip_condition) {
        pos += 2;  /* while允许复杂控制 */
    } else {
        pos++;
    }
}

/* ✅ 合规 - 循环退出条件明确 */
bool done = false;
for (int i = 0; (i < 100) && !done; i++) {
    if (completion_condition) {
        done = true;  /* 使用标志而非break */
    }
}
