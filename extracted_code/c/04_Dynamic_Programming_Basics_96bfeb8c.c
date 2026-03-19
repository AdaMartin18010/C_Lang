/*
 * Auto-generated from: 09_Data_Structures_Algorithms\02_Fundamental_Algorithms\04_Dynamic_Programming_Basics.md
 * Line: 156
 * Language: c
 * Block ID: 96bfeb8c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * DP解题框架
 *
 * 步骤1: 明确状态定义
 *   dp[i] = 考虑前i个元素时的最优值
 *
 * 步骤2: 推导状态转移
 *   dp[i] = max/min(dp[i-1] + val[i], val[i])
 *
 * 步骤3: 确定初始条件
 *   dp[0] = 0 或 第一个元素的值
 *
 * 步骤4: 确定遍历顺序
 *   通常从左到右，或从下到上
 *
 * 步骤5: 返回结果
 *   通常是dp[n-1]或dp数组的最大值
 */
