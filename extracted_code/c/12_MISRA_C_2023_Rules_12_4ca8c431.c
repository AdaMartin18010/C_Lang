/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\12_MISRA_C_2023_Rules_12.md
 * Line: 220
 * Language: c
 * Block ID: 4ca8c431
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 简单for循环 */
for (int i = 0; i < count; i++) {
    process(i);  /* 不修改i */
}

/* ✅ 合规 - 使用while处理复杂逻辑 */
int i = 0;
while (i < count) {
    if (should_skip(i)) {
        i += 2;  /* while允许复杂控制 */
    } else {
        process(i);
        i++;
    }
}

/* ✅ 合规 - 使用临时变量 */
for (int i = 0; i < n; i++) {
    int idx = i;
    if (condition) {
        idx = alternative_index(i);  /* 使用临时变量 */
    }
    use(idx);
}

/* ✅ 合规 - 逆向遍历 */
for (int i = count - 1; i >= 0; i--) {
    /* 安全删除元素 */
    remove(i);
}
