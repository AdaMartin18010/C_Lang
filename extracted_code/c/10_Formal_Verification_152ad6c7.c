/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\10_Formal_Verification.md
 * Line: 354
 * Language: c
 * Block ID: 152ad6c7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 规约:
 * @requires: n >= 0
 * @ensures: result == n!
 *           (即 result == 1 * 2 * ... * n)
 */
int factorial(int n) {
    // 前置条件检查
    assert(n >= 0);

    int result = 1;
    int i = 1;

    while (i <= n) {
        // 循环不变式:
        // result == (i-1)!
        // 1 <= i <= n+1

        result = result * i;
        i = i + 1;

        // 验证不变式保持:
        // 新result = 旧result * i = (i-1)! * i = i!
        // 新i = i + 1, 所以 新result = (新i-1)!
    }

    // 终止时 i = n+1
    // result = (n+1-1)! = n!
    return result;
}

// 验证总结:
// 1. 初始化: i=1, result=1=0! (假设0!=1), 成立
// 2. 保持: 证明如上
// 3. 终止: i每次+1, 最终超过n
// 4. 正确性: 终止时 result = n!
