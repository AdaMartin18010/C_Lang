/*
 * Auto-generated from: 07_Modern_Toolchain\05_Case_Studies\02_SQLite_Engineering.md
 * Line: 683
 * Language: c
 * Block ID: 0fc082c4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* pager.c - 页缓存和事务管理 */

/* 打开数据库 */
int sqlite3PagerOpen(
    sqlite3_vfs *pVfs,       /* 虚拟文件系统 */
    Pager *pPager,           /* Pager 结构 */
    const char *zFilename,   /* 数据库文件名 */
    int nExtra,              /* 每页额外字节 */
    int flags,               /* 标志 */
    int vfsFlags,            /* VFS 标志 */
    void (*xReinit)(DbPage*) /* 页重初始化函数 */
){
    int rc = SQLITE_OK;

    assert( zFilename!=0 || pVfs->zName==0 );

    /* 分配内存 */
    memset(pPager, 0, sizeof(*pPager));
    pPager->pPCache = (PCache*)sqlite3MallocZero(sizeof(PCache));
    if( pPager->pPCache==0 ){
        return SQLITE_NOMEM_BKPT;
    }

    /* 初始化 Pager */
    pPager->pVfs = pVfs;
    pPager->vfsFlags = vfsFlags;
    pPager->fd = (sqlite3_file*)sqlite3MallocZero(pVfs->szOsFile);
    if( pPager->fd==0 ){
        sqlite3_free(pPager->pPCache);
        return SQLITE_NOMEM_BKPT;
    }

    /* 打开数据库文件 */
    rc = sqlite3OsOpen(pVfs, zFilename, pPager->fd, vfsFlags, &pPager->ctrlFlags);
    if( rc!=SQLITE_OK ){
        sqlite3_free(pPager->fd);
        sqlite3_free(pPager->pPCache);
        return rc;
    }

    /* 初始化页缓存 */
    sqlite3PcacheOpen(pPager->pageSize, nExtra,
                      pPager->mxPgno, pPager->pPCache);

    pPager->eState = PAGER_OPEN;
    pPager->eLock = UNKNOWN_LOCK;

    return SQLITE_OK;
}

/* 开始事务 */
int sqlite3PagerBegin(Pager *pPager, int exFlag, int subjInMemory){
    int rc = SQLITE_OK;

    assert( pPager->eState!=PAGER_ERROR );
    assert( pPager->eState>=PAGER_OPEN );

    if( pPager->eState==PAGER_OPEN ){
        /* 获取共享锁 */
        rc = pagerLockDb(pPager, SHARED_LOCK);
        if( rc!=SQLITE_OK ){
            return rc;
        }

        /* 读取数据库头部 */
        rc = pagerPagecount(pPager, &pPager->dbSize);
        if( rc!=SQLITE_OK ){
            pagerUnlockIfUnused(pPager);
            return rc;
        }

        /* 检查是否需要恢复 */
        rc = pagerPlaybackSavepoint(pPager, 0);
        if( rc==SQLITE_OK ){
            pPager->eState = PAGER_READER;
            pPager->eLock = SHARED_LOCK;
        }
    }

    if( exFlag && pPager->eLock<RESERVED_LOCK ){
        /* 升级到保留锁 */
        rc = pagerLockDb(pPager, RESERVED_LOCK);
        if( rc!=SQLITE_OK ){
            return rc;
        }
        pPager->eLock = RESERVED_LOCK;

        /* 打开日志文件 */
        rc = pagerOpenWal(pPager);
        if( rc!=SQLITE_OK ){
            pagerUnlockIfUnused(pPager);
            return rc;
        }
    }

    return rc;
}

/* 提交事务 */
int sqlite3PagerCommitPhaseOne(
    Pager *pPager,           /* Pager */
    const char *zMaster,     /* 主日志文件名 */
    int noSync               /* 是否跳过同步 */
){
    int rc = SQLITE_OK;

    if( pPager->eState<PAGER_WRITER_CACHEMOD ){
        return SQLITE_OK;
    }

    /* 同步日志文件 */
    if( !noSync ){
        rc = sqlite3WalCheckpoint(pPager->pWal, pPager->fd,
                                  SQLITE_CHECKPOINT_PASSIVE, 0, 0);
    }

    if( rc==SQLITE_OK ){
        /* 刷新页缓存 */
        rc = sqlite3PcacheFlushDirty(pPager->pPCache);
    }

    if( rc==SQLITE_OK ){
        /* 同步数据库文件 */
        if( !noSync && pPager->syncFlags!=SQLITE_SYNC_OFF ){
            rc = sqlite3OsSync(pPager->fd, pPager->syncFlags);
        }
    }

    return rc;
}
