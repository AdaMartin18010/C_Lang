/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\12_MISRA_C_2023_Rules_12.md
 * Line: 54
 * Language: c
 * Block ID: 27fe88ee
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 明确比较 */
if (x == 0) {
    /* ... */
}

/* ✅ 合规 - 分离赋值和比较 */
x = get_value() + offset;
if (x != 0) {
    /* ... */
}

/* ✅ 合规 - 明确处理结果 */
result = process_data();
if (result != 0) {
    /* ... */
}

/* ✅ 合规 - 先赋值再switch */
status = get_status();
switch (status) {
    /* ... */
}

/* ✅ 合规 - 如果必须检查非零 */
if ((x = get_value()) != 0) {  /* 允许，但需显式比较 */
    use(x);
}
