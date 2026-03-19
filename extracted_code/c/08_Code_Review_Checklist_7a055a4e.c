/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering\08_Code_Review_Checklist.md
 * Line: 609
 * Language: c
 * Block ID: 7a055a4e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误示例：命令注入
void process_file(const char *filename) {
    char cmd[256];
    // 用户输入直接拼接到命令中
    sprintf(cmd, "cat %s", filename);
    system(cmd);  // 危险！
    // 攻击者输入: "; rm -rf /;"
}

// ✅ 正确示例：使用exec系列函数
#include <unistd.h>

void process_file_safe(const char *filename) {
    // 使用execl，参数不作为shell命令解析
    execl("/bin/cat", "cat", filename, (char *)NULL);
}

// ✅ 更好示例：严格的文件名验证
#include <ctype.h>

int validate_filename(const char *filename) {
    if (filename == NULL) return -1;

    // 禁止路径遍历字符
    if (strstr(filename, "..") != NULL) return -1;
    if (strchr(filename, ';') != NULL) return -1;
    if (strchr(filename, '|') != NULL) return -1;
    if (strchr(filename, '&') != NULL) return -1;
    if (strchr(filename, '$') != NULL) return -1;
    if (strchr(filename, '`') != NULL) return -1;

    // 只允许安全字符
    const char *p = filename;
    while (*p) {
        if (!isalnum(*p) && *p != '.' && *p != '_' && *p != '-') {
            return -1;
        }
        p++;
    }
    return 0;
}
