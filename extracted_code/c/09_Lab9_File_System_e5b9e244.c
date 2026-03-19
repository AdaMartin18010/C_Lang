/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\09_Lab9_File_System.md
 * Line: 81
 * Language: c
 * Block ID: e5b9e244
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/fs.h

// Inode磁盘结构 (64字节)
struct dinode {
    short type;           // 文件类型: 0=空闲, 1=目录, 2=文件, 3=设备
    short major;          // 主设备号
    short minor;          // 次设备号
    short nlink;          // 硬链接计数
    uint size;            // 文件大小(字节)
    uint addrs[NDIRECT+1]; // 数据块地址
};

#define NDIRECT 12
#define NINDIRECT (BSIZE / sizeof(uint))  // 256
#define MAXFILE (NDIRECT + NINDIRECT)     // 268 blocks

// 内存中的inode结构
struct inode {
    uint dev;             // 设备号
    uint inum;            // inode编号
    int ref;              // 引用计数
    struct sleeplock lock; // 保护以下字段
    int valid;            // 已从磁盘读取

    // dinode的副本
    short type;
    short major;
    short minor;
    short nlink;
    uint size;
    uint addrs[NDIRECT+1];
};
