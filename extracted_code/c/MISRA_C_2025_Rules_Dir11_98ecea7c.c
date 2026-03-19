/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir11.md
 * Line: 682
 * Language: c
 * Block ID: 98ecea7c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 预期规则方向 [待官方确认] */
/* 显式转换（强制类型转换）使用 */

/* ❌ 违反 [待官方确认] - 可能导致信息丢失 */
int large = 100000;
short small = (short)large;  /* 截断 */

/* ✅ 合规 [待官方确认] - 检查范围后转换 */
if (large > SHRT_MAX || large < SHRT_MIN) {
    /* 错误处理 */
} else {
    short small = (short)large;  /* 安全 */
}

/* ✅ 合规 [待官方确认] - 明确意图 */
float f = 3.7f;
int i = (int)f;  /* 明确截断小数 */
int rounded = (int)round(f);  /* 正确舍入 */
