/*
 * Auto-generated from: 07_Modern_Toolchain\05_Case_Studies\02_SQLite_Engineering.md
 * Line: 1058
 * Language: c
 * Block ID: e101962d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 模糊测试工具 - tool/fuzzers */

/* SQL 模糊测试 */
int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    sqlite3 *db;
    int rc;
    char *zErr = 0;
    char *zSql;

    /* 限制输入大小 */
    if( size>10000 ) return 0;

    /* 打开内存数据库 */
    rc = sqlite3_open(":memory:", &db);
    if( rc ) return 0;

    /* 转换为 SQL 字符串 */
    zSql = sqlite3_mprintf("%.*s", (int)size, data);

    /* 准备语句 */
    sqlite3_stmt *pStmt;
    rc = sqlite3_prepare_v2(db, zSql, -1, &pStmt, 0);

    if( rc==SQLITE_OK && pStmt ){
        /* 尝试执行 */
        int nRow = 0;
        while( sqlite3_step(pStmt)==SQLITE_ROW ){
            nRow++;
            if( nRow>100 ) break;  /* 限制结果集 */
        }
        sqlite3_finalize(pStmt);
    }

    sqlite3_free(zSql);
    sqlite3_close(db);

    return 0;
}

/* 数据库文件模糊测试 */
int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    char *zFilename;
    sqlite3 *db;
    sqlite3_stmt *pStmt;
    int rc;

    /* 创建临时文件 */
    zFilename = tempnam(0, "fuzz");

    FILE *f = fopen(zFilename, "wb");
    fwrite(data, 1, size, f);
    fclose(f);

    /* 尝试打开 */
    rc = sqlite3_open(zFilename, &db);
    if( rc==SQLITE_OK ){
        /* 执行 PRAGMA integrity_check */
        rc = sqlite3_prepare_v2(db, "PRAGMA integrity_check", -1, &pStmt, 0);
        if( rc==SQLITE_OK ){
            while( sqlite3_step(pStmt)==SQLITE_ROW ){}
            sqlite3_finalize(pStmt);
        }

        /* 尝试查询 sqlite_schema */
        rc = sqlite3_prepare_v2(db, "SELECT * FROM sqlite_schema", -1, &pStmt, 0);
        if( rc==SQLITE_OK ){
            int nRow = 0;
            while( sqlite3_step(pStmt)==SQLITE_ROW ){
                nRow++;
                if( nRow>50 ) break;
            }
            sqlite3_finalize(pStmt);
        }

        sqlite3_close(db);
    }

    unlink(zFilename);
    free(zFilename);

    return 0;
}
