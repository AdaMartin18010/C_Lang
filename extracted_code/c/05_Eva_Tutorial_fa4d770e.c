/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\05_Eva_Tutorial.md
 * Line: 343
 * Language: c
 * Block ID: fa4d770e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// tutorial2_sum.c
#include <stdio.h>

/*@
  @requires n >= 0;
  @requires n <= 1000;  // 限制范围以便精确分析
  @assigns \nothing;
*/
int sum_n(int n) {
    int sum = 0;
    int i = 0;

    // Eva会自动推断:
    // i ∈ [0, n]
    // sum ∈ [0, n*(n-1)/2]  (实际为三角形数)
    while (i < n) {
        sum += i;
        i++;
    }

    return sum;
}

int main() {
    int result = sum_n(100);
    printf("Sum: %d\n", result);
    return 0;
}
