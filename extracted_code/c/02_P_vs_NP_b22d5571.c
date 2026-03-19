/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\02_P_vs_NP.md
 * Line: 437
 * Language: c
 * Block ID: b22d5571
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 分析复杂度：O(n³)
int algorithm_analysis(int n) {
    int sum = 0;
    for (int i = 0; i < n; i++) {           // n次
        for (int j = 0; j < n; j++) {       // n次
            for (int k = 0; k < j; k++) {   // 平均n/2次
                sum += i * j * k;
            }
        }
    }
    return sum;
}
// 总复杂度：n × n × n/2 = O(n³)
