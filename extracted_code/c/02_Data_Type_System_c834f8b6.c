/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\02_Data_Type_System.md
 * Line: 309
 * Language: c
 * Block ID: c834f8b6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

uint8_t a = 200, b = 100;

// 方法1: 扩大存储类型
unsigned int sum = (unsigned int)a + (unsigned int)b;

// 方法2: 使用更大类型进行运算
uint16_t c = (uint16_t)a + (uint16_t)b;

// 方法3: 条件判断前显式转换
if ((unsigned int)a + (unsigned int)b > 250) {
    // 正确判断
}
