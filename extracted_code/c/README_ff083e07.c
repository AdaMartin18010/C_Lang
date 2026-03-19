/*
 * Auto-generated from: 06_Thinking_Representation\04_Case_Studies\README.md
 * Line: 514
 * Language: c
 * Block ID: ff083e07
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 原始代码：混乱的协议解析器
int parse_packet(char* data, int len) {
    if (len < 4) return -1;
    int type = data[0];
    int flags = data[1];
    int length = (data[2] << 8) | data[3];

    if (type == 1) {
        if (len < 4 + length) return -1;
        // 处理类型 1
        for (int i = 0; i < length; i++) {
            printf("%c", data[4 + i]);
        }
        return 4 + length;
    } else if (type == 2) {
        if (len < 8) return -1;
        int id = (data[4] << 24) | (data[5] << 16) | (data[6] << 8) | data[7];
        printf("ID: %d\n", id);
        return 8;
    } else if (type == 3) {
        // 更多混乱的代码...
        return -1;
    }
    return -1;
}
