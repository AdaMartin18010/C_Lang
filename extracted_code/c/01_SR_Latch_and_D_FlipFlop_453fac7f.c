/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\03_Memory_Circuits\01_SR_Latch_and_D_FlipFlop.md
 * Line: 1100
 * Language: c
 * Block ID: 453fac7f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C语言状态机
enum State { IDLE, RUNNING, PAUSED, STOPPED };
enum State current_state = IDLE;

void state_machine(int event) {
    switch (current_state) {
        case IDLE:
            if (event == START) current_state = RUNNING;
            break;
        case RUNNING:
            if (event == PAUSE) current_state = PAUSED;
            if (event == STOP)  current_state = STOPPED;
            break;
        // ...
    }
}

/* 硬件状态机实现：
 *
 *          ┌─────────────────────────────────────────┐
 *          │           组合逻辑 (下一状态)            │
 *          │                                         │
 *  输入 ───┤  current_state  event  ──→ next_state  │
 *          │                                         │
 *          └──────────────────┬──────────────────────┘
 *                             │
 *                             ▼
 *          ┌─────────────────────────────────────────┐
 *          │      D触发器组 (状态寄存器)              │
 *          │                                         │
 *          │    CLK ↑ : 采样 next_state ──→ Q       │
 *          │                                         │
 *          │    Q ──────────────────────────────────┘
 *          │    │
 *          │    └──→ current_state (输出到组合逻辑)
 *          └─────────────────────────────────────────┘
 */
