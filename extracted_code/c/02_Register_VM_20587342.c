/*
 * Auto-generated from: 03_System_Technology_Domains\01_Virtual_Machine_Interpreter\02_Register_VM.md
 * Line: 281
 * Language: c
 * Block ID: 20587342
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 函数调用栈帧布局（从base开始）：
 * [0]       : 函数本身（closure）
 * [1..n]    : 固定参数（由调用者设置）
 * [n+1..]   : 变长参数/临时变量
 * [..top-1] : 表达式临时值
 *
 * 寄存器分配策略：
 * - 参数优先使用低编号寄存器（R[0], R[1], ...）
 * - 局部变量紧随其后
 * - 临时表达式使用高编号寄存器
 */

typedef struct {
    RVValue *func;        /* 被调函数 */
    RVValue *base;        /* 当前帧基址 */
    uint32_t saved_pc;    /* 返回地址 */
    uint16_t nresults;    /* 期望返回值数量 */
    uint16_t nparams;     /* 参数数量 */
} RVCallInfo;

typedef struct {
    RVValue stack[RV_STACK_SIZE];
    RVCallInfo ci_stack[RV_MAX_CALL_DEPTH];
    RVCallInfo *ci;       /* 当前调用信息 */
    RVValue *top;         /* 栈顶 */
    int stack_size;
} RVStack;
