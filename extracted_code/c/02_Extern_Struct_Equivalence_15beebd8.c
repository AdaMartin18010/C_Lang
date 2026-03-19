/*
 * Auto-generated from: 08_Zig_C_Connection\01_Type_System_Mapping\02_Extern_Struct_Equivalence.md
 * Line: 1980
 * Language: c
 * Block ID: 15beebd8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C 代码 - complex_struct.c
#ifndef COMPLEX_STRUCT_H
#define COMPLEX_STRUCT_H

#include <stdint.h>
#include <stddef.h>

// 嵌套结构体 - 模拟配置数据结构
struct ConfigValue {
    uint32_t type;
    union {
        int64_t i;
        double f;
        struct {
            const char* data;
            size_t len;
        } s;
        struct {
            void* ptr;
            void (*destructor)(void*);
        } p;
    } value;
};

// 网络包头
struct __attribute__((packed)) PacketHeader {
    uint8_t version;
    uint8_t type;
    uint16_t length;
    uint32_t seq;
    uint64_t timestamp;
};

// 内存分配器统计
struct AllocatorStats {
    struct {
        size_t current;
        size_t peak;
        size_t total;
    } used;
    struct {
        size_t current;
        size_t total;
    } blocks;
    uint32_t alloc_count;
    uint32_t free_count;
};

// 文件元数据
struct FileMetadata {
    uint64_t inode;
    uint64_t size;
    struct {
        int64_t sec;
        int32_t nsec;
    } atime, mtime, ctime;
    uint32_t mode;
    uint32_t uid;
    uint32_t gid;
    uint32_t links;
};

#endif
