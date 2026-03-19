/*
 * Auto-generated from: 06_Thinking_Representation\06_Learning_Paths\05_Interview_Preparation_Path.md
 * Line: 729
 * Language: c
 * Block ID: aeb2065f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 为什么需要形式化验证：
// - 测试无法证明无bug
// - 关键系统（航空、医疗、汽车）要求绝对正确

// 方法：
// 1. 模型检测（Model Checking）
//    - 穷举状态空间
//    - 验证安全属性

// 2. 定理证明（Theorem Proving）
//    - 使用Coq/Isabelle
//    - 证明代码符合规范

// 3. 静态分析（Static Analysis）
//    - ASTREE、Polyspace
//    - 证明无运行时错误

// 4. 符号执行（Symbolic Execution）
//    - KLEE
//    - 探索所有执行路径

// 实际应用：
// - seL4微内核：形式化验证正确性
// - CompCert编译器：验证编译正确性
// - 自动驾驶系统：ISO 26262 ASIL-D要求

// 在C语言中使用：
// - Frama-C/WP验证函数契约
// - CBMC验证边界条件
// - 编写ACS注释
