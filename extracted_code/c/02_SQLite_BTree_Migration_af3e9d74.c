/*
 * Auto-generated from: 08_Zig_C_Connection\07_Real_World_Migration_Cases\02_SQLite_BTree_Migration.md
 * Line: 49
 * Language: c
 * Block ID: af3e9d74
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// sqlite3_btree.h (简化)
typedef struct Btree Btree;
typedef struct BtCursor BtCursor;
typedef struct BtShared BtShared;

struct Btree {
    sqlite3 *db;
    BtShared *pBt;
    u8 inTrans;
    u8 sharable;
};

struct BtShared {
    Pager *pPager;
    sqlite3 *db;
    BtCursor *pCursor;
    u8 *pTmpSpace;
    int pageSize;
    int usableSize;
    int nPage;
    int maxLocal;
    int minLocal;
    int maxLeaf;
    int minLeaf;
};

struct BtCursor {
    Btree *pBtree;
    BtShared *pBt;
    BtCursor *pNext;
    BtCursor *pPrev;
    struct KeyInfo *pKeyInfo;
    Pgno pgnoRoot;
    CellInfo info;
    u8 curIntKey;
    u8 curFlags;
    u16 aiIdx[BTCURSOR_MAX_DEPTH];
    struct MemPage *pPage;
};
