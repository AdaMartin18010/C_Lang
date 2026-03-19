/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\02_C99_Library.md
 * Line: 224
 * Language: c
 * Block ID: 02d4ac55
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 注意：复数运算可能较慢
double complex z = ...;
double r = cabs(z);  // 涉及sqrt运算

// 如果只是比较大小，比较平方避免sqrt
if (creal(z)*creal(z) + cimag(z)*cimag(z) > threshold*threshold) {
    // 比 cabs(z) > threshold 更快
}
