/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\ISO_IEC_TS_17961\README.md
 * Line: 169
 * Language: c
 * Block ID: adba3a28
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 不合规代码 - 双重释放
void process_buffer(char** buf1, char** buf2) {
    free(*buf1);
    free(*buf2);  // 如果 buf1 == buf2，双重释放!
}

// 合规代码
void process_buffer_safe(char** buf1, char** buf2) {
    if (*buf1 == *buf2) {
        // 同一缓冲区，只释放一次
        free(*buf1);
        *buf1 = NULL;
        *buf2 = NULL;
    } else {
        free(*buf1);
        *buf1 = NULL;
        free(*buf2);
        *buf2 = NULL;
    }
}
