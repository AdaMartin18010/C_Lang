/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\06_Modern_C_C23.md
 * Line: 290
 * Language: c
 * Block ID: d015349b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 二进制输出
printf("Binary: %b\n", 42);   // 输出: 101010

// 更好的bool输出
printf("Flag: %B\n", true);   // 输出: TRUE (或1)

// 新增长度修饰符
int32_t i32 = 42;
printf("int32: %w32d\n", i32);  // w32, w64 修饰符

// 字符类型
c16_t c16 = u'中';
printf("char16: %lc\n", (wint_t)c16);
