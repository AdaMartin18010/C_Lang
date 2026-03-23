---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../../01_Core_Knowledge_System/05_Engineering_Layer/README.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../../02_Formal_Semantics_and_Physics/README.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../../03_System_Technology_Domains/README.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../../04_Industrial_Scenarios/README.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../../06_Thinking_Representation/README.md) | 核心关联 | 思维表征基础 |
# Git 内部机制源码解读

> **源码版本**: Git 2.40+
> **核心文件**: sha1-file.c, object.c, commit.c, diff.c
> **学习重点**: 内容寻址存储、对象模型、diff算法

---

## 1. 整体架构

### 1.1 核心设计

```
Git = 内容寻址文件系统 + VCS界面

关键特性:
- 快照而非差异
- 本地操作优先
- 完整性保证 (SHA-1)
- 分支即引用
```

### 1.2 数据模型

```
对象数据库 (.git/objects)
├── blob (文件内容)
├── tree (目录结构)
├── commit (快照元数据)
└── tag (签名标签)

引用 (.git/refs)
├── heads/ (分支)
├── tags/  (标签)
└── remotes/ (远程追踪)

索引 (.git/index) - 暂存区
```

---

## 2. 对象系统

### 2.1 对象结构

```c
// git object通用结构
struct object {
    unsigned parsed : 1;      // 是否已解析
    unsigned type : TYPE_BITS; // 对象类型
    unsigned flags : FLAG_BITS;

    struct object_id oid;     // SHA-1哈希
};

// blob对象 - 仅存储内容
struct blob {
    struct object object;
};

// tree对象 - 目录项列表
struct tree {
    struct object object;
    void* buffer;             // 序列化数据
    unsigned long size;
};

// commit对象
struct commit {
    struct object object;
    struct commit_list* parents;  // 父提交
    struct tree* tree;            // 根tree
    struct strbuf buffer;         // 原始数据

    char* message;            // 提交信息
    struct ident_split author;
    struct ident_split committer;
};
```

### 2.2 对象存储格式

```
对象格式: header + '\0' + content

header: "<type> <size>"

示例 (blob):
"blob 11\0Hello World" → SHA-1 → 3b18e5...

压缩存储:
- 使用zlib压缩
- 按SHA-1前2位分目录
  .git/objects/3b/18e512dba79e4c8300dd08aeb37f8e728b8dad
```

### 2.3 对象读写

```c
// 读取对象
int read_object_file(const struct object_id* oid,
                     enum object_type* type,
                     unsigned long* size,
                     void** data) {
    // 1. 构造路径: .git/objects/xx/xxxxxxxx...
    char* path = sha1_file_name(oid);

    // 2. 打开文件
    int fd = open(path, O_RDONLY);

    // 3. 解压缩 (zlib)
    git_zstream stream;
    git_inflate_init(&stream);

    // 4. 读取并解压
    void* buf = NULL;
    unsigned long buf_size = 0;

    // ... 解压逻辑

    // 5. 解析header
    const char* hdr = buf;
    *type = parse_type(hdr);
    *size = parse_size(hdr);
    *data = buf + header_len;

    return 0;
}

// 写入对象
int write_object_file(const void* buf, unsigned long len,
                      enum object_type type,
                      struct object_id* oid) {
    // 1. 构造内容: header + data
    struct strbuf hdr = STRBUF_INIT;
    strbuf_addf(&hdr, "%s %lu", type_name(type), len);

    // 2. 计算SHA-1
    hash_object_file(hdr.buf, hdr.len, buf, len, oid);

    // 3. 检查是否已存在
    if (has_object_file(oid))
        return 0;

    // 4. 压缩 (zlib)
    git_zstream stream;
    unsigned char compressed[MAX_COMPRESSED_SIZE];
    // ... 压缩

    // 5. 写入临时文件后原子重命名
    char tmp_path[PATH_MAX];
    int fd = create_tmpfile(tmp_path);
    write(fd, compressed, compressed_len);
    close(fd);

    char* obj_path = sha1_file_name(oid);
    mkdir_for_path(obj_path);
    rename(tmp_path, obj_path);

    return 0;
}
```

