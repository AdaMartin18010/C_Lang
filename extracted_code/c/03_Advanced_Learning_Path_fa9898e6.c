/*
 * Auto-generated from: 06_Thinking_Representation\06_Learning_Paths\03_Advanced_Learning_Path.md
 * Line: 681
 * Language: c
 * Block ID: fa9898e6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 使用ACS注释验证安全属性

/*@
    requires \valid(arr + (0..n-1));
    requires n >= 0;
    assigns \nothing;
    ensures \result == -1 || (0 <= \result < n);
    ensures \result != -1 ==> arr[\result] == key;
    ensures \result == -1 ==>
            \forall integer i; 0 <= i < n ==> arr[i] != key;
*/
int binary_search(const int *arr, int n, int key);

/*@
    requires \valid(dest + (0..n-1));
    requires \valid(src + (0..n-1));
    requires n >= 0;
    requires \separated(dest + (0..n-1), src + (0..n-1));
    assigns dest[0..n-1];
    ensures \forall integer i; 0 <= i < n ==> dest[i] == src[i];
*/
void safe_memcpy(int *dest, const int *src, int n);

/*@
    requires \valid(ptr);
    requires *ptr > INT_MIN;
    assigns *ptr;
    ensures *ptr == \old(*ptr) - 1;
*/
void safe_decrement(int *ptr);
