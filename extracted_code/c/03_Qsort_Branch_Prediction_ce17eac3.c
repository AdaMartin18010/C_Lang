/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\06_Standard_Library_Physics\03_Qsort_Branch_Prediction.md
 * Line: 761
 * Language: c
 * Block ID: ce17eac3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：递归深度无限制
void wrong_quicksort(int *arr, int left, int right) {
    if (left >= right) return;

    int p = partition(arr, left, right);

    // 最坏情况O(n)递归深度
    wrong_quicksort(arr, left, p - 1);   // ❌ 可能栈溢出
    wrong_quicksort(arr, p + 1, right);  // ❌
}

// 正确：尾递归优化
void correct_quicksort(int *arr, int left, int right) {
    while (left < right) {
        int p = partition(arr, left, right);

        // 递归处理较小部分
        if (p - left < right - p) {
            correct_quicksort(arr, left, p - 1);
            left = p + 1;  // 循环处理较大部分
        } else {
            correct_quicksort(arr, p + 1, right);
            right = p - 1;
        }
    }
}
