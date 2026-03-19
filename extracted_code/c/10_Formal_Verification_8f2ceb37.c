/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\10_Formal_Verification.md
 * Line: 517
 * Language: c
 * Block ID: 8f2ceb37
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*@ requires n >= 0;
    requires \valid_read(arr + (0..n-1));
    assigns \nothing;
    ensures \result == (\sum(0, n-1, \lambda integer i; arr[i]));
*/
int array_sum(int *arr, int n) {
    int sum = 0;

    /*@ loop invariant 0 <= i <= n;
        loop invariant sum == (\sum(0, i-1, \lambda integer j; arr[j]));
        loop assigns i, sum;
        loop variant n - i;
    */
    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }

    return sum;
}

// loop invariant: 循环不变式
// loop assigns: 循环修改的变量
// loop variant: 变体函数 (用于终止性)
