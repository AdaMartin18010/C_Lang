/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering\08_Code_Review_Checklist.md
 * Line: 655
 * Language: c
 * Block ID: 471723fa
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误示例：路径遍历漏洞
void read_config(const char *user_config) {
    char path[128] = "/etc/myapp/";
    strcat(path, user_config);
    // 攻击者输入: "../../etc/passwd"
    FILE *f = fopen(path, "r");
}

// ✅ 正确示例：路径规范化与验证
#include <limits.h>
#include <stdlib.h>

int read_config_safe(const char *user_config, char *output, size_t out_size) {
    // 基础验证
    if (validate_filename(user_config) != 0) {
        return -1;
    }

    // 构建完整路径
    char full_path[PATH_MAX];
    if (snprintf(full_path, sizeof(full_path), "/etc/myapp/%s", user_config) >= sizeof(full_path)) {
        return -1;  // 路径过长
    }

    // 规范化路径（解析符号链接、. 和 ..）
    char resolved[PATH_MAX];
    if (realpath(full_path, resolved) == NULL) {
        return -1;
    }

    // 确保解析后的路径仍在允许的目录下
    const char *allowed_prefix = "/etc/myapp/";
    if (strncmp(resolved, allowed_prefix, strlen(allowed_prefix)) != 0) {
        return -1;  // 路径遍历尝试！
    }

    FILE *f = fopen(resolved, "r");
    if (f == NULL) return -1;

    // 读取内容...
    fclose(f);
    return 0;
}
