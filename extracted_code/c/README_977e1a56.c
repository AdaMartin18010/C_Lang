/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\README.md
 * Line: 179
 * Language: c
 * Block ID: 977e1a56
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 缓存行通常为64字节
// 硬件预取器识别模式并提前加载

// 顺序访问模式（预取友好）
for (int i = 0; i < N; i++) {
    process(arr[i]);  // 硬件预取器识别步长
}

// 链表遍历（预取困难）
while (node) {
    process(node);
    node = node->next;  // 地址不可预测
}

// 软件预取提示
#ifdef __x86_64__
    __builtin_prefetch(&arr[i + 16], 0, 3);
#endif
