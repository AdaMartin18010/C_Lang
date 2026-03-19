/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\07_Microarchitecture\02_Speculative_Execution.md
 * Line: 391
 * Language: c
 * Block ID: 244851cf
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：边界检查后访问可被推测执行绕过
void unsafe_access(int *arr, int n, int idx) {
    if (idx < n) {
        // 攻击者可以训练分支预测器认为条件为真
        return arr[idx];  // 推测执行时idx可能越界
    }
}

// 正确：使用推测屏障或恒定时间访问
void safe_access(int *arr, int n, int idx) {
    unsigned int mask = -(idx < n);  // 全1或全0
    unsigned int safe_idx = idx & mask;
    return arr[safe_idx];  // 越界时访问arr[0]
}
