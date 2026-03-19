/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\C23_Standard_Library\02_stdckdint_h_Complete_Reference.md
 * Line: 28
 * Language: c
 * Block ID: afa1eb8b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23 引入 <stdckdint.h> 解决整数溢出问题
// 整数溢出是安全漏洞的主要来源之一

#include <stdckdint.h>
#include <stdio.h>
#include <limits.h>

int main(void) {
    int result;

    // 安全加法
    bool overflow = ckd_add(&result, INT_MAX, 1);
    if (overflow) {
        printf("Overflow detected!\n");
        // result 包含回绕值，但不使用
    }

    return 0;
}
