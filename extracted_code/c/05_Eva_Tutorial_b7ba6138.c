/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\05_Eva_Tutorial.md
 * Line: 915
 * Language: c
 * Block ID: b7ba6138
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// tutorial14_buffer.c
#include <string.h>

#define BUF_SIZE 256

// 字符串复制(安全版本)
/*@
  @requires \valid(dst + (0..dst_size-1));
  @requires \valid_read(src);
  @requires dst_size > 0;
  @assigns dst[0..dst_size-1];
  @ensures \forall integer i; 0 <= i < dst_size ==>
    (i < strlen(src) ==> dst[i] == src[i]) &&
    (i == strlen(src) ==> dst[i] == '\0');
*/
void safe_strcpy(char* dst, const char* src, size_t dst_size) {
    size_t i;
    for (i = 0; i < dst_size - 1 && src[i] != '\0'; i++) {
        dst[i] = src[i];
    }
    dst[i] = '\0';
}

// 缓冲区填充
/*@
  @requires \valid(buf + (0..n-1));
  @requires n >= 0;
  @assigns buf[0..n-1];
  @ensures \forall integer i; 0 <= i < n ==> buf[i] == value;
*/
void buffer_fill(char* buf, size_t n, char value) {
    for (size_t i = 0; i < n; i++) {
        buf[i] = value;
    }
}