---

## 3. 索引 (Index/Staging Area)

### 3.1 索引结构

```c
// 索引文件格式
struct index_state {
    struct cache_entry** cache;  // 条目数组
    unsigned int cache_nr;        // 条目数
    unsigned int cache_alloc;     // 分配大小

    struct strbuf name_hash;
    unsigned int name_hash_nr;

    struct cache_tree* cache_tree; // 缓存的tree对象

    int changed;                  // 是否修改
    int version;                  // 格式版本
};

// 单个条目 (cache_entry)
struct cache_entry {
    struct cache_time ce_ctime;   // 元数据时间
    struct cache_time ce_mtime;
    unsigned int ce_dev;
    unsigned int ce_ino;
    unsigned int ce_mode;
    unsigned int ce_uid;
    unsigned int ce_gid;
    unsigned int ce_size;

    struct object_id oid;         // blob哈希
    unsigned short ce_flags;
    char name[FLEX_ARRAY];        // 文件名 (柔性数组)
};
```

### 3.2 索引操作

```c
// 添加文件到索引
int add_to_index(struct index_state* istate, const char* path,
                 const struct object_id* oid, int st_mode) {
    struct cache_entry* ce;
    int pos;

    // 检查是否已存在
    pos = index_name_pos(istate, path, strlen(path));

    if (pos >= 0) {
        // 更新现有条目
        ce = istate->cache[pos];
        oidcpy(&ce->oid, oid);
        ce->ce_mode = create_ce_mode(st_mode);
    } else {
        // 创建新条目
        pos = -pos - 1;
        ce = make_cache_entry(oid, st_mode, path);

        // 插入到正确位置 (保持排序)
        add_index_entry_at(istate, pos, ce);
    }

    istate->changed = 1;
    return 0;
}

// 从索引创建tree对象
struct tree* write_index_as_tree(struct index_state* istate) {
    struct object_id oid;

    // 递归构建tree
    cache_tree_update(istate->cache_tree, istate->cache, istate->cache_nr);

    // 写入顶层tree
    write_cache_as_tree(&oid, istate->cache_tree->sha1, istate);

    return lookup_tree(the_repository, &oid);
}
```

---

## 4. 提交历史与遍历

### 4.1 提交链

```c
// 提交图遍历
struct commit_list {
    struct commit* item;
    struct commit_list* next;
};

// 遍历历史 (类似链表)
void traverse_history(struct commit* start) {
    struct commit_list* list = NULL;
    commit_list_insert(start, &list);

    while (list) {
        struct commit* commit = pop_commit(&list);

        // 处理当前提交
        process_commit(commit);

        // 添加父提交
        struct commit_list* parents = commit->parents;
        while (parents) {
            if (!parents->item->object.parsed)
                commit_list_insert(parents->item, &list);
            parents = parents->next;
        }
    }
}
```

### 4.2 合并基础 (Merge Base)

```c
// 查找两个提交的最近公共祖先
struct commit* get_merge_base(struct commit* one, struct commit* two) {
    struct commit_list* result = NULL;

    // 标记两个提交的所有祖先
    struct commit_list* list1 = NULL;
    struct commit_list* list2 = NULL;

    paint_down_to_common(one, &list1);
    paint_down_to_common(two, &list2);

    // 找到最近的公共提交
    result = merge_bases(one, two);

    return result ? result->item : NULL;
}
```

---

## 5. Diff算法

### 5.1 Myers差分算法

```c
// 核心思想: 最短编辑脚本 (SES)
// 将diff问题转化为图的最短路径问题

struct diff_entry {
    int begin_a, end_a;
    int begin_b, end_b;
};

// 简化的Myers算法
void myers_diff(int* a, int n, int* b, int m) {
    int max_d = n + m;
    int* v = calloc(2 * max_d + 1, sizeof(int));
    int d, k, x, y;

    v[max_d] = 0;

    for (d = 0; d <= max_d; d++) {
        for (k = -d; k <= d; k += 2) {
            // 选择从上或从左扩展
            if (k == -d || (k != d && v[max_d + k - 1] < v[max_d + k + 1])) {
                x = v[max_d + k + 1];
            } else {
                x = v[max_d + k - 1] + 1;
            }

            y = x - k;

            // 对角线扩展 (匹配相同元素)
            while (x < n && y < m && a[x] == b[y]) {
                x++;
                y++;
            }

            v[max_d + k] = x;

            // 到达终点
            if (x >= n && y >= m) {
                // 回溯构建diff
                free(v);
                return;
            }
        }
    }

    free(v);
}
```

