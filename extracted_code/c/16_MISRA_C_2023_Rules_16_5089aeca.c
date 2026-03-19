/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\16_MISRA_C_2023_Rules_16.md
 * Line: 100
 * Language: c
 * Block ID: 5089aeca
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 类型匹配 */
void process(int value);
process(42);  /* 匹配 */

/* ✅ 合规 - 显式转换 */
float f = 3.14f;
process((int)f);  /* 显式 */

/* ✅ 合规 - 使用正确类型 */
void set_size(size_t size);
size_t len = 100;
set_size(len);

/* ✅ 合规 - 类型安全包装 */
void process_float(float f)
{
    int value = (int)round(f);  /* 正确处理 */
    process(value);
}
