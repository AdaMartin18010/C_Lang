/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering_Layer\02_Code_Quality.md
 * Line: 291
 * Language: c
 * Block ID: 86e7ffb5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 检查时间到使用时间竞争
if (access("/tmp/file", W_OK) == 0) {  // 检查时
    FILE *fp = fopen("/tmp/file", "w");  // 使用时可能已改变！
    // ...
}

// ✅ 原子操作
define _GNU_SOURCE
#include <fcntl.h>

int fd = open("/tmp/file", O_WRONLY | O_CREAT | O_NOFOLLOW, 0600);
if (fd < 0) {
    // 处理错误
}
// 使用fd安全操作
