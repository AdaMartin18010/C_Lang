/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\04_Cognitive_Representation\02_Embodied_Cognition.md
 * Line: 312
 * Language: c
 * Block ID: 20f701d4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 支持手势理解的调试命令

/*
调试手势隐喻：

← →  单步执行（一步一步走）
↑    跳出函数（向外走）
↓    进入函数（向内走）
⏸   暂停（停下）
▶   继续（前行）
⟲   重新开始（重来）
*/

// 调试命令接口
typedef enum {
    DEBUG_STEP_OVER,    // →
    DEBUG_STEP_INTO,    // ↓
    DEBUG_STEP_OUT,     // ↑
    DEBUG_CONTINUE,     // ▶
    DEBUG_PAUSE,        // ⏸
    DEBUG_RESTART       // ⟲
} DebugCommand;

void debugger_execute(DebugCommand cmd, DebugContext *ctx) {
    switch (cmd) {
        case DEBUG_STEP_OVER:
            // 执行当前行，不进入函数
            ctx->action = STEP_NEXT;
            break;
        case DEBUG_STEP_INTO:
            // 进入函数调用
            ctx->action = STEP_IN;
            break;
        case DEBUG_STEP_OUT:
            // 执行到函数返回
            ctx->action = STEP_OUT;
            break;
        // ...
    }
}
