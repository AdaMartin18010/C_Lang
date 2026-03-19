/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\10_Formal_Verification.md
 * Line: 550
 * Language: c
 * Block ID: 2596350f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*@ requires n >= 0;
    requires \valid(src + (0..n-1));
    requires \valid(dst + (0..n-1));
    requires \separated(src + (0..n-1), dst + (0..n-1));
    assigns dst[0..n-1];
    ensures \forall integer i; 0 <= i < n ==> dst[i] == \old(src[i]);
*/
void copy_array(int *src, int *dst, int n) {
    /*@ loop invariant 0 <= i <= n;
        loop invariant \forall integer j; 0 <= j < i ==> dst[j] == \old(src[j]);
        loop assigns i, dst[0..n-1];
        loop variant n - i;
    */
    for (int i = 0; i < n; i++) {
        dst[i] = src[i];  // RTE检查确保i在范围内
    }
}

// 注意: \separated确保src和dst不重叠
// 对于memcpy，需要处理重叠情况
