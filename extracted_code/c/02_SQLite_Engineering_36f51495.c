/*
 * Auto-generated from: 07_Modern_Toolchain\05_Case_Studies\02_SQLite_Engineering.md
 * Line: 319
 * Language: c
 * Block ID: 36f51495
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* Btree 结构 - B-Tree 实例 */
struct Btree {
    sqlite3 *db;              /* 数据库连接 */
    BtShared *pBt;            /* 共享 B-Tree 数据 */
    u8 inTrans;               /* 事务状态 */
    u8 sharable;              /* 是否可共享 */
    int wantToLock;           /* 期望锁计数 */
    int nBackup;              /* 备份数量 */
    Btree *pNext;             /* 链表中下一个 */
    Btree *pPrev;             /* 链表中前一个 */
    BtLock *pLock;            /* 锁定对象 */
};

/* 共享 B-Tree 数据 */
struct BtShared {
    Pager *pPager;            /* 关联的页缓存 */
    sqlite3 *db;              /* 数据库连接 */
    BtCursor *pCursor;        /* 活跃游标链表 */
    MemPage *pPage1;          /* 第一页缓存 */
    i64 nPage;                /* 数据库页数 */
    u32 openFlags;            /* 打开标志 */
    u8 autoVacuum;            /* 自动 vacuum 模式 */
    u8 incrVacuum;            /* 增量 vacuum */
    u8 doNotUseWAL;           /* 禁用 WAL */
    u8 temporarilySecure;     /* 临时安全模式 */
    u8 bDoTruncate;           /* 截断数据库 */
    int nRef;                 /* 引用计数 */
    int nTransaction;         /* 活跃事务数 */
    Bitvec *pHasContent;      /* 有内容的页集合 */
    char *pTmpSpace;          /* 临时空间 */
    int nPreformatSize;       /* 预格式化大小 */
    struct BtShared *pNext;   /* 共享链表 */
    struct BtShared *pPrev;   /* 共享链表 */
#ifdef SQLITE_DEBUG
    u8 pageDump;              /* 调试：页 dump */
#endif
};

/* B-Tree 游标 */
struct BtCursor {
    u8 eState;                /* CURSOR_VALID, CURSOR_INVALID, CURSOR_SKIPNEXT */
    u8 curFlags;              /* 游标标志 */
    u8 curPagerFlags;         /* pager 访问标志 */
    u8 hints;                 /* 性能提示 */
    int iPage;                /* 栈深度 */
    MemPage *apPage[BTCURSOR_MAX_DEPTH];  /* 页栈 */
    u16 aiIdx[BTCURSOR_MAX_DEPTH];        /* 索引栈 */
    Btree *pBtree;            /* 所属 Btree */
    Pgno *aOverflow;          /* 溢出页缓存 */
    CellInfo info;            /* 当前单元格信息 */
    i64 nKey;                 /* 整数键值（INTKEY 表） */
    void *pKey;               /* 打包键值（索引） */
    Pgno pgnoRoot;            /* 根页码 */
    int nOvflAlloc;           /* 分配的溢出页缓存 */
    int skipNext;             /* sqlite3BtreeNext() 返回值 */
    const char *eComment;     /* 调试注释 */
#ifdef SQLITE_DEBUG
    u64 wrFlag;               /* 写标志 */
#endif
};