### 5.2 行级Diff

```c
// 文件diff流程
void diff_files(struct diff_filepair* p) {
    mmfile_t mf1, mf2;

    // 1. 读取文件内容
    read_mmfile(&mf1, p->one);
    read_mmfile(&mf2, p->two);

    // 2. 文本或二进制检测
    if (diff_filespec_is_binary(p->one) ||
        diff_filespec_is_binary(p->two)) {
        // 二进制: 只显示是否不同
        printf("Binary files differ\n");
        return;
    }

    // 3. 逐行比较
    xpparam_t xpp = {0};
    xdemitconf_t xecfg = {0};

    xdl_diff(&mf1, &mf2, &xpp, &xecfg, &ecb);

    // 4. 输出diff格式
    emit_diff(p, &mf1, &mf2);
}
```

---

## 6. 打包文件 (Packfile)

### 6.1 打包格式

```
Packfile: 高效的delta压缩存储

结构:
- Header: "PACK" + version(4) + num_objects(4)
- Objects: 每个对象delta压缩后存储
- Trailer: 所有对象SHA-1校验和

Delta压缩:
- 选择base对象
- 存储差异指令 (copy/insert)
- 大幅减少存储空间
```

### 6.2 增量编码

```c
// delta编码格式
// 0xxxxxxx - 从base复制offset个size字节
// 1xxxxxxx - 插入xxxxxxx字节的新数据

int patch_delta(const unsigned char* src, unsigned long src_size,
                const unsigned char* delta, unsigned long delta_size,
                unsigned char** dst, unsigned long* dst_size) {
    const unsigned char* delta_end = delta + delta_size;
    unsigned long result_size;

    // 解析结果大小
    result_size = get_delta_hdr_size(&delta, delta_end);
    *dst = xmalloc(result_size);

    unsigned char* out = *dst;
    const unsigned char* out_end = out + result_size;

    while (delta < delta_end) {
        unsigned char cmd = *delta++;

        if (cmd & 0x80) {
            // 从base复制
            unsigned long offset = 0, size = 0;

            if (cmd & 0x01) offset |= *delta++;
            if (cmd & 0x02) offset |= (*delta++) << 8;
            if (cmd & 0x04) offset |= (*delta++) << 16;
            if (cmd & 0x08) offset |= (*delta++) << 24;

            if (cmd & 0x10) size |= *delta++;
            if (cmd & 0x20) size |= (*delta++) << 8;
            if (cmd & 0x40) size |= (*delta++) << 16;

            if (size == 0) size = 0x10000;

            memcpy(out, src + offset, size);
            out += size;
        } else if (cmd) {
            // 插入新数据
            memcpy(out, delta, cmd);
            delta += cmd;
            out += cmd;
        } else {
            // 错误
            return -1;
        }
    }

    *dst_size = out - *dst;
    return 0;
}
```

---

## 7. 学习要点

### 7.1 设计思想

```
内容寻址:
- SHA-1作为对象ID
- 天然去重
- 完整性验证

不可变对象:
- 一旦写入永不修改
- 简化缓存和共享
- 原子操作友好

引用可变:
- 分支只是指针
- 轻量级创建
- 灵活的历史操作
```

### 7.2 代码质量

| 特点 | 说明 |
|:-----|:-----|
| 性能优先 | 大量的缓存和优化 |
| 跨平台 | 抽象层处理系统差异 |
| 兼容性 | 支持旧格式 |
| 测试覆盖 | 大量回归测试 |

---

**最后更新**: 2026-03-24
