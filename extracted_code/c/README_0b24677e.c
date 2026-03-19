/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\README.md
 * Line: 351
 * Language: c
 * Block ID: 0b24677e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void demonstrate_logical_ops(void)
{
    int a = 5, b = 10, c = 5;

    /* ===== 关系运算符 ===== */
    printf("a == b: %d\n", a == b);  // 0 (假)
    printf("a != b: %d\n", a != b);  // 1 (真)
    printf("a < b:  %d\n", a < b);   // 1
    printf("a > b:  %d\n", a > b);   // 0
    printf("a <= c: %d\n", a <= c);  // 1
    printf("a >= c: %d\n", a >= c);  // 1

    /* ===== 逻辑运算符 ===== */
    int p = 1, q = 0;

    printf("p && q: %d\n", p && q);  // 0 (逻辑与)
    printf("p || q: %d\n", p || q);  // 1 (逻辑或)
    printf("!p:     %d\n", !p);      // 0 (逻辑非)

    /* ===== 短路求值 ===== */
    int x = 0;

    // 短路行为：若 p 为假，不评估右侧
    if (p && (x = 10)) {
        // x 被赋值为 10
    }

    // 短路行为：若 q 为真，不评估右侧
    if (q || (x = 20)) {
        // x 被赋值为 20
    }

    /* ===== 三目运算符 ===== */
    int max = (a > b) ? a : b;  // 取较大值

    // 嵌套使用（谨慎）
    const char *sign = (a > 0) ? "positive" :
                       (a < 0) ? "negative" : "zero";
}
