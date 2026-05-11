# SQLite 测试哲学：工业级测试密度实践

> **源码版本**: SQLite 3.45+
> **参考文档**: [SQLite Testing](https://www.sqlite.org/testing.html)
> **核心指标**: 590:1 测试代码/产品代码比例

---

## 1. 测试密度：为什么 SQLite 是行业标杆

### 1.1 数据对比

| 项目 | 产品代码 (SLOC) | 测试代码 (SLOC) | 测试比例 | 关键实践 |
|:-----|:----------------|:----------------|:---------|:---------|
| **SQLite** | ~150,000 | ~91,000,000 (TCL脚本) | **590:1** | MC/DC、模糊测试、回归套件 |
| Linux Kernel | ~30,000,000 | ~ millions | ~30:1 | kselftest, KUnit |
| PostgreSQL | ~1,000,000 | ~ millions | ~100:1 | regress, TAP |
| 典型企业项目 | varies | varies | 1:1 ~ 5:1 | 单元测试 |

SQLite 的 590:1 比例不是堆砌测试用例，而是**测试生成技术**的结果：大量 TCL 脚本自动生成组合测试。

### 1.2 测试哲学核心

```
SQLite 的测试不是验证"代码正确"，而是验证"代码在任何输入下都不崩溃"
```

三条铁律：
1. **分支覆盖**: 100% 条件/分支覆盖（实际接近 100% MC/DC）
2. **防御性测试**: 测试非法输入、内存不足、磁盘满、系统调用失败
3. **平台矩阵**: 在数十种操作系统、编译器、架构组合上运行

---

## 2. 测试体系架构

### 2.1 五层测试金字塔

```
                    ┌─────────────────┐
                    │  5. 模糊测试    │  ← AFL, libFuzzer
                    │  (Fuzz Tests)   │     数十亿随机输入
                    └────────┬────────┘
                             │
                    ┌────────┴────────┐
                    │ 4. 回归测试套件 │  ← 数十万个 SQL 语句
                    │ (Regression)    │     历史 Bug 永不复发
                    └────────┬────────┘
                             │
                    ┌────────┴────────┐
                    │ 3. 边界条件测试 │  ← 极端值、空值、溢出
                    │ (Boundary)      │     每个分支的真/假路径
                    └────────┬────────┘
                             │
                    ┌────────┴────────┐
                    │ 2. 集成测试     │  ← TCL 脚本驱动完整场景
                    │ (Integration)   │     事务、并发、崩溃恢复
                    └────────┬────────┘
                             │
                    ┌────────┴────────┐
                    │ 1. 单元测试     │  ← C 函数级测试
                    │ (Unit Tests)    │     内存分配器、B-Tree 操作
                    └─────────────────┘
```

### 2.2 TCL 测试框架

SQLite 的核心测试基础设施是自定义的 **TCL 测试框架**（`test/` 目录下的数千个 `.test` 文件）。

```tcl
# 典型 TCL 测试示例 (test/select1.test)
do_test select1-1.1 {
  execsql {
    CREATE TABLE t1(a INTEGER PRIMARY KEY, b TEXT);
    INSERT INTO t1 VALUES(1, 'hello');
    INSERT INTO t1 VALUES(2, 'world');
    SELECT * FROM t1 ORDER BY a;
  }
} {1 hello 2 world}

# 自动验证输出匹配期望值
```

关键设计：
- **确定性**: 每个测试用例完全独立，无全局状态污染
- **可重复**: 相同的种子产生相同的测试序列
- **快速失败**: 失败立即停止，便于定位

---

## 3. MC/DC：Modified Condition/Decision Coverage

### 3.1 什么是 MC/DC

MC/DC（修正条件/判定覆盖）是 DO-178C 标准中要求的最严格覆盖标准之一，用于安全关键软件：

> **要求**: 对于每个条件的每个可能结果，必须存在测试用例：
> 1. 独立影响判定的输出
> 2. 同时保持其他条件不变

### 3.2 C 语言示例

```c
// 需要 MC/DC 覆盖的判定
if (ptr != NULL && ptr->len > 0 && ptr->data != NULL) {
    process(ptr);
}

// MC/DC 要求 4 个测试用例：
// T1: ptr=NULL      → false (独立证明第一个条件)
// T2: ptr!=NULL, len=0, data=NULL → false (独立证明第二个条件)
// T3: ptr!=NULL, len>0, data=NULL → false (独立证明第三个条件)
// T4: ptr!=NULL, len>0, data!=NULL → true  (所有条件为真)
```

### 3.3 SQLite 的 MC/DC 实践

SQLite 虽然不是航空软件，但自愿采用 MC/DC 级别的覆盖：

| 覆盖类型 | 要求 | SQLite 实际 |
|:---------|:-----|:------------|
| 语句覆盖 (Statement) | 100% | 100% |
| 分支覆盖 (Branch) | 100% | 100% |
| 条件覆盖 (Condition) | 100% | ~99.9% |
| MC/DC | 所有条件独立影响 | ~99.5% |

**工具链**: 
- 使用 `gcov` + 自定义脚本生成覆盖报告
- 每晚自动构建检查覆盖退化
- 任何提交导致覆盖下降会被拒绝

---

## 4. 突变测试 (Mutation Testing)

### 4.1 原理

突变测试验证**测试套件的有效性**：
1. 自动修改源代码（引入"突变体"）
2. 运行测试套件
3. 如果测试失败 → "杀死"突变体（好）
4. 如果测试通过 → "存活"突变体（坏，说明测试不足）

### 4.2 C 语言突变算子

```c
// 原始代码
if (a > b) return a;

// 突变 1: 关系运算符
if (a >= b) return a;   // 边界突变
if (a < b) return a;    // 反向突变

// 突变 2: 返回值
if (a > b) return b;    // 返回值替换

// 突变 3: 语句删除
if (a > b) ;            // 空语句
```

### 4.3 SQLite 的突变策略

SQLite 不使用标准突变测试工具（如 Mull），而是采用**手动突变验证**：
- 每个 Bug 修复后，验证旧测试能否捕获该 Bug
- 故意"回退"修复，确认测试失败
- 这等价于验证突变体被"杀死"

### 4.4 现代 C 突变测试工具

```bash
# Mull (基于 LLVM IR)
$ mull-cxx --mutators=all-codes ./my_test

# 输出示例
[info] Mutation score: 87.3%
[info] Killed mutants: 1,247
[info] Survived mutants: 183  ← 需要补充测试
```

---

## 5. 防御性测试：故障注入

### 5.1 系统调用故障模拟

SQLite 的核心创新之一是 **"故障注入虚拟文件系统"**（Fault-Injecting VFS）：

```c
// sqlite3_test_control() 接口
sqlite3_test_control(SQLITE_TESTCTRL_FAULT_INSTALL, pFault);

// 模拟第 N 次 malloc 失败
void simulate_oom(int n) {
    sqlite3_test_control(SQLITE_TESTCTRL_FAULT_CONFIG, 
                         n,  // 第 n 次分配失败
                         1); // 启用故障
}
```

测试场景矩阵：

| 故障类型 | 模拟方法 | 测试目的 |
|:---------|:---------|:---------|
| malloc 失败 | 计数器控制 | 内存不足处理 |
| 磁盘满 | VFS 钩子 | 写入失败恢复 |
| 磁盘 I/O 错误 | VFS 模拟 | 页损坏检测 |
| 系统崩溃 | 进程 kill | WAL 恢复机制 |
| 时钟跳变 | 时间函数钩子 | 超时逻辑 |

### 5.2 崩溃恢复测试

```tcl
# test/crash.test 片段
# 在事务执行中途强制崩溃，验证 WAL 恢复
do_test crash-1.1 {
  crashsql -delay 1 -file test.db-journal {
    BEGIN;
    INSERT INTO t1 VALUES(1,2,3);
    -- 此处模拟崩溃
  }
  execsql { SELECT * FROM t1 }
} {}
# 期望: 崩溃后数据库仍一致，未提交数据不出现
```

---

## 6. 模糊测试 (Fuzzing)

### 6.1 SQLite 的模糊测试基础设施

SQLite 维护两个持续运行的模糊测试集群：

| 集群 | 工具 | 输入 | 持续时间 |
|:-----|:-----|:-----|:---------|
| **OSS-Fuzz** | libFuzzer + AFL | SQL 语法 | 24/7 持续 |
| **内部集群** | 自定义生成器 | 变异 SQL | 每次提交前 |

### 6.2 SQL 语法模糊测试

```c
// fuzzershell.c - SQLite 的模糊测试入口
int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    sqlite3 *db;
    sqlite3_open(":memory:", &db);
    
    // 输入数据作为 SQL 语句执行
    sqlite3_exec(db, (const char*)data, 0, 0, 0);
    
    sqlite3_close(db);
    return 0;
}
```

**关键**: 模糊测试不仅找崩溃，还验证：
- 内存泄漏 (ASan)
- 未定义行为 (UBSan)
- 断言失败

### 6.3 为 C 项目集成模糊测试

```bash
# 1. 编译带 sanitizer 的模糊目标
clang -fsanitize=fuzzer,address,undefined \
      -o my_fuzzer fuzz_target.c \
      -I. -L. -lmylib

# 2. 运行（自动语料库管理）
./my_fuzzer corpus/ -max_total_time=3600

# 3. 复现崩溃
./my_fuzzer crash-xxx
```

---

## 7. 可借鉴的 C 项目测试模式

### 7.1 测试 harness 设计

```c
// 最小化 SQLite 风格的 C 测试框架
#include <assert.h>
#include <stdio.h>
#include <string.h>

static int test_count = 0;
static int fail_count = 0;

#define TEST(name) static void test_##name(void)
#define RUN_TEST(name) do { \
    printf("  %s ... ", #name); \
    test_##name(); \
    test_count++; \
    printf("OK\n"); \
} while(0)

#define ASSERT_EQ(a, b) do { \
    if ((a) != (b)) { \
        printf("FAIL: %s:%d: %ld != %ld\n", \
               __FILE__, __LINE__, (long)(a), (long)(b)); \
        fail_count++; \
        return; \
    } \
} while(0)

// 使用示例
TEST(btree_insert) {
    Btree *bt = btree_create();
    btree_insert(bt, 42, "hello");
    ASSERT_EQ(btree_count(bt), 1);
    btree_destroy(bt);
}

int main(void) {
    printf("Running tests...\n");
    RUN_TEST(btree_insert);
    printf("\n%d/%d passed\n", test_count - fail_count, test_count);
    return fail_count;
}
```

### 7.2 测试即文档

SQLite 的 TCL 测试文件就是最好的 API 使用文档：
- 每个测试展示一个具体用法
- 边界条件明确标注
- 错误处理模式清晰可见

---

## 8. 关键启示

### 对于 C 语言项目的测试建议

| 优先级 | 实践 | 投入产出比 |
|:-------|:-----|:-----------|
| P0 | **分支覆盖 >90%** | 高 |
| P0 | **AddressSanitizer 常规运行** | 极高 |
| P1 | **故障注入测试**（OOM、I/O 错误） | 高 |
| P1 | **模糊测试**（至少 1 小时/提交） | 高 |
| P2 | **MC/DC 覆盖**（安全关键模块） | 中 |
| P2 | **突变测试**（核心算法模块） | 中 |
| P3 | **多平台 CI 矩阵** | 中 |

### 单文件 C 项目的测试起点

即使是最小的 C 项目，也应至少包含：

```bash
# 最小测试套件结构
project/
├── src/
│   └── mylib.c
├── tests/
│   ├── test_basic.c      # 单元测试
│   ├── fuzz_target.c     # 模糊测试入口
│   └── fixtures/         # 测试数据
├── Makefile
└── .github/workflows/
    └── ci.yml            # ASan + UBSan + 覆盖率
```

---

> **最后更新**: 2026-05-11
> **维护者**: C_Lang Knowledge Base
> **参考**: SQLite Testing Strategy (sqlite.org), DO-178C Coverage Analysis
