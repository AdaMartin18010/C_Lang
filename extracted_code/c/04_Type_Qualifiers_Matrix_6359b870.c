/*
 * Auto-generated from: 06_Thinking_Representation\02_Comparison_Matrices\04_Type_Qualifiers_Matrix.md
 * Line: 186
 * Language: c
 * Block ID: 6359b870
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 配置数据：初始化后不变，多线程安全读取
const _Atomic Config global_config;

// 计数器：多线程安全递增
_Atomic int request_count = 0;

// 停止标志：可被信号处理程序修改
volatile sig_atomic_t should_stop = 0;
