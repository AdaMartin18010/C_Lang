/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\06_Malloc_Lab.md
 * Line: 110
 * Language: c
 * Block ID: 52b66afb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// mm.c
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/* 基本常量 */
#define WSIZE       4       // 字大小（字节）
#define DSIZE       8       // 双字大小（字节）
#define CHUNKSIZE  (1<<12)  // 扩展堆的默认大小（4096字节）
#define ALIGNMENT   8       // 对齐要求

#define MAX(x, y) ((x) > (y) ? (x) : (y))

/* 将大小和分配位打包到字中 */
#define PACK(size, alloc)  ((size) | (alloc))

/* 读写一个字 */
#define GET(p)       (*(unsigned int *)(p))
#define PUT(p, val)  (*(unsigned int *)(p) = (val))

/* 从地址p读取大小和分配位 */
#define GET_SIZE(p)   (GET(p) & ~0x7)
#define GET_ALLOC(p)  (GET(p) & 0x1)

/* 给定块指针bp，计算头部和脚部的地址 */
#define HDRP(bp)       ((char *)(bp) - WSIZE)
#define FTRP(bp)       ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)

/* 给定块指针bp，计算下一个和前一个块的地址 */
#define NEXT_BLKP(bp)  ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))
#define PREV_BLKP(bp)  ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))

/* 全局变量 */
static char *heap_listp = 0;  // 指向序言块
