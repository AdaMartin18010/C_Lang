/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\09_Lab9_File_System.md
 * Line: 239
 * Language: c
 * Block ID: f2373138
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/fs.h

// 目录项结构 (16字节)
struct dirent {
    ushort inum;      // inode编号 (0表示空闲项)
    char name[14];    // 文件名 (以null结尾)
};

// 目录就是一个特殊的文件
// 包含dirent数组
// 查找时顺序扫描
