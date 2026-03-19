/*
 * Auto-generated from: 07_Modern_Toolchain\05_Case_Studies\02_SQLite_Engineering.md
 * Line: 822
 * Language: c
 * Block ID: e1361cfb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* wal.c - WAL 实现 */

/* WAL 索引头部 */
struct WalIndexHdr {
    u32 iVersion;           /* WAL 索引格式版本 */
    u32 unused;             /* 未使用 */
    u32 iChange;            /* 计数器，每次事务递增 */
    u8 isInit;              /* 是否已初始化 */
    u8 bigEndCksum;         /* 大端校验和标志 */
    u16 szPage;             /* 页大小 */
    u32 mxFrame;            /* 最大帧号 */
    u32 nPage;              /* 数据库页数 */
    u32 aFrameCksum[2];     /* 最后帧的校验和 */
    u32 aSalt[2];           /* 盐值 */
    u32 aCksum[2];          /* 头部校验和 */
};

/* 开始写事务 */
int sqlite3WalBeginWriteTransaction(Wal *pWal){
    int rc;

    /* 检查是否已有活跃读事务冲突 */
    if( pWal->readLock>=0 ){
        return SQLITE_BUSY_SNAPSHOT;
    }

    /* 获取写锁 */
    rc = walLockExclusive(pWal, WAL_WRITE_LOCK, 1);
    if( rc ){
        return rc;
    }
    pWal->writeLock = 1;

    /* 复制当前索引头部 */
    memcpy(&pWal->hdr, walIndexHdr(pWal), sizeof(WalIndexHdr));

    /* 检查是否需要检查点 */
    if( pWal->hdr.mxFrame>=pWal->mxFrame ){
        rc = SQLITE_BUSY;
    }

    return rc;
}

/* 写入帧 */
int sqlite3WalFrames(
    Wal *pWal,               /* WAL 句柄 */
    int szPage,              /* 页大小 */
    PgHdr *pList,            /* 脏页列表 */
    Pgno nTruncate,          /* 截断后的页数 */
    int isCommit,            /* 是否是提交 */
    int syncFlags            /* 同步标志 */
){
    int rc;
    PgHdr *p;
    Pgno nLast = 0;
    int nExtra;

    /* 计算需要写入的帧数 */
    int nFrames = 0;
    for(p=pList; p; p=p->pDirty){
        nFrames++;
    }

    /* 为帧分配空间 */
    rc = walIndexAppend(pWal, nFrames);
    if( rc ) return rc;

    /* 写日志文件 */
    for(p=pList; p; p=p->pDirty){
        u32 iFrame;
        void *pData;

        pData = p->pData;
        iFrame = pWal->hdr.mxFrame + 1;

        /* 写帧头部 */
        WalFrameHdr hdr;
        hdr.pgno = p->pgno;
        hdr.nTruncate = isCommit && p->pDirty==0 ? nTruncate : 0;
        hdr.aCksum[0] = pWal->hdr.aSalt[0];
        hdr.aCksum[1] = pWal->hdr.aSalt[1];

        /* 计算校验和 */
        walChecksumBytes(pWal->hdr.bigEndCksum,
                         (u8*)&hdr, sizeof(hdr),
                         pData, szPage, hdr.aCksum);

        /* 写入帧 */
        rc = sqlite3OsWrite(pWal->pWalFd, &hdr, sizeof(hdr),
                            walFrameOffset(iFrame, szPage) - WAL_FRAME_HDRSIZE);
        if( rc ) return rc;

        rc = sqlite3OsWrite(pWal->pWalFd, pData, szPage,
                            walFrameOffset(iFrame, szPage));
        if( rc ) return rc;

        /* 更新索引 */
        walIndexWrite(pWal, p->pgno, iFrame);
        pWal->hdr.mxFrame = iFrame;
        nLast = p->pgno;
    }

    /* 更新索引头部 */
    if( isCommit ){
        pWal->hdr.nPage = nTruncate;
        pWal->hdr.iChange++;
        walIndexWriteHdr(pWal);

        /* 同步 WAL 文件 */
        if( syncFlags ){
            sqlite3OsSync(pWal->pWalFd, syncFlags);
        }
    }

    return SQLITE_OK;
}

/* 读取页 */
int sqlite3WalRead(
    Wal *pWal,               /* WAL 句柄 */
    Pgno pgno,               /* 页号 */
    int *pReadLock,          /* 读锁级别 */
    u8 noContent,            /* 是否不需要内容 */
    void *pBuf               /* 输出缓冲区 */
){
    u32 iRead = 0;
    int rc;

    /* 在 WAL 索引中查找 */
    if( pReadLock ){
        iRead = walFramePgno(pWal, *pReadLock, pgno);
    }

    /* 如果 WAL 中没有，从数据库读取 */
    if( iRead==0 ){
        i64 iOffset = (pgno-1) * (i64)pWal->szPage;
        rc = sqlite3OsRead(pWal->pDbFd, pBuf, pWal->szPage, iOffset);
        return rc;
    }

    /* 从 WAL 读取 */
    i64 iOffset = walFrameOffset(iRead, pWal->szPage);
    rc = sqlite3OsRead(pWal->pWalFd, pBuf, pWal->szPage, iOffset);

    return rc;
}
