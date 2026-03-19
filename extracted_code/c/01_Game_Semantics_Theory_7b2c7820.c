/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\01_Game_Semantics\01_Game_Semantics_Theory.md
 * Line: 455
 * Language: c
 * Block ID: 7b2c7820
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 简单的博弈求解器实现
#include <stdio.h>
#include <stdbool.h>

typedef enum { POSITIVE, NEGATIVE } Outcome;

// 博弈树节点
typedef struct GameNode {
    bool is_terminal;
    Outcome outcome;  // 仅当is_terminal时有效
    struct GameNode **children;
    int num_children;
    int depth;
} GameNode;

// 计算博弈值（极小极大算法）
Outcome solve_game(GameNode *node, bool maximizing) {
    if (node->is_terminal) {
        return node->outcome;
    }

    if (maximizing) {
        Outcome best = NEGATIVE;
        for (int i = 0; i < node->num_children; i++) {
            Outcome val = solve_game(node->children[i], false);
            if (val == POSITIVE) return POSITIVE;
        }
        return NEGATIVE;
    } else {
        Outcome best = POSITIVE;
        for (int i = 0; i < node->num_children; i++) {
            Outcome val = solve_game(node->children[i], true);
            if (val == NEGATIVE) return NEGATIVE;
        }
        return POSITIVE;
    }
}

// 打印博弈树
void print_game_tree(GameNode *node, int indent) {
    for (int i = 0; i < indent; i++) printf("  ");

    if (node->is_terminal) {
        printf("[终端: %s]\n",
               node->outcome == POSITIVE ? "正方胜" : "反方胜");
    } else {
        printf("[%d个子节点]\n", node->num_children);
        for (int i = 0; i < node->num_children; i++) {
            print_game_tree(node->children[i], indent + 1);
        }
    }
}
