/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\01_Game_Semantics\01_Game_Semantics_Theory.md
 * Line: 154
 * Language: c
 * Block ID: bfed166f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C语言模拟策略结构
#include <stdbool.h>
#include <string.h>

typedef struct {
    int move_id;
    int player;  // 0=O, 1=P
    int type;    // 0=Question, 1=Answer
    char label[64];
} Move;

typedef struct Position {
    Move *moves;
    int length;
    int capacity;
} Position;

typedef struct Strategy {
    // 策略是合法位置的集合
    Position **positions;
    int count;
    int capacity;

    // 判断给定位置是否属于策略
    bool (*contains)(struct Strategy *, Position *);

    // 给定偶数长度位置，返回P的响应
    Move *(*respond)(struct Strategy *, Position *);
} Strategy;
