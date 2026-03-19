/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir11.md
 * Line: 577
 * Language: c
 * Block ID: 0287a3a7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 预期规则方向 [待官方确认] */
/* 循环控制安全 */

/* ❌ 违反 [待官方确认] - 修改循环计数器 */
for (int i = 0; i < 10; i++) {
    if (condition) {
        i++;  /* 修改循环计数器！ */
    }
}

/* ❌ 违反 [待官方确认] - 无限循环无退出 */
while (1) {  /* 必须有break/return */
    /* 无退出条件 */
}

/* ✅ 合规 [待官方确认] */
for (int i = 0; i < 10; i++) {
    /* i只由for语句修改 */
    process(i);
}

/* ✅ 合规 [待官方确认] - 明确退出 */
bool done = false;
for (int i = 0; (i < 100) && !done; i++) {
    if (completion_condition) {
        done = true;  /* 使用标志而非break */
    }
}

/* ✅ 合规 [待官方确认] - while处理复杂逻辑 */
int pos = 0;
while (pos < max_pos) {
    if (skip_condition) {
        pos += 2;  /* while允许复杂控制 */
    } else {
        pos++;
    }
}
