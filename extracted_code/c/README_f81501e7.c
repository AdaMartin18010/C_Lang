/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\01_Game_Semantics\README.md
 * Line: 459
 * Language: c
 * Block ID: f81501e7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 使用所有权类型模拟线性逻辑
 */

#include <stdio.h>
#include <stdlib.h>

// 线性资源：只能移动，不能复制
typedef struct {
    int data;
    bool consumed;
} Linear;

// 创建线性资源（所有权初始化）
Linear linear_new(int value)
{
    return (Linear){ .data = value, .consumed = false };
}

// 消费线性资源（所有权转移）
int linear_consume(Linear *res)
{
    if (res->consumed) {
        fprintf(stderr, "Error: Double consumption\n");
        abort();
    }
    res->consumed = true;
    return res->data;
}

// 线性函数：A ⊸ B
typedef Linear (*LinearFn)(Linear);

// 示例：消耗 int 返回加倍后的 int
Linear double_linear(Linear x)
{
    int v = linear_consume(&x);
    return linear_new(v * 2);
}

// 线性复合：(A ⊸ B) ⊸ (B ⊸ C) ⊸ (A ⊸ C)
Linear linear_compose(Linear x, LinearFn f, LinearFn g)
{
    Linear y = f(x);
    Linear z = g(y);
    return z;
}

// 使用示例
void linear_example(void)
{
    Linear x = linear_new(21);

    // 可以转移所有权
    Linear y = double_linear(x);
    // x 现在已消费，不能再使用

    int result = linear_consume(&y);
    printf("Result: %d\n", result);
    // y 现在已消费
}

// 文件句柄作为线性资源
typedef struct {
    FILE *handle;
    bool closed;
} LinearFile;

LinearFile linear_fopen(const char *path)
{
    FILE *f = fopen(path, "r");
    if (!f) perror("fopen");
    return (LinearFile){ .handle = f, .closed = false };
}

void linear_fclose(LinearFile *lf)
{
    if (lf->closed) {
        fprintf(stderr, "Error: File already closed\n");
        return;
    }
    fclose(lf->handle);
    lf->closed = true;
}
