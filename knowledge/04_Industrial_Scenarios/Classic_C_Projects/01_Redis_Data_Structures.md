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
# Redis 核心数据结构源码解读

> **源码版本**: Redis 7.x
> **核心文件**: src/sds.c, src/dict.c, src/ziplist.c, src/quicklist.c
> **学习重点**: 紧凑内存布局、渐进式rehash、跳表实现

---

## 1. SDS (Simple Dynamic String)

### 1.1 设计目标

- 二进制安全 (可包含\0)
- 常数时间获取长度
- 避免缓冲区溢出
- 预分配减少内存分配次数

### 1.2 内存布局

```c
// sds.h
// SDS有5种header类型，根据字符串长度选择

// sdshdr8 示例 (字符串长度 < 256)
struct __attribute__ ((__packed__)) sdshdr8 {
    uint8_t len;        // 已使用长度
    uint8_t alloc;      // 总分配长度 (不含header和\0)
    unsigned char flags;// 类型标识 (低3位)
    char buf[];         // 柔性数组
};

// 内存布局:
// +--------+--------+--------+--------+--------+--------+--------+
// | len    | alloc  | flags  | 'h'    | 'e'    | 'l'    | 'l'    |
// | 4      | 10     | 0x01   |        |        |        |        |
// +--------+--------+--------+--------+--------+--------+--------+
//   ↑ header start                          ↑ buf指针指向这里
//
// 后续: | 'o' | '\0' | 空闲空间... |
```

### 1.3 关键代码

```c
// 创建SDS
sds sdsnewlen(const void* init, size_t initlen) {
    void* sh;
    sds s;
    char type = sdsReqType(initlen);  // 根据长度选择header类型

    int hdrlen = sdsHdrSize(type);
    unsigned char* fp;  // flags pointer

    // 分配: header + 数据 + \0
    sh = s_malloc(hdrlen + initlen + 1);

    // ... 初始化header ...

    s = (char*)sh + hdrlen;  // s指向buf开始
    fp = ((unsigned char*)s) - 1;  // flags位置
    *fp = type;

    if (initlen && init)
        memcpy(s, init, initlen);

    s[initlen] = '\0';  // C字符串兼容
    return s;
}

// 获取长度 - O(1)
static inline size_t sdslen(const sds s) {
    unsigned char flags = s[-1];
    switch(flags & SDS_TYPE_MASK) {
        case SDS_TYPE_5: return SDS_TYPE_5_LEN(flags);
        case SDS_TYPE_8: return ((sdshdr8*)s - 1)->len;
        // ... 其他类型
    }
    return 0;
}

// 扩容
sds sdscatlen(sds s, const void* t, size_t len) {
    size_t curlen = sdslen(s);

    // 确保空间足够
    s = sdsMakeRoomFor(s, len);

    // 追加数据
    memcpy(s + curlen, t, len);

    // 更新长度
    sdssetlen(s, curlen + len);
    s[curlen + len] = '\0';

    return s;
}
```

### 1.4 优点分析

| 特性 | C字符串 | SDS |
|:-----|:--------|:----|
| 获取长度 | O(n) | O(1) |
| 缓冲区溢出 | 容易 | 不可能 |
| 二进制安全 | 否 | 是 |
| 内存预分配 | 无 | 有 |

---

## 2. Dict (Hash Table)

### 2.1 双哈希表设计

```c
// dict.h
typedef struct dictht {
    dictEntry** table;      // 桶数组
    unsigned long size;     // 桶数量
    unsigned long sizemask; // size-1, 用于hash % size
    unsigned long used;     // 已有节点数
} dictht;

typedef struct dict {
    dictType* type;         // 类型特定函数
    void* privdata;         // 私有数据
    dictht ht[2];           // 两个哈希表，用于渐进式rehash
    long rehashidx;         // rehash进度，-1表示不在rehash
    int16_t pauserehash;    // 安全迭代器计数
} dict;

// 渐进式rehash时:
// ht[0]: 旧表，只减不增
// ht[1]: 新表，所有新数据插入这里
```

### 2.2 渐进式Rehash

```c
// 执行一步rehash (移动一个桶)
int dictRehashStep(dict* d) {
    if (d->pauserehash == 0)
        return dictRehash(d, 1);  // 移动1个桶
    return 0;
}

// 定时rehash多个桶
int dictRehash(dict* d, int n) {
    int empty_visits = n * 10;  // 最大空桶检查数

    while (n-- && d->ht[0].used != 0) {
        dictEntry* de, *nextde;

        // 找到下一个非空桶
        while (d->ht[0].table[d->rehashidx] == NULL) {
            d->rehashidx++;
            if (--empty_visits == 0) return 1;
        }

        de = d->ht[0].table[d->rehashidx];

        // 移动该桶所有节点到新表
        while (de) {
            uint64_t h = dictHashKey(d, de->key) & d->ht[1].sizemask;
            nextde = de->next;

            // 头插法插入新桶
            de->next = d->ht[1].table[h];
            d->ht[1].table[h] = de;

            d->ht[0].used--;
            d->ht[1].used++;
            de = nextde;
        }

        d->ht[0].table[d->rehashidx] = NULL;
        d->rehashidx++;
    }

    // rehash完成
    if (d->ht[0].used == 0) {
        zfree(d->ht[0].table);
        d->ht[0] = d->ht[1];
        _dictReset(&d->ht[1]);
        d->rehashidx = -1;
        return 0;
    }

    return 1;  // 还有数据需要rehash
}

// 查找时同时检查两个表
dictEntry* dictFind(dict* d, const void* key) {
    dictEntry* he;
    uint64_t h, idx, table;

    if (d->ht[0].used + d->ht[1].used == 0) return NULL;

    // 如果在rehash，先执行一步
    if (dictIsRehashing(d)) _dictRehashStep(d);

    h = dictHashKey(d, key);

    // 搜索两个表
    for (table = 0; table <= 1; table++) {
        idx = h & d->ht[table].sizemask;
        he = d->ht[table].table[idx];

        while (he) {
            if (key == he->key || dictCompareKeys(d, key, he->key))
                return he;
            he = he->next;
        }

        if (!dictIsRehashing(d)) break;
    }
    return NULL;
}
```

