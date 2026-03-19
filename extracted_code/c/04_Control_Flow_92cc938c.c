/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\04_Control_Flow.md
 * Line: 230
 * Language: c
 * Block ID: 92cc938c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 循环不变量外提
// ❌ 低效
for (int i = 0; i < n; i++) {
    int len = strlen(s);  // 每次循环都计算
    use(i, len);
}

// ✅ 高效
int len = strlen(s);  // 只计算一次
for (int i = 0; i < n; i++) {
    use(i, len);
}

// 递减循环（某些架构更快，避免与0比较）
// ✅ 递减（可能更快）
for (size_t i = n; i-- > 0; ) {
    process(array[i]);
}

// 循环展开（手动或-O3自动）
// ✅ 手动展开（大数据集）
for (size_t i = 0; i < n; i += 4) {
    process(i);
    if (i+1 < n) process(i+1);
    if (i+2 < n) process(i+2);
    if (i+3 < n) process(i+3);
}
