/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_Rules_Dir11.md
 * Line: 806
 * Language: c
 * Block ID: a24020ea
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 预期规则方向 [待官方确认] */
/* 函数定义规范 */

/* ❌ 违反 [待官方确认] - 旧式函数定义 */
int old_style(a, b)
int a;
int b;
{  /* K&R风格 */
    return a + b;
}

/* ❌ 违反 [待官方确认] - 无返回类型 */
func(int x) {  /* 隐式int返回 */
    return x;
}

/* ❌ 违反 [待官方确认] - 空参数列表 */
void func();  /* 不是void函数，参数未指定 */

/* ✅ 合规 [待官方确认] */
int modern_style(int a, int b) {
    return a + b;
}

/* ✅ 合规 [待官方确认] */
void func(void) {  /* 明确表示无参数 */
    /* ... */
}

/* ✅ 合规 [待官方确认] */
void process(const char *input);  /* 原型 */
void process(const char *input) {  /* 定义匹配 */
    /* ... */
}
