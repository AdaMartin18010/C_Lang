/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\01_C89_Library.md
 * Line: 119
 * Language: c
 * Block ID: 31497a0f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 格式字符串漏洞
void log_message(const char *msg) {
    printf(msg);  // 如果msg包含%，崩溃或信息泄漏
}
// 攻击输入: "%s%s%s%s%s%s" 导致读取任意内存

// ✅ 修正
void log_message_safe(const char *msg) {
    printf("%s", msg);  // 固定格式
    // 或
    fputs(msg, stdout);
}

// ❌ sprintf缓冲区溢出
char buf[10];
int x = 1234567890;
sprintf(buf, "%d", x);  // "1234567890" = 10字符 + '\0' = 溢出！

// ✅ C99 snprintf（推荐）
int len = snprintf(buf, sizeof(buf), "%d", x);
if (len >= sizeof(buf)) {
    // 输出被截断，需要更大缓冲区
}

// ✅ 动态格式化：计算所需大小
int format_dynamic(int x) {
    int len = snprintf(NULL, 0, "%d", x);  // C99：获取所需大小
    char *buf = malloc(len + 1);
    if (!buf) return -1;
    sprintf(buf, "%d", x);
    // 使用buf...
    free(buf);
    return 0;
}
