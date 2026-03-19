/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\04_Frama_C_2024_Guide.md
 * Line: 516
 * Language: c
 * Block ID: 05d33093
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*@
  @requires \valid(dst) && \valid(src);
  @assigns *dst;

  @behavior success:
    @assumes *src >= 0 && *src <= 255;
    @assigns *dst;
    @ensures *dst == *src;
    @ensures \result == 0;

  @behavior error_overflow:
    @assumes *src > 255;
    @assigns \nothing;
    @ensures \result == -1;

  @behavior error_underflow:
    @assumes *src < 0;
    @assigns \nothing;
    @ensures \result == -2;

  @complete behaviors;
  @disjoint behaviors;
*/
int safe_assign_byte(unsigned char* dst, const int* src);
