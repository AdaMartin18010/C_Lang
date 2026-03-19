/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\DO_178C\01_Software_Levels_and_Objectives.md
 * Line: 104
 * Language: c
 * Block ID: 8df48e9f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * DAL A级别函数 - MC/DC覆盖要求
 */

/* 判定1: 高度检查 */
bool check_altitude(int32_t current, int32_t target, int32_t tolerance)
{
    int32_t diff = abs(current - target);

    /* 判定: (diff <= tolerance) && (current > 0) */
    if ((diff <= tolerance) && (current > 0)) {
        return true;  /* 在目标高度范围内 */
    }
    return false;
}

/* MC/DC测试用例 */
void test_check_altitude(void)
{
    /* 证明diff <= tolerance独立影响输出 */
    TEST(check_altitude(1000, 1005, 10, 1000) == true);   /* T && T = T */
    TEST(check_altitude(1000, 1050, 10, 1000) == false);  /* F && T = F */

    /* 证明current > 0独立影响输出 */
    TEST(check_altitude(1000, 1005, 10, 1000) == true);   /* T && T = T */
    TEST(check_altitude(-100, 1005, 10, -100) == false);  /* T && F = F */
}

/* 复杂判定 */
bool complex_decision(bool a, bool b, bool c, bool d)
{
    /* 判定: (a && b) || (c && d) */
    return (a && b) || (c && d);
}

/* MC/DC需要证明a,b,c,d每个都能独立影响输出 */
void test_complex_decision(void)
{
    /* a独立: 其他固定，改变a改变输出 */
    TEST(complex_decision(true, true, false, false) == true);   /* T&&T || F = T */
    TEST(complex_decision(false, true, false, false) == false); /* F&&T || F = F */

    /* b独立 */
    TEST(complex_decision(true, true, false, false) == true);   /* T&&T || F = T */
    TEST(complex_decision(true, false, false, false) == false); /* T&&F || F = F */

    /* c独立 */
    TEST(complex_decision(false, false, true, true) == true);   /* F || T&&T = T */
    TEST(complex_decision(false, false, false, true) == false); /* F || F&&T = F */

    /* d独立 */
    TEST(complex_decision(false, false, true, true) == true);   /* F || T&&T = T */
    TEST(complex_decision(false, false, true, false) == false); /* F || T&&F = F */
}
