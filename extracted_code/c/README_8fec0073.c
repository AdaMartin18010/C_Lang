/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\01_Game_Semantics\README.md
 * Line: 348
 * Language: c
 * Block ID: 8fec0073
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 策略作为函数：位置 -> 移动
 */

#include <stdbool.h>
#include <stdlib.h>

// 博弈位置（简化表示）
typedef struct Position {
    int move_history[100];
    int length;
} Position;

// 移动类型
typedef enum { MOVE_QUESTION, MOVE_ANSWER } MoveType;

typedef struct Move {
    MoveType type;
    int label;
} Move;

// 策略：给定位置，选择移动
typedef Move (*Strategy)(const Position *pos);

// 常数策略：总是返回固定值
Move const_strategy(const Position *pos)
{
    (void)pos;  // 忽略历史
    return (Move){ .type = MOVE_ANSWER, .label = 42 };
}

// 复制策略：返回接收到的值
Move copy_strategy(const Position *pos)
{
    // 查找最后一个答案
    for (int i = pos->length - 1; i >= 0; i--) {
        // 假设偶数索引是答案
        if (i % 2 == 1) {
            return (Move){
                .type = MOVE_ANSWER,
                .label = pos->move_history[i]
            };
        }
    }
    return (Move){ .type = MOVE_ANSWER, .label = 0 };
}

// 顺序组合策略
Move compose_strategy(const Position *pos, Strategy s1, Strategy s2)
{
    // 根据位置历史决定调用哪个策略
    if (pos->length < 2) {
        return s1(pos);
    } else {
        return s2(pos);
    }
}

// 博弈执行引擎
void run_game(Strategy player, Strategy opponent, int max_moves)
{
    Position pos = { .length = 0 };
    bool player_turn = false;  // O 先开始

    while (pos.length < max_moves) {
        Move m;
        if (player_turn) {
            m = player(&pos);
            printf("P: %s %d\n",
                   m.type == MOVE_QUESTION ? "?" : "!", m.label);
        } else {
            m = opponent(&pos);
            printf("O: %s %d\n",
                   m.type == MOVE_QUESTION ? "?" : "!", m.label);
        }

        pos.move_history[pos.length++] = m.label;
        player_turn = !player_turn;

        // 终止条件：最终答案
        if (m.type == MOVE_ANSWER && pos.length > 2) break;
    }
}
