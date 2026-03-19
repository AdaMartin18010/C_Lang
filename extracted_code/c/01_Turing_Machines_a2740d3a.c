/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\07_Computability_Theory\01_Turing_Machines.md
 * Line: 113
 * Language: c
 * Block ID: a2740d3a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 图灵机的完整C实现

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define TAPE_SIZE 10000
#define MAX_STATES 100
#define MAX_TRANSITIONS 500

typedef enum { LEFT = -1, RIGHT = 1 } Direction;
typedef enum {
    STATE_NORMAL = 0,
    STATE_ACCEPT = 1,
    STATE_REJECT = 2
} StateType;

// 转移函数条目
typedef struct {
    int from_state;
    char read_symbol;
    int to_state;
    char write_symbol;
    Direction move;
} Transition;

// 图灵机结构
typedef struct {
    char tape[TAPE_SIZE];
    int head;                    // 读写头位置
    int current_state;
    int num_states;
    StateType state_types[MAX_STATES];
    Transition transitions[MAX_TRANSITIONS];
    int num_transitions;
    int step_count;              // 执行的步数
} TuringMachine;

// 初始化图灵机
void tm_init(TuringMachine *tm, const char *input) {
    memset(tm->tape, '_', TAPE_SIZE);  // 空白符
    strcpy(tm->tape + TAPE_SIZE/2, input);  // 输入放在中间
    tm->head = TAPE_SIZE / 2;
    tm->current_state = 0;
    tm->step_count = 0;
}

// 添加转移函数
void tm_add_transition(TuringMachine *tm, int from, char read,
                        int to, char write, Direction dir) {
    Transition *t = &tm->transitions[tm->num_transitions++];
    t->from_state = from;
    t->read_symbol = read;
    t->to_state = to;
    t->write_symbol = write;
    t->move = dir;
}

// 单步执行
bool tm_step(TuringMachine *tm) {
    char current_symbol = tm->tape[tm->head];

    // 查找匹配的转移
    for (int i = 0; i < tm->num_transitions; i++) {
        Transition *t = &tm->transitions[i];
        if (t->from_state == tm->current_state &&
            t->read_symbol == current_symbol) {

            // 执行转移
            tm->tape[tm->head] = t->write_symbol;
            tm->current_state = t->to_state;
            tm->head += t->move;
            tm->step_count++;

            // 检查边界
            if (tm->head < 0 || tm->head >= TAPE_SIZE) {
                fprintf(stderr, "Error: Head out of bounds\n");
                return false;
            }

            return true;
        }
    }

    // 无匹配转移，停机
    return false;
}

// 运行直到停机
int tm_run(TuringMachine *tm, int max_steps) {
    while (tm->step_count < max_steps) {
        // 检查是否到达接受/拒绝状态
        if (tm->state_types[tm->current_state] == STATE_ACCEPT) {
            return 1;  // 接受
        }
        if (tm->state_types[tm->current_state] == STATE_REJECT) {
            return -1;  // 拒绝
        }

        if (!tm_step(tm)) {
            return 0;  // 停机（无定义转移）
        }
    }

    return -2;  // 超过最大步数（可能不停机）
}
