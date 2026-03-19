/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\07_Lab7_Network_Driver.md
 * Line: 405
 * Language: c
 * Block ID: ee17a411
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/mbuf.h

#ifndef MBUF_SIZE
#define MBUF_SIZE 2048
#endif

// mbuf头部大小
#define MBUF_HEADROOM 128

struct mbuf {
    struct mbuf *next;      // 链表指针
    char *head;             // 数据起始
    uint len;               // 数据长度
    char buf[MBUF_SIZE];    // 数据缓冲区
};

// 分配mbuf
struct mbuf *mbufalloc(uint headroom);

// 释放mbuf
void mbuffree(struct mbuf *m);

// 在头部预留空间
char *mbufpush(struct mbuf *m, uint len);

// 在尾部添加空间
char *mbufput(struct mbuf *m, uint len);

// 从头部移除数据
char *mbufpull(struct mbuf *m, uint len);

// 从尾部移除数据
void mbuftrim(struct mbuf *m, uint len);
