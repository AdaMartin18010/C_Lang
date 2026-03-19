/*
 * Auto-generated from: 08_Zig_C_Connection\07_Real_World_Migration_Cases\01_Redis_Data_Structure_Migration.md
 * Line: 105
 * Language: c
 * Block ID: b4170187
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// sds.c - 简化实现
#include "sds.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static inline int sdsHdrSize(char type) {
    switch(type&SDS_TYPE_MASK) {
        case SDS_TYPE_5: return sizeof(struct sdshdr5);
        case SDS_TYPE_8: return sizeof(struct sdshdr8);
        // ...
    }
    return 0;
}

sds sdsnewlen(const void* init, size_t initlen) {
    void* sh;
    sds s;
    char type = sdsReqType(initlen);
    int hdrlen = sdsHdrSize(type);

    sh = malloc(hdrlen + initlen + 1);
    if (sh == NULL) return NULL;

    // 初始化头部...
    s = (char*)sh + hdrlen;
    if (initlen && init) memcpy(s, init, initlen);
    s[initlen] = '\0';
    return s;
}

void sdsfree(sds s) {
    if (s == NULL) return;
    free((char*)s - sdsHdrSize(s[-1]));
}

sds sdscatlen(sds s, const void* t, size_t len) {
    size_t curlen = sdslen(s);
    if (sdsavail(s) < len) {
        s = sdsMakeRoomFor(s, len);
        if (s == NULL) return NULL;
    }
    memcpy(s+curlen, t, len);
    sdssetlen(s, curlen+len);
    s[curlen+len] = '\0';
    return s;
}
