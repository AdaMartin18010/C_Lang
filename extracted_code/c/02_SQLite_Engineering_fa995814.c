/*
 * Auto-generated from: 07_Modern_Toolchain\05_Case_Studies\02_SQLite_Engineering.md
 * Line: 388
 * Language: c
 * Block ID: fa995814
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* tokenize.c - SQL 词法分析 */

/* 主分词函数 */
int sqlite3RunParser(Parse *pParse, const char *zSql, char **pzErrMsg) {
    int nErr = 0;
    int i;
    void *pEngine;
    int tokenType;
    int lastTokenParsed = -1;
    sqlite3 *db = pParse->db;
    extern void *sqlite3ParserAlloc(void*(*)(u64));
    extern void sqlite3ParserFree(void*, void(*)(void*));
    extern int sqlite3Parser(void*, int, Token, Parse*);

    db->busyHandler.nBusy = 0;
    if( db->u1.isInterrupted ){
        pParse->rc = SQLITE_INTERRUPT;
        return SQLITE_INTERRUPT;
    }

    pParse->rc = SQLITE_OK;
    pParse->zTail = zSql;
    i = 0;
    pEngine = sqlite3ParserAlloc((void*(*)(u64))sqlite3Malloc);
    if( pEngine==0 ){
        db->mallocFailed = 1;
        return SQLITE_NOMEM_BKPT;
    }

    assert( pParse->pNewTable==0 );
    assert( pParse->pNewTrigger==0 );
    assert( pParse->nVar==0 );
    assert( pParse->nzVar==0 );
    assert( pParse->azVar==0 );

    while( zSql[i]!=0 ){
        pParse->sLastToken.z = &zSql[i];
        pParse->sLastToken.n = sqlite3GetToken((u8*)&zSql[i], &tokenType);
        i += pParse->sLastToken.n;
        if( i>SQLITE_MAX_SQL_LENGTH ){
            pParse->rc = SQLITE_TOOBIG;
            break;
        }

        switch( tokenType ){
            case TK_SPACE: {
                break;
            }
            case TK_COMMENT: {
#ifdef SQLITE_OMIT_EXPLAIN
                if( db->xAuth==0 )
#endif
                db->nProgressOps++;
                break;
            }
            case TK_ILLEGAL: {
                sqlite3ErrorMsg(pParse, "unrecognized token: \"%T\"",
                               &pParse->sLastToken);
                nErr++;
                goto abort_parse;
            }
            case TK_SEMI: {
                pParse->zTail = &zSql[i];
            }
            default: {
                sqlite3Parser(pEngine, tokenType, pParse->sLastToken, pParse);
                lastTokenParsed = tokenType;
                if( pParse->rc!=SQLITE_OK ){
                    goto abort_parse;
                }
                break;
            }
        }
    }

abort_parse:
    if( zSql[i]==0 && nErr==0 && pParse->rc==SQLITE_OK ){
        if( lastTokenParsed!=TK_SEMI ){
            sqlite3Parser(pEngine, TK_SEMI, pParse->sLastToken, pParse);
        }
        sqlite3Parser(pEngine, 0, pParse->sLastToken, pParse);
    }

    sqlite3ParserFree(pEngine, sqlite3_free);
    return nErr;
}
