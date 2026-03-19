/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\Formal_Verification\README.md
 * Line: 247
 * Language: c
 * Block ID: 9b6d811e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 数组填充函数的完整规约 */

/*@ requires \valid(array + (0..n-1));
  @ requires \forall integer i; 0 <= i < n ==> \valid(&array[i]);
  @ requires n >= 0;
  @ assigns array[0..n-1];
  @ ensures \forall integer i; 0 <= i < n ==> array[i] == value;
  @*/
void fill_array(int* array, size_t n, int value) {
    /*@ loop invariant 0 <= i <= n;
      @ loop invariant \forall integer j; 0 <= j < i ==> array[j] == value;
      @ loop assigns i, array[0..n-1];
      @ loop variant n - i;
      @*/
    for (size_t i = 0; i < n; i++) {
        array[i] = value;
    }
}
