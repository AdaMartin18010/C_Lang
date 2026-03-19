/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\01_Game_Semantics\README.md
 * Line: 98
 * Language: c
 * Block ID: 00fbcd88
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * C 程序中的博弈语义映射
 *
 * 函数调用 = 博弈启动
 * 参数传递 = Opponent 的询问
 * 返回值   = Player 的回答
 * 副作用   = 与环境的交互
 */

// 基本值博弈
int get_value(void)
{
    // O: "给我一个 int"
    return 42;  // P: "这里是 42"
}

// 函数类型博弈
int add(int x, int y)
{
    // O: "add 作用于什么?"
    // P: "给我第一个参数"
    // O: "x = 3"
    // P: "给我第二个参数"
    // O: "y = 5"
    return x + y;  // P: "结果是 8"
}

// 高阶函数博弈
int apply(int (*f)(int), int x)
{
    // O: "f 是什么函数?"
    // O: "x 是什么值?"
    return f(x);  // 启动子博弈
}

// 指针作为博弈引用
typedef struct {
    int (*query)(void);   // O 询问
    void (*command)(int); // P 执行
} Interface;

// 博弈中的命名位置
void interaction_example(Interface *iface)
{
    // O 询问当前状态
    int state = iface->query();  // P 回答

    // P 执行命令
    iface->command(state + 1);   // O 接受执行
}
