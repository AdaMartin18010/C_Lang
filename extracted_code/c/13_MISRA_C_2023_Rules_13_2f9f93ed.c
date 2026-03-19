/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\13_MISRA_C_2023_Rules_13.md
 * Line: 38
 * Language: c
 * Block ID: 2f9f93ed
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 过长函数 */
void process_everything(void)
{
    /* 100+ 行代码 */
    /* 打开文件 */
    /* 读取配置 */
    /* 初始化硬件 */
    /* 主处理循环 */
    /* 保存结果 */
    /* 关闭资源 */
}

/* ✅ 拆分为小函数 */
static int load_config(Config *cfg);
static int init_hardware(void);
static int process_data(const Config *cfg);
static int save_results(void);

void process_everything(void)
{
    Config cfg = {0};

    if (load_config(&cfg) != 0) {
        return;
    }

    if (init_hardware() != 0) {
        return;
    }

    if (process_data(&cfg) != 0) {
        return;
    }

    save_results();
}
