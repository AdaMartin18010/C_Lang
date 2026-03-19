/*
 * Auto-generated from: 07_Modern_Toolchain\05_Case_Studies\01_Redis_Architecture.md
 * Line: 440
 * Language: c
 * Block ID: 5952ca17
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* quicklist.c - 快速列表（ziplist + linked list 混合）*/

/* 快速列表节点 */
typedef struct quicklistNode {
    struct quicklistNode *prev;
    struct quicklistNode *next;
    unsigned char *zl;           // 指向 ziplist
    unsigned int sz;             // ziplist 大小（字节）
    unsigned int count : 16;     // 元素数量
    unsigned int encoding : 2;   // RAW==1 or LZF==2
    unsigned int container : 2;  // NONE==1 or ZIPLIST==2
    unsigned int recompress : 1; // 临时解压标记
    unsigned int attempted_compress : 1;
    unsigned int extra : 10;
} quicklistNode;

/* 快速列表 */
typedef struct quicklist {
    quicklistNode *head;
    quicklistNode *tail;
    unsigned long count;         // 总元素数
    unsigned long len;           // 节点数
    int fill : QL_FILL_BITS;     // 填充因子
    unsigned int compress : QL_COMP_BITS;  // 压缩深度
} quicklist;

/* 内存效率优化：
 * 1. 小列表使用 ziplist 连续存储
 * 2. 大列表分裂为多个 ziplist 节点
 * 3. 中间节点使用 LZF 压缩
 */

// 创建快速列表
quicklist *quicklistCreate(void) {
    struct quicklist *quicklist = zmalloc(sizeof(*quicklist));
    quicklist->head = quicklist->tail = NULL;
    quicklist->len = 0;
    quicklist->count = 0;
    quicklist->compress = 0;
    quicklist->fill = -2;  // 默认每个 ziplist 8KB
    return quicklist;
}

// 在头部插入元素
int quicklistPushHead(quicklist *quicklist, void *value, const size_t sz) {
    quicklistNode *orig_head = quicklist->head;

    if (likely(orig_head && orig_head->encoding == QUICKLIST_NODE_ENCODING_RAW &&
               quicklistNodeAllowInsert(orig_head, sz))) {
        // 现有节点可以容纳
        quicklist->head->zl = ziplistPush(quicklist->head->zl, value, sz, ZIPLIST_HEAD);
        quicklistNodeUpdateSz(quicklist->head);
    } else {
        // 创建新节点
        quicklistNode *node = quicklistCreateNode();
        node->zl = ziplistPush(ziplistNew(), value, sz, ZIPLIST_HEAD);
        quicklistNodeUpdateSz(node);
        quicklistLinkNodeHead(quicklist, node);
    }

    quicklist->count++;
    return 1;
}
