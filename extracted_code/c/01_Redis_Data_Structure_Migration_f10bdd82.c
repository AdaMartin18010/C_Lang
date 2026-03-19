/*
 * Auto-generated from: 08_Zig_C_Connection\07_Real_World_Migration_Cases\01_Redis_Data_Structure_Migration.md
 * Line: 50
 * Language: c
 * Block ID: f10bdd82
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// sds.h - Redis 原始实现简化版
#ifndef SDS_H
#define SDS_H

#include <sys/types.h>
#include <stdarg.h>
#include <stdint.h>

typedef char* sds;

// SDS 头部结构
struct __attribute__((__packed__)) sdshdr5 {
    unsigned char flags; /* 3 lsb of type, 5 msb of strlen */
    char buf[];
};
struct __attribute__((__packed__)) sdshdr8 {
    uint8_t len;
    uint8_t alloc;
    unsigned char flags;
    char buf[];
};
// ... sdshdr16, sdshdr32, sdshdr64

#define SDS_TYPE_5  0
#define SDS_TYPE_8  1
#define SDS_TYPE_16 2
#define SDS_TYPE_32 3
#define SDS_TYPE_64 4

// API
sds sdsnewlen(const void* init, size_t initlen);
sds sdsnew(const char* init);
void sdsfree(sds s);
sds sdscatlen(sds s, const void* t, size_t len);
sds sdscat(sds s, const char* t);
sds sdscatsds(sds s, const sds t);
sds sdsdup(const sds s);
sds sdsgrowzero(sds s, size_t len);
sds sdscatprintf(sds s, const char* fmt, ...);

// 获取属性
static inline size_t sdslen(const sds s) {
    unsigned char flags = s[-1];
    switch(flags&SDS_TYPE_MASK) {
        case SDS_TYPE_5: return SDS_TYPE_5_LEN(flags);
        case SDS_TYPE_8: return SDS_HDR(8,s)->len;
        // ...
    }
    return 0;
}

#endif
