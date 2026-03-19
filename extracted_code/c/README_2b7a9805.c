/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\01_Game_Semantics\README.md
 * Line: 181
 * Language: c
 * Block ID: 2b7a9805
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * C 语言中的名称绑定与博弈位置
 */

#include <stdio.h>
#include <stdlib.h>

// 名称作为博弈的命名策略
typedef struct {
    int *value_ref;  // 对博弈位置的引用
} Name;

// 创建一个命名位置
Name create_name(int initial)
{
    Name n;
    n.value_ref = malloc(sizeof(int));
    *n.value_ref = initial;
    return n;
}

// 博弈中的询问动作 (Opponent)
int query(Name n)
{
    // O: "这个位置当前的值是什么?"
    return *n.value_ref;  // P 回答当前值
}

// 博弈中的更新动作 (Player)
void update(Name n, int new_value)
{
    // P: "我将这个位置更新为..."
    *n.value_ref = new_value;
    // O: 接受更新（继续博弈）
}

// 作用域作为博弈的可见性规则
void scoped_interaction(void)
{
    // 进入作用域 = 创建新的博弈子舞台
    Name x = create_name(0);

    // 在作用域内的交互
    int v = query(x);      // O 询问
    update(x, v + 1);      // P 更新

    // 离开作用域 = 博弈结束，资源释放
    free(x.value_ref);
}
