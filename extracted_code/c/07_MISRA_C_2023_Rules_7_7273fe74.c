/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\07_MISRA_C_2023_Rules_7.md
 * Line: 351
 * Language: c
 * Block ID: 7273fe74
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 索引检查 */
void process(int index)
{
    int arr[10];
    if (index < 0 || index >= 10) {
        /* 错误处理 */
        return;
    }
    arr[index] = 42;
}

/* ✅ 合规 - 使用安全字符串函数 */
char buf[10];
strncpy(buf, "Hello, World!", sizeof(buf) - 1);
buf[sizeof(buf) - 1] = '\0';  /* 确保终止 */

/* ✅ 合规 - 安全的sprintf */
char buf[10];
int x = 123;
snprintf(buf, sizeof(buf), "%d", x);  /* 最多写入9字符+'\0' */

/* ✅ 合规 - 安全的memcpy */
char dest[5];
char src[10] = "123456789";
size_t copy_len = sizeof(dest) < 10 ? sizeof(dest) : 10;
memcpy(dest, src, copy_len);
