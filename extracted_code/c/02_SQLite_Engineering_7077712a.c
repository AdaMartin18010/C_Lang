/*
 * Auto-generated from: 07_Modern_Toolchain\05_Case_Studies\02_SQLite_Engineering.md
 * Line: 227
 * Language: c
 * Block ID: 7077712a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* Parse 结构 - 整个解析过程的上下文 */
struct Parse {
    sqlite3 *db;              /* 数据库连接 */
    char *zErrMsg;            /* 错误消息 */
    Vdbe *pVdbe;              /* 正在构建的虚拟机程序 */
    int rc;                   /* 返回代码 */
    u8 colNamesSet;           /* 列名已设置标志 */
    u8 checkSchema;           /* 检查模式锁定 */
    u8 nested;                /* 嵌套调用标志 */
    u8 nTempReg;              /* 临时寄存器数量 */
    int aTempReg[8];          /* 临时寄存器池 */
    int nRangeReg;            /* 范围寄存器数量 */
    int iRangeReg;            /* 范围寄存器基址 */
    int nErr;                 /* 错误数量 */
    int nTab;                 /* 生成的表游标数量 */
    int nMem;                 /* 使用的内存单元数量 */
    int nSet;                 /* 使用的集合数量 */
    int nOnce;                /* 使用的 OP_Once 指令 */
    int ckBase;               /* CHECK 约束起始寄存器 */
    int iSelfTab;             /* 自引用列的表索引 */
    int iCacheLevel;          /* 缓存有效级别 */
    int iCacheCnt;            /* 缓存计数器 */
    struct yColCache {        /* 列缓存 */
        int iTable;
        i16 iColumn;
        u8 tempReg;
        int iLevel;
        int iReg;
        u8 affChange;
    } aColCache[SQLITE_N_COLCACHE];
    ExprList *pConstExpr;     /* 常量表达式列表 */
    int cookieValue[SQLITE_MAX_ATTACHED+2];  /* 模式 cookie */
    int regRowid;             /* 行 ID 寄存器 */
    int regRoot;              /* 根页码寄存器 */
    int nMaxArg;              /* 最大函数参数数量 */
    int nLabel;               /* 标签数量 */
    int *aLabel;              /* 标签地址数组 */
    ExprList *pParenAgg;      /* 父聚合表达式 */
    AuthContext *pAuthContext;/* 授权上下文 */
    int nHeight;              /* 表达式树高度 */
    int iSelectId;            /* SELECT ID 计数器 */
    int iNextSelectId;        /* 下一个 SELECT ID */
    Select *pNewTable;        /* 新建表定义 */
    Table *pTriggerTab;       /* 触发器目标表 */
    u32 oldmask;              /* 旧列访问掩码 */
    u32 newmask;              /* 新列访问掩码 */
    u8 eTriggerOp;            /* 触发器操作类型 */
    u8 eOrconf;               /* 默认 ON CONFLICT 策略 */
    u8 disableTriggers;       /* 禁用触发器标志 */
    int nQueryLoop;           /* 估计的循环次数 */
    double nQueryLoop;        /* 查询循环次数估计 */
    u32 writeMask;            /* 写操作锁定掩码 */
    u32 cookieMask;           /* 需要验证的 cookie */
    int regLockProof;         /* 锁定证明寄存器 */
    int orignCol;             /* 原始列号 */
    int nVar;                 /* 变量数量 */
    int nzVar;                /* 变量名数组大小 */
    u8 explain;               /* 当前处于 EXPLAIN 模式 */
    u8 eParseMode;            /* 解析模式 */
    const char *zAuthContext; /* 授权上下文字符串 */
#ifndef SQLITE_OMIT_VIRTUALTABLE
    Token sArg;               /* 虚拟表参数 */
    VTable *pVtabLock;        /* 需要锁定的虚拟表 */
#endif
    int nAlias;               /* 别名数组大小 */
    int iAlias;               /* 当前别名索引 */
    int *aAlias;              /* 别名寄存器数组 */
};

/* SELECT 语句结构 */
struct Select {
    ExprList *pEList;         /* SELECT 结果列 */
    u8 op;                    /* SELECT, UNION, EXCEPT, INTERSECT */
    LogEst nSelectRow;        /* 估计返回行数 */
    u32 selFlags;             /* 各种标志位 */
    int iLimit, iOffset;      /* LIMIT/OFFSET 寄存器地址 */
    With *pWith;              /* WITH 子句 */
    Select *pPrior;           /* 前一个 SELECT（复合查询） */
    Select *pNext;            /* 下一个 SELECT */
    Select *pLeftmost;        /* 最左侧 SELECT */
    SrcList *pSrc;            /* FROM 子句 */
    Expr *pWhere;             /* WHERE 子句 */
    ExprList *pGroupBy;       /* GROUP BY 子句 */
    Expr *pHaving;            /* HAVING 子句 */
    ExprList *pOrderBy;       /* ORDER BY 子句 */
    SelectDest dest;          /* 结果处理方式 */
};
