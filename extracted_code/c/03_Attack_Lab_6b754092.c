/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\03_Attack_Lab.md
 * Line: 68
 * Language: c
 * Block ID: 6b754092
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void vulnerable_function() {
    char buffer[80];           // 栈上缓冲区
    gets(buffer);              // 不检查边界！
    // 如果输入超过80字节，会覆盖返回地址
}
