---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../../03_System_Technology_Domains/14_Concurrency_Parallelism/readme.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../../01_Core_Knowledge_System/05_Engineering_Layer/readme.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../../02_Formal_Semantics_and_Physics/readme.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../../03_System_Technology_Domains/readme.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../../04_Industrial_Scenarios/readme.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../../06_Thinking_Representation/readme.md) | 核心关联 | 思维表征基础 |
# SQLite 架构与核心机制源码解读

> **源码版本**: SQLite 3.40+
> **核心文件**: src/pager.c, src/btree.c, src/vdbe.c
> **学习重点**: B-Tree实现、虚拟数据库引擎、事务管理

---

## 1. 整体架构

### 1.1 模块层次

```
SQL Interface
      ↓
Parser (分词、语法分析)
      ↓
Code Generator → Virtual Machine Code (字节码)
      ↓
Virtual Database Engine (VDBE)
      ↓
B-Tree / Pager / OS Interface
      ↓
File System / Memory
```

### 1.2 核心数据结构

```c
// sqliteInt.h

typedef struct sqlite3 {
    // 连接级状态
    int openFlags;              // 打开标志
    int errCode;                // 最近错误码
    int nDb;                    // 数据库数量
    Db* aDb;                    // 数据库数组

    // 虚拟机
    int nVdbeActive;            // 活跃VDBE数量
    int nVdbeRead;              // 读操作计数
    int nVdbeWrite;             // 写操作计数

    // 事务
    int autoCommit;             // 自动提交模式
    int nDeferredCons;          // 延迟约束数量
} sqlite3;

typedef struct Db {
    char* zName;                // 数据库名 (main, temp, 附加)
    Btree* pBt;                 // B-Tree句柄
    u8 safety_level;            // 同步级别
    Schema* pSchema;            // 模式
} Db;
```

---

## 2. B-Tree实现

### 2.1 B-Tree vs B+Tree

SQLite使用**B+Tree**:

- 所有数据在叶子节点
- 内部节点只存储键和指针
- 叶子节点形成双向链表

```
内部节点: [key1, ptr1, key2, ptr2, key3, ptr3]
叶子节点: [key1, data1, key2, data2, ...] ← 实际数据
           ↑                              ↑
         双向链表连接
```

### 2.2 页面格式

```c
// btreeInt.h

// 数据库页面头部 (100字节，位于第1页)
#define SQLITE_HEADER_SIZE 100

// B-Tree页面头部 (偏移100开始)
struct BtreePageHdr {
    u8 flags;           // 1=叶子页, 2=内部页, 4=溢出页
    u16 nCell;          // 单元数量
    u16 cellOffset;     // 单元内容起始偏移
    u8 nFragment;       // 碎片字节数
    u32 rightChild;     // 最右子页 (仅内部页)
};

// 单元指针数组 (从页面末尾向头部增长)
// [cell pointer 0][cell pointer 1]...[cell pointer N]
//                        ↓
//              [cell content area]
```

### 2.3 单元格式

```
变长整数 (Varint):
0xxxxxxx                    - 7位值，1字节
1xxxxxxx 0xxxxxxx           - 14位值，2字节
1xxxxxxx 1xxxxxxx 0xxxxxxx  - 21位值，3字节
...

Payload格式 (叶子页单元):
+----------------+----------------+----------------+----------------+
| payload size   | rowid          | header size    | type [0]       |
| (varint)       | (varint)       | (varint)       | (varint)       |
+----------------+----------------+----------------+----------------+
| type [1]       | ...            | data offset    | data content   |
| (varint)       |                |                |                |
+----------------+----------------+----------------+----------------+

类型编码:
0    → NULL
1..6 → 1..6字节整数
7    → IEEE浮点
8..9 → 整数0/1 (优化)
10+even → BLOB，长度 = (N-12)/2
11+odd  → TEXT，长度 = (N-13)/2
```

### 2.4 关键操作

