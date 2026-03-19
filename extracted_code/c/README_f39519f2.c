/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\ISO_IEC_TS_17961\README.md
 * Line: 108
 * Language: c
 * Block ID: f39519f2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 不合规代码
void process_file(const char* filename) {
    char command[256];
    sprintf(command, "cat %s", filename);  // 命令注入漏洞
    system(command);
}

// 合规代码
#include <fcntl.h>
#include <unistd.h>

bool display_file_safe(const char* filename) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) return false;

    char buffer[4096];
    ssize_t n;
    while ((n = read(fd, buffer, sizeof(buffer))) > 0) {
        write(STDOUT_FILENO, buffer, n);
    }

    close(fd);
    return n == 0;
}
