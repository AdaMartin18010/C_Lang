/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\05_Eva_Tutorial.md
 * Line: 407
 * Language: c
 * Block ID: 2911c9f9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// tutorial3_array.c
#include <string.h>

#define SIZE 100

/*@
  @requires \valid(arr + (0..SIZE-1));
  @requires \initialized(arr + (0..SIZE-1));
  @assigns \nothing;
  @ensures \result >= 0;
*/
int find_max(const int arr[SIZE]) {
    int max = arr[0];

    // Eva推断:
    // i ∈ [1, SIZE]
    // max ∈ [min(arr), max(arr)]
    for (int i = 1; i < SIZE; i++) {
        if (arr[i] > max) {
            max = arr[i];
        }
    }

    return max;
}

/*@
  @requires \valid(dst + (0..SIZE-1));
  @requires \valid_read(src + (0..SIZE-1));
  @requires \separated(dst + (0..SIZE-1), src + (0..SIZE-1));
  @assigns dst[0..SIZE-1];
*/
void scale_array(int dst[SIZE], const int src[SIZE], int factor) {
    // Eva推断:
    // i ∈ [0, SIZE]
    // dst[0..i-1] 已初始化
    for (int i = 0; i < SIZE; i++) {
        dst[i] = src[i] * factor;
    }
}