```c
// 搜索B-Tree
int sqlite3BtreeMovetoUnpacked(
    BtCursor* pCur,      // 游标
    UnpackedRecord* pIdxKey,  // 键
    i64 intKey,          // 整数键 (表)
    int biasRight,       // 偏向
    int* pRes            // 结果: -1=小于, 0=等于, 1=大于
) {
    // 从根页开始
    Pgno pgno = pCur->pgnoRoot;

    while (1) {
        MemPage* pPage = pCur->apPage[pCur->iPage];

        // 二分查找单元
        int upr = pPage->nCell - 1;
        int lwr = 0;

        while (lwr <= upr) {
            int idx = (lwr + upr) / 2;
            CellInfo info;

            // 解析单元
            btreeParseCell(pPage, idx, &info);

            // 比较键
            int c = sqlite3VdbeRecordCompare(...);

            if (c < 0) upr = idx - 1;
            else if (c > 0) lwr = idx + 1;
            else {
                // 找到
                pCur->ix = idx;
                *pRes = 0;
                return SQLITE_OK;
            }
        }

        // 判断是否为叶子页
        if (pPage->leaf) {
            pCur->ix = lwr;
            *pRes = 1;
            return SQLITE_OK;
        }

        // 移动到子页
        pgno = getChildPage(pPage, lwr);
        moveToChild(pCur, pgno);
    }
}
```

---

## 3. Pager (页面管理)

### 3.1 页面缓存

```c
// pager.c

typedef struct Pager {
    sqlite3_vfs* pVfs;          // 虚拟文件系统
    char* zFilename;            // 数据库文件名
    char* zJournal;             // 日志文件名

    // 缓存
    int nPage;                  // 缓存页面数
    PgHdr* pAll;                // 所有页面链表
    PgHdr* pDirty;              // 脏页链表
    PgHdr* pAllRead;            // 只读页链表

    // 状态
    u8 journalMode;             // 日志模式
    u8 eState;                  // Pager状态
    u8 eLock;                   // 锁状态

    // 事务
    Pgno dbSize;                // 数据库大小 (页面数)
    Pgno dbOrigSize;            // 事务开始时的大小
    Pgno dbFileSize;            // 文件实际大小
} Pager;

// 页面头部
struct PgHdr {
    void* pData;                // 页面数据
    void* pExtra;               // 额外数据 (Btree使用)
    PgHdr* pDirty;              // 脏页链表指针
    Pager* pPager;              // 所属Pager
    Pgno pgno;                  // 页号
    u16 flags;                  // 标志
    int nRef;                   // 引用计数
};
```

### 3.2 事务与ACID

```c
// 事务状态
#define PAGER_OPEN        0   // 打开，无事务
#define PAGER_READER      1   // 读事务
#define PAGER_WRITER_LOCKED 2  // 写锁获取
#define PAGER_WRITER_CACHEMOD 3 // 修改缓存
#define PAGER_WRITER_DBMOD 4    // 修改数据库
#define PAGER_WRITER_FINISHED 5 // 完成
#define PAGER_ERROR       6     // 错误

// BEGIN TRANSACTION流程:
// 1. 获取共享锁 (RESERVED)
// 2. 读取页面到缓存

// COMMIT流程:
int sqlite3PagerCommitPhaseOne(Pager* pPager, const char* zMaster) {
    // 1. 将脏页写入日志 (WAL模式)
    // 或写入回滚日志 (DELETE模式)

    // 2. 同步日志到磁盘
    sqlite3OsSync(pPager->jfd, pPager->syncFlags);

    // 3. 将脏页写入数据库文件
    pager_write_pagelist(pPager, pPager->pDirty);

    // 4. 同步数据库文件
    sqlite3OsSync(pPager->fd, pPager->syncFlags);

    return SQLITE_OK;
}

int sqlite3PagerCommitPhaseTwo(Pager* pPager) {
    // 1. 删除日志文件 (或截断)

    // 2. 释放锁
    pager_unlock(pPager);

    return SQLITE_OK;
}
```

### 3.3 WAL (Write-Ahead Logging)

```
传统模式 (DELETE):          WAL模式:
1. 写回滚日志               1. 追加到WAL文件
2. 修改数据库页             2. 可选: checkpoint
3. 同步日志
4. 同步数据库               WAL文件格式:
5. 删除日志                 +----------------+
                           | WAL header     |
                           +----------------+
                           | Frame 1 header |
                           | Frame 1 data   |
                           +----------------+
                           | Frame 2 header |
                           | Frame 2 data   |
                           +----------------+
```

---

## 4. VDBE (虚拟数据库引擎)

### 4.1 字节码结构

