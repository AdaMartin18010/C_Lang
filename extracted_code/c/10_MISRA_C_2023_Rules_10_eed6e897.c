/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\10_MISRA_C_2023_Rules_10.md
 * Line: 237
 * Language: c
 * Block ID: eed6e897
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 最小化作用域 */
void func(void)
{
    /* 声明靠近使用 */
    int local_temp;

    /* 循环变量在循环中声明 */
    for (int i = 0; i < 10; i++) {
        /* i只在循环内可见 */
    }

    /* 块作用域 */
    {
        int block_var = 42;
        use(block_var);
    } /* block_var作用域结束 */
}

/* ✅ 合规 - 需要时才打开 */
void process_file(void)
{
    FILE *fp = fopen("file", "r");
    if (fp == NULL) {
        return;
    }
    /* 使用fp */
    fclose(fp);
} /* fp作用域结束 */

/* ✅ 合规 - 结构体前向声明 */
struct InternalData;  /* 只暴露不透明指针 */

typedef struct InternalData *Handle;

Handle create_handle(void);
void destroy_handle(Handle h);
