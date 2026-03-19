/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\DO_178C\01_Software_Levels_and_Objectives.md
 * Line: 67
 * Language: c
 * Block ID: 8ba095c5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * MC/DC要求：
 * 1. 每个条件的每个可能结果至少出现一次
 * 2. 每个判定至少出现一次所有可能结果
 * 3. 每个条件独立影响判定输出
 */

/* 示例判定 */
if ((A && B) || C) {
    action();
}

/*
 * 真值表：
 * A B C | 结果 | 测试用例
 * ------+------+----------
 * T T T |   T  |  1
 * T T F |   T  |  2
 * T F T |   T  |  3
 * T F F |   F  |  4
 * F T T |   T  |  5
 * F T F |   F  |  6
 * F F T |   T  |  7
 * F F F |   F  |  8
 *
 * MC/DC最小测试集（证明每个条件独立影响输出）：
 * - A: 用例2(T,T,F→T) 和 6(F,T,F→F) - B=T,C=F时A改变输出
 * - B: 用例2(T,T,F→T) 和 4(T,F,F→F) - A=T,C=F时B改变输出
 * - C: 用例4(T,F,F→F) 和 7(F,F,T→T) - A=F,B=F时C改变输出
 *
 * 最小测试集: {2, 4, 6, 7} 或 {2, 4, 7} + {6或8}
 */