```c
// vdbe.h

typedef struct VdbeOp {
    u8 opcode;          // 操作码
    int p1;             // 第1参数
    int p2;             // 第2参数 (跳转目标)
    int p3;             // 第3参数
    union p4union {     // 第4参数 (变长)
        int i;
        void* p;
        double r;
        char* z;
        i64* pI64;
        // ...
    } p4;
    u8 p5;              // 第5参数 (标志)
#ifdef SQLITE_DEBUG
    char* zComment;     // 调试注释
#endif
} VdbeOp;

// 常用操作码
#define OP_Init        0   // 初始化
#define OP_OpenRead    1   // 打开读游标
#define OP_OpenWrite   2   // 打开写游标
#define OP_Rewind      3   // 游标重置
#define OP_Column      4   // 读取列
#define OP_ResultRow   5   // 返回结果行
#define OP_Next        6   // 下一行
#define OP_Halt        7   // 停止
#define OP_Integer     8   // 整数常量
#define OP_String      9   // 字符串常量
#define OP_Eq          10  // 等于
#define OP_Gt          11  // 大于
// ... 150+ 操作码
```

### 4.2 执行循环

```c
// vdbe.c

int sqlite3VdbeExec(Vdbe* p) {
    Op* aOp = p->aOp;           // 操作码数组
    Op* pOp;                     // 当前操作

    // 主执行循环
    for (pOp = &aOp[p->pc]; 1; pOp = &aOp[++p->pc]) {

        switch (pOp->opcode) {

            case OP_Init: {
                // 初始化
                p->pc = pOp->p2 - 1;  // 跳转到p2
                break;
            }

            case OP_OpenRead: {
                // 打开只读游标
                int i = pOp->p1;        // 游标编号
                int p2 = pOp->p2;       // 根页号
                int p3 = pOp->p3;       // 数据库编号

                Btree* pX = p->db->aDb[p3].pBt;
                BtCursor* pCur = allocateCursor(p, i, ...);

                sqlite3BtreeCursor(pX, p2, BTREE_WRCSR, ...);
                break;
            }

            case OP_Column: {
                // 读取当前行的一列
                BtCursor* pC = p->apCsr[pOp->p1];
                u32 column = pOp->p2;

                // 解析记录，提取第column列
                sqlite3VdbeColumn(pC, column);
                break;
            }

            case OP_Next: {
                // 移动到下一行
                BtCursor* pC = p->apCsr[pOp->p1];

                int res = sqlite3BtreeNext(pC, 0);
                if (res == 0) {
                    p->pc = pOp->p2 - 1;  // 跳回循环开始
                }
                break;
            }

            case OP_ResultRow: {
                // 返回结果行给调用者
                p->pc++;
                return SQLITE_ROW;
            }

            case OP_Halt: {
                // 停止执行
                return pOp->p1 ? SQLITE_ERROR : SQLITE_DONE;
            }

            // ... 更多操作码处理
        }
    }
}
```

### 4.3 字节码示例

```sql
-- SQL
SELECT name FROM users WHERE age > 25;

-- 生成的字节码 (简化)
0:  Init        0    11    0               // 跳转到11
1:  OpenRead    0     2    0 users        // 打开users表
2:  Rewind      0    10    0               // 重置到开始
3:  Column      0     2    1               // 读取age列到reg[1]
4:  Integer    25     2    0               // reg[2] = 25
5:  Gt          1     2    9   if reg[1]>reg[2] goto 9
6:  Column      0     1    3               // 读取name列到reg[3]
7:  ResultRow   3     1    0               // 返回reg[3]
8:  Next        0     3    0               // 下一行，跳转到3
9:  Close       0     0    0               // 关闭游标
10: Halt        0     0    0               // 结束
```

---

## 5. 学习要点

### 5.1 数据库设计思想

```
零配置:
- 单文件存储
- 无服务器进程
- 自动类型转换

B+Tree选择:
- 范围查询高效
- 顺序访问优化
- 更好的缓存局部性

页面抽象:
- 统一I/O单位
- 便于缓存管理
- 支持不同存储后端
```

### 5.2 代码质量

| 特点 | 说明 |
|:-----|:-----|
| 测试覆盖 | 超过95%的代码覆盖率 |
| 代码生成 | 工具自动生成部分代码 |
| 内存安全 | 严格的引用计数 |
| 平台兼容 | 通过VFS抽象文件系统 |

---

**最后更新**: 2026-03-24
