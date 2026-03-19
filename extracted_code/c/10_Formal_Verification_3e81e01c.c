/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\10_Formal_Verification.md
 * Line: 456
 * Language: c
 * Block ID: 3e81e01c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*@ requires n >= 0;
    requires \valid_read(arr + (0..n-1));
    assigns \nothing;
    ensures \result == (\sum(0, n-1, \lambda integer i; arr[i]));
    behavior empty:
      assumes n == 0;
      ensures \result == 0;
    behavior non_empty:
      assumes n > 0;
      ensures \result > 0;
    complete behaviors;
    disjoint behaviors;
*/
int array_sum(int *arr, int n);

// 关键ACSL谓词:
// \valid(p): p是有效的内存位置
// \valid_read(p): p可读
// \initialized(p): p已初始化
// \separated(p,q): p和q指向不重叠区域
// \old(E): E在函数入口时的值
// \result: 返回值
