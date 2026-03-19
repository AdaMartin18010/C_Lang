/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\05_Frama_C_LLM_Integration.md
 * Line: 252
 * Language: c
 * Block ID: 06278f21
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 原始代码：字符串复制 */
void strcpy(char *dest, const char *src) {
    while ((*dest++ = *src++) != '\0')
        ;
}

/* LLM生成的ACSL规范 */
/*@
  @ requires \valid(dest) && \valid(src);
  @ requires \separated(dest, src);
  @ requires \valid(dest+(0..strlen(src)));
  @ requires \valid(src+(0..strlen(src)));
  @ ensures \forall integer i; 0 <= i <= strlen(src) ==>
  @         dest[i] == \old(src[i]);
  @ ensures dest[strlen(src)] == '\0';
  @ ensures \result == dest;
  @ assigns dest[0..strlen(src)];
  @*/
char *strcpy(char *dest, const char *src);
