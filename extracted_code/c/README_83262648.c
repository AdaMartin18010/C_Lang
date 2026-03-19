/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\Formal_Verification\README.md
 * Line: 112
 * Language: c
 * Block ID: 83262648
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*@ requires \valid(result);
  @ requires a > 0 ==> b <= INT_MAX - a;
  @ requires a < 0 ==> b >= INT_MIN - a;
  @ assigns *result;
  @ behavior success:
  @   assumes a > 0 ==> b <= INT_MAX - a;
  @   assumes a < 0 ==> b >= INT_MIN - a;
  @   ensures *result == a + b;
  @   ensures \result == 1;
  @ behavior overflow:
  @   assumes a > 0 && b > INT_MAX - a ||
  @           a < 0 && b < INT_MIN - a;
  @   ensures \result == 0;
  @ complete behaviors;
  @ disjoint behaviors;
  @*/
int safe_add(int a, int b, int* result);
