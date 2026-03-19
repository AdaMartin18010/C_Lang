/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\09_Lab9_File_System.md
 * Line: 422
 * Language: c
 * Block ID: 08184450
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/log.c

// 日志头 (存储在日志区的第一个块)
struct logheader {
    int n;              // 已提交的块数
    int block[LOGSIZE]; // 目标块号数组
};

struct log {
    struct spinlock lock;
    int start;          // 日志区起始块号
    int size;           // 日志区大小
    int outstanding;    // 未完成的事务数
    int committing;     // 是否正在提交
    int dev;
    struct logheader lh;
};

struct log log;
