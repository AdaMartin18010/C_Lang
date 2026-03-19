/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\01_Data_Lab.md
 * Line: 706
 * Language: c
 * Block ID: e2749dee
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 打印二进制表示
void print_binary(int x) {
    for (int i = 31; i >= 0; i--) {
        printf("%d", (x >> i) & 1);
        if (i % 8 == 0) printf(" ");
    }
    printf("\n");
}

// 检查中间结果
int test_function() {
    int step1 = ...;
    printf("step1 = 0x%08x\n", step1);
    int step2 = ...;
    printf("step2 = 0x%08x\n", step2);
    return step2;
}
