/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\09_Lab9_File_System.md
 * Line: 719
 * Language: c
 * Block ID: abd2a288
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 创建大量文件
void stress_test() {
    char name[32];

    for (int i = 0; i < 100; i++) {
        snprintf(name, sizeof(name), "file%d", i);
        int fd = open(name, O_CREATE | O_WRONLY);

        // 写入一些数据
        for (int j = 0; j < 10; j++) {
            write(fd, name, strlen(name));
        }
        close(fd);
    }

    // 删除所有文件
    for (int i = 0; i < 100; i++) {
        snprintf(name, sizeof(name), "file%d", i);
        unlink(name);
    }
}