---

## 3. Skip List (有序集合ZSet)

### 3.1 结构定义

```c
// server.h
// 跳表节点
typedef struct zskiplistNode {
    sds ele;                    // 成员
    double score;               // 分数
    struct zskiplistNode* backward;  // 后退指针
    struct zskiplistLevel {
        struct zskiplistNode* forward;   // 前进指针
        unsigned long span;              // 跨度 (用于rank)
    } level[];                  // 柔性数组，层数随机
} zskiplistNode;

typedef struct zskiplist {
    struct zskiplistNode* header, *tail;
    unsigned long length;       // 节点总数
    int level;                  // 最大层数
} zskiplist;
```

### 3.2 随机层数

```c
// 节点层数随机，幂次分布
// 第1层: 100%
// 第2层: 50%
// 第3层: 25%
// ...

#define ZSKIPLIST_MAXLEVEL 32
#define ZSKIPLIST_P 0.25      // 晋升概率

int zslRandomLevel(void) {
    int level = 1;
    while ((random() & 0xFFFF) < (ZSKIPLIST_P * 0xFFFF))
        level += 1;
    return (level < ZSKIPLIST_MAXLEVEL) ? level : ZSKIPLIST_MAXLEVEL;
}
```

### 3.3 插入操作

```c
zskiplistNode* zslInsert(zskiplist* zsl, double score, sds ele) {
    zskiplistNode* update[ZSKIPLIST_MAXLEVEL], *x;
    unsigned int rank[ZSKIPLIST_MAXLEVEL];
    int i, level;

    x = zsl->header;

    // 从最高层向下搜索插入位置
    for (i = zsl->level - 1; i >= 0; i--) {
        rank[i] = (i == zsl->level - 1) ? 0 : rank[i + 1];

        while (x->level[i].forward &&
               (x->level[i].forward->score < score ||
                (x->level[i].forward->score == score &&
                 sdscmp(x->level[i].forward->ele, ele) < 0))) {
            rank[i] += x->level[i].span;
            x = x->level[i].forward;
        }
        update[i] = x;
    }

    // 随机层数
    level = zslRandomLevel();

    // 增加新层
    if (level > zsl->level) {
        for (i = zsl->level; i < level; i++) {
            rank[i] = 0;
            update[i] = zsl->header;
            update[i]->level[i].span = zsl->length;
        }
        zsl->level = level;
    }

    // 创建节点
    x = zslCreateNode(level, score, ele);

    // 插入各层
    for (i = 0; i < level; i++) {
        x->level[i].forward = update[i]->level[i].forward;
        update[i]->level[i].forward = x;

        // 更新跨度
        x->level[i].span = update[i]->level[i].span - (rank[0] - rank[i]);
        update[i]->level[i].span = (rank[0] - rank[i]) + 1;
    }

    // 增加层的不需要更新span
    for (i = level; i < zsl->level; i++) {
        update[i]->level[i].span++;
    }

    // 后退指针
    x->backward = (update[0] == zsl->header) ? NULL : update[0];
    if (x->level[0].forward)
        x->level[0].forward->backward = x;
    else
        zsl->tail = x;

    zsl->length++;
    return x;
}
```

### 3.4 复杂度分析

| 操作 | 平均复杂度 | 最坏复杂度 |
|:-----|:-----------|:-----------|
| 插入 | O(log n) | O(n) |
| 删除 | O(log n) | O(n) |
| 查找 | O(log n) | O(n) |
| 范围查询 | O(log n + m) | O(n) |

---

## 4. 学习要点

### 4.1 内存优化技巧

```c
// 1. 柔性数组 - 变长数据
struct { int len; char data[]; };

// 2. __packed__ - 紧凑布局
struct __attribute__((packed)) { char a; int b; };

// 3. 根据大小选择结构版本
// SDS_TYPE_5/8/16/32/64

// 4. 内存预分配
// SDS多分配空间减少realloc
```

### 4.2 算法设计思想

```
渐进式rehash:
- 避免一次性大数据迁移阻塞服务
- 分摊到每次读写操作
- 定时任务补充

跳表层数随机:
- 期望O(log n)复杂度
- 简单高效，无需平衡操作
- 概率保证性能
```

---

**最后更新**: 2026-03-24
