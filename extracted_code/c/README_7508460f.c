/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\05_Practical_Verification_Cases\README.md
 * Line: 161
 * Language: c
 * Block ID: 7508460f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*@
  @ requires a > 0;
  @ requires b > 0;
  @ requires \gcd(a, b) == 1;  // 互质
  @ assigns \nothing;
  @ ensures \result > 0;
  @ ensures (\result % a) == 0;
  @ ensures (\result % b) == 0;
  @ ensures \forall integer x; (x % a == 0 && x % b == 0) ==> x >= \result;
*/
int lcm(int a, int b);
