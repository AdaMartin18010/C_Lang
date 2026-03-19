/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\02_MISRA_C_2023_Rules_2.md
 * Line: 287
 * Language: c
 * Block ID: 6013046d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 使用表达式结果 */
void process(int x)
{
    int result = x + 5;
    use_result(result);
}

/* ✅ 合规 - 明确的意图 */
void count(void)
{
    static int counter = 0;
    counter++;  /* 明确用于计数 */
}

/* ✅ 合规 - 使用读取的数据 */
void read_sensor(void)
{
    int value = adc_read();
    if (value > THRESHOLD) {
        trigger_alarm();
    }
}

/* ✅ 合规 - 显式void转换表示故意忽略 */
void discard_result(void)
{
    (void)printf("Message\n");  /* 显式忽略返回值 */
}
