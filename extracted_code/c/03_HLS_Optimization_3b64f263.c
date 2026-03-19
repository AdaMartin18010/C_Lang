/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\13_Hardware_Description\04_HLS_with_C\03_HLS_Optimization.md
 * Line: 1296
 * Language: c
 * Block ID: 3b64f263
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * HLS设计检查清单
 */

// 功能正确性
void check_functional() {
    // □ C仿真通过所有测试用例
    // □ 边界条件测试（空输入、最大输入）
    // □ 与参考模型对比（数值精度）
    // □ C/RTL协同仿真通过
    // □ 代码覆盖率分析
}

// 性能优化
void check_performance() {
    // □ 关键循环达到目标II
    // □ 吞吐率满足系统要求
    // □ 延迟在可接受范围
    // □ 存储器带宽利用率 > 80%
    // □ 流水线效率 > 90%
}

// 资源优化
void check_resources() {
    // □ DSP使用在预算内
    // □ BRAM利用率合理（不过度分区）
    // □ LUT/FF满足约束
    // □ 无时序违例
    // □ 功耗在热设计范围内
}

// 代码质量
void check_code_quality() {
    // □ 无动态内存分配
    // □ 循环边界确定
    // □ 递归转换为迭代
    // □ 使用restrict消除别名
    // □ 适当的pragma注释
}

// 可维护性
void check_maintainability() {
    // □ 模块化设计
    // □ 参数化配置
    // □ 版本控制
    // □ 文档完整
    // □ 测试平台可重用
}
