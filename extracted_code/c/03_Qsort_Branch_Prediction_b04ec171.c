/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\06_Standard_Library_Physics\03_Qsort_Branch_Prediction.md
 * Line: 738
 * Language: c
 * Block ID: b04ec171
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：非严格弱序
int wrong_compare(const void *a, const void *b) {
    int x = *(const int*)a;
    int y = *(const int*)b;

    // 缺少等于情况，导致未定义行为
    if (x < y) return -1;
    if (x > y) return 1;
    return rand() % 3 - 1;  // ❌ 随机返回！
}

// 正确：严格弱序
int correct_compare(const void *a, const void *b) {
    int x = *(const int*)a;
    int y = *(const int*)b;

    return (x > y) - (x < y);  // ✅ 一致的三态返回
}
