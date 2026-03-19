/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\05_Quantum_Random_Computing\02_Randomized_Algorithms.md
 * Line: 560
 * Language: c
 * Block ID: 2cfb1133
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 错误：盲目信任概率结果
bool is_prime = miller_rabin(n, 1);  // 错误概率太高！
if (is_prime) use_for_crypto(n);     // 危险！

// 正确：足够的迭代次数
bool is_prime = miller_rabin(n, 40);  // 错误概率 < 2^-80
// 或者使用确定性测试（对于小数字）
