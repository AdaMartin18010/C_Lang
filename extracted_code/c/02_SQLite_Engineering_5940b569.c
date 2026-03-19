/*
 * Auto-generated from: 07_Modern_Toolchain\05_Case_Studies\02_SQLite_Engineering.md
 * Line: 557
 * Language: c
 * Block ID: 5940b569
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* B-Tree 核心操作 */

/* 打开 B-Tree 游标 */
int sqlite3BtreeCursor(
    Btree *p,              /* Btree */
    int iTable,            /* 根页码 */
    int wrFlag,            /* 写标志 */
    struct KeyInfo *pKeyInfo,  /* 键信息 */
    BtCursor *pCur         /* 游标 */
){
    BtShared *pBt = p->pBt;
    BtCursor *pX;
    int rc = SQLITE_OK;

    assert( sqlite3BtreeHoldsMutex(p) );
    assert( wrFlag==0 || wrFlag==1 );

    /* 检查数据库是否已损坏 */
    if( pBt->db->mallocFailed ){
        return SQLITE_NOMEM_BKPT;
    }

    if( wrFlag ){
        /* 写游标需要独占锁 */
        if( pBt->readOnly ){
            return SQLITE_READONLY;
        }
        if( checkReadLocks(p, iTable, 0, 0) ){
            return SQLITE_LOCKED_SHAREDCACHE;
        }
    }

    /* 初始化游标 */
    pCur->pgnoRoot = (Pgno)iTable;
    pCur->iPage = -1;
    pCur->pKeyInfo = pKeyInfo;
    pCur->pBtree = p;
    pCur->pBt = pBt;
    pCur->curFlags = wrFlag ? BTCF_WriteFlag : 0;
    pCur->curPagerFlags = wrFlag ? 0 : PAGER_GET_READONLY;
    pCur->hints = 0;
    memset(&pCur->info, 0, sizeof(pCur->info));
    pCur->iPage = -1;

    /* 将游标加入活跃链表 */
    pCur->pNext = pBt->pCursor;
    if( pCur->pNext ){
        pCur->pNext->pPrev = pCur;
    }
    pBt->pCursor = pCur;
    pCur->eState = CURSOR_INVALID;

    return SQLITE_OK;
}

/* 插入记录 */
int sqlite3BtreeInsert(
    BtCursor *pCur,        /* 插入游标 */
    const void *pKey,      /* 键 */
    i64 nKey,              /* 键长度 */
    const void *pData,     /* 数据 */
    int nData,             /* 数据长度 */
    int nZero,             /* 末尾零字节数 */
    int appendBias,        /* 是否追加优化 */
    int seekResult         /* 搜索结果 */
){
    int rc;
    int loc = seekResult;
    int szNew;
    MemPage *pPage;
    BtShared *pBt;
    unsigned char *oldCell;
    unsigned char *newCell = 0;

    assert( cursorHoldsMutex(pCur) );
    assert( pCur->eState==CURSOR_VALID );
    assert( pCur->iPage>=0 );
    assert( pCur->iPage<BTCURSOR_MAX_DEPTH );
    assert( pCur->wrFlag );

    pBt = pCur->pBt;
    pPage = pCur->apPage[pCur->iPage];

    /* 计算新单元格大小 */
    szNew = cellSizePtr(pPage, pKey, nKey, pData, nData);
    if( szNew>MX_CELL_SIZE(pBt) ){
        return SQLITE_TOOBIG;
    }

    /* 分配新单元格内存 */
    newCell = pBt->pTmpSpace;
    if( newCell==0 ){
        return SQLITE_NOMEM;
    }

    /* 组装单元格 */
    rc = fillInCell(pPage, newCell, pKey, nKey, pData, nData, nZero, &szNew);
    if( rc ) return rc;

    /* 检查是否有足够空间 */
    if( loc==0 ){
        u32 szOld;
        assert( pCur->info.nSize==0 );
        btreeParseCell(pCur, &pCur->info);
        szOld = pCur->info.nSize;
        if( szOld<4 ) szOld = pPage->minLocal;
        if( szNew<=szOld ){
            /* 原地替换 */
        }else if( locateCell(pPage, pCur->aiIdx[pCur->iPage])>2 ){
            /* 移动到更靠前的位置 */
        }
    }

    /* 插入单元格 */
    rc = insertCell(pPage, pCur->aiIdx[pCur->iPage], newCell, szNew, 0, 0);
    if( rc==SQLITE_OK ){
        rc = balance(pCur, 1);
    }

    return rc;
}
