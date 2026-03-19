/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering\08_Code_Review_Checklist.md
 * Line: 1543
 * Language: c
 * Block ID: 9571b983
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file cyclomatic_complexity.c
 * @brief 圈复杂度示例与优化
 *
 * 圈复杂度计算：
 * - 基础值：1
 * - 每个 if/for/while/case/&&/|| 加1
 * - 推荐：函数圈复杂度 <= 10
 * - 强制：函数圈复杂度 <= 20
 */

#include <stdio.h>
#include <stdbool.h>

/* ========== 高复杂度示例（圈复杂度=12）========== */

/**
 * ❌ 圈复杂度 = 12，过高
 * 决策点：if(7个) + for(1个) + &&(2个) + 基础1 = 11？
 * 实际：每个条件分支都算，详细计算见下
 */
int process_command_complex(char cmd, int arg1, int arg2, int arg3)
{
    /* 基础复杂度: 1 */

    if (cmd == 'A') {              /* +1 = 2 */
        if (arg1 > 0) {            /* +1 = 3 */
            return arg1 * 2;
        } else {
            return -1;
        }
    } else if (cmd == 'B') {       /* +1 = 4 */
        if (arg2 > 0 && arg3 > 0) {/* +2 = 6 (两个条件) */
            for (int i = 0; i < arg2; i++) { /* +1 = 7 */
                if (i % 2 == 0) {  /* +1 = 8 */
                    arg3 += i;
                }
            }
            return arg3;
        }
        return 0;
    } else if (cmd == 'C') {       /* +1 = 9 */
        switch (arg1) {            /* +1 = 10 */
            case 1: return arg2;
            case 2: return arg3;
            default: return 0;
        }
    } else if (cmd == 'D') {       /* +1 = 11 */
        return arg1 + arg2 + arg3;
    }

    return -1;                     /* 总复杂度约 12 */
}

/* ========== 重构后（使用策略模式）========== */

/* 策略函数类型 */
typedef int (*command_handler_t)(int, int, int);

/* 具体命令处理函数（每个圈复杂度 <= 3） */
static int handle_cmd_a(int arg1, int arg2, int arg3)
{
    (void)arg2; (void)arg3;
    return (arg1 > 0) ? arg1 * 2 : -1;
}

static int handle_cmd_b(int arg1, int arg2, int arg3)
{
    (void)arg1;
    if (arg2 <= 0 || arg3 <= 0) {
        return 0;
    }

    int sum = arg3;
    for (int i = 0; i < arg2; i += 2) {  /* 优化：直接步进2 */
        sum += i;
    }
    return sum;
}

static int handle_cmd_c(int arg1, int arg2, int arg3)
{
    static const int lookup[] = {0, arg2, arg3};
    if (arg1 >= 1 && arg1 <= 2) {
        return lookup[arg1];
    }
    return 0;
}

static int handle_cmd_d(int arg1, int arg2, int arg3)
{
    return arg1 + arg2 + arg3;
}

/* 命令映射表 */
typedef struct {
    char cmd;
    command_handler_t handler;
} command_entry_t;

static const command_entry_t command_table[] = {
    {'A', handle_cmd_a},
    {'B', handle_cmd_b},
    {'C', handle_cmd_c},
    {'D', handle_cmd_d},
};

/**
 * ✅ 重构后圈复杂度 = 3
 * 使用查找表替代条件分支
 */
int process_command_simple(char cmd, int arg1, int arg2, int arg3)
{
    for (size_t i = 0; i < sizeof(command_table)/sizeof(command_table[0]); i++) {
        if (command_table[i].cmd == cmd) {
            return command_table[i].handler(arg1, arg2, arg3);
        }
    }
    return -1;  /* 未知命令 */
}

/* ========== 圈复杂度检查脚本 ========== */

#endif /* CYCLOMATIC_COMPLEXITY_C */
