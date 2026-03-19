/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\07_Computability_Theory\README.md
 * Line: 546
 * Language: c
 * Block ID: 6c7c7676
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * P类问题示例：排序
 * 可以在多项式时间内解决
 */
void quicksort(int arr[], int low, int high) {
    if (low < high) {
        int pivot = partition(arr, low, high);
        quicksort(arr, low, pivot - 1);
        quicksort(arr, pivot + 1, high);
    }
}
// 时间复杂度：O(n log n) - 属于P类

/*
 * NP类问题示例：布尔可满足性问题(SAT)
 * 可以在多项式时间内验证解，但不知道是否能多项式时间求解
 */
bool verify_sat(bool assignment[], Formula *f) {
    // 验证一个赋值是否满足布尔公式
    // 时间复杂度：O(|f|) - 多项式时间可验证
    return evaluate(f, assignment);
}

// 求解脱机需要指数时间（目前最好的算法）
bool solve_sat(Formula *f, bool assignment[]) {
    int n = f->num_variables;
    // 暴力枚举所有2^n种赋值
    for (int mask = 0; mask < (1 << n); mask++) {
        for (int i = 0; i < n; i++) {
            assignment[i] = (mask >> i) & 1;
        }
        if (verify_sat(assignment, f)) {
            return true;
        }
    }
    return false;
}
// 时间复杂度：O(2^n * |f|) - 指数时间
