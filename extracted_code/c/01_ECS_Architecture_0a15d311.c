/*
 * Auto-generated from: 04_Industrial_Scenarios\05_Game_Engine\01_ECS_Architecture.md
 * Line: 144
 * Language: c
 * Block ID: 0a15d311
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * DOD 三原则
 *
 * 原则1: 知道你的数据
 *   - 理解数据的访问模式
 *   - 理解缓存行大小(通常64字节)
 *   - 理解内存带宽限制
 *
 * 原则2: 让数据相似者相邻
 *   - 同类型组件连续存储
 *   - 按访问顺序排列数据
 *   - 避免指针跳跃
 *
 * 原则3: 处理的是数据，不是对象
 *   - 批量处理而非逐个处理
 *   - SIMD友好
 *   - 并行友好
 */
