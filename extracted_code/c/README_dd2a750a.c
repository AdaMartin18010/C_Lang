/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\README.md
 * Line: 528
 * Language: c
 * Block ID: dd2a750a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void demonstrate_loops(void)
{
    /* ===== while 循环 ===== */
    int i = 0;
    while (i < 10) {
        printf("%d ", i++);
    }
    printf("\n");

    /* ===== do-while 循环 ===== */
    int input;
    do {
        printf("Enter a number (0 to exit): ");
        scanf("%d", &input);
        process_input(input);
    } while (input != 0);

    /* ===== for 循环 ===== */
    // 标准形式
    for (int j = 0; j < 10; j++) {
        printf("%d ", j);
    }

    // 多变量循环
    for (int m = 0, n = 10; m < n; m++, n--) {
        printf("m=%d, n=%d\n", m, n);
    }

    // 省略部分（需谨慎）
    int k = 0;
    for (;;) {  // 无限循环
        if (k++ >= 10) break;
    }

    /* ===== 循环控制 ===== */
    for (int n = 0; n < 100; n++) {
        if (n % 2 == 0) continue;  // 跳过偶数
        if (n > 50) break;          // 提前退出
        printf("%d ", n);
    }

    /* ===== 嵌套循环与标签 ===== */
outer_loop:
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            if (row == 1 && col == 1) {
                break;  // 仅跳出内层循环
            }
            if (row == 2) {
                goto outer_done;  // 跳出多层
            }
        }
    }
outer_done:
    ;
}

void process_input(int value) {
    (void)value;  // 抑制未使用警告
}
