/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\03_Algorithm_Lower_Bounds.md
 * Line: 535
 * Language: c
 * Block ID: 1d3d38e4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 分析此函数的比较次数下界
int find_min(int arr[], int n) {
    int min = arr[0];
    int comparisons = 0;

    for (int i = 1; i < n; i++) {
        comparisons++;  // 每次循环一次比较
        if (arr[i] < min) {
            min = arr[i];
        }
    }
    return min;
}
// 下界：n-1 次比较（必须检查每个元素）
// 上界：n-1 次比较
// 因此算法是最优的
