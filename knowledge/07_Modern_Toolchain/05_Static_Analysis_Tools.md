# 静态分析工具链深度解析

> **层级定位**: 07 Modern Toolchain
> **参考工具**: Clang Static Analyzer, CodeQL, Coverity, Infer
> **难度级别**: L4-L5
> **预估学习时间**: 12-15 小时

---

## 1. 静态分析概述

### 1.1 什么是静态分析

```
静态分析：在不执行程序的情况下分析代码

┌─────────────────────────────────────────────────────────────┐
│                  静态分析层次                                │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  Level 1: 语法分析                                          │
│  • 词法分析、语法分析                                        │
│  • 编译错误检测                                              │
│  • 工具：编译器前端                                          │
│                                                             │
│  Level 2: 语义分析                                          │
│  • 类型检查                                                  │
│  • 未初始化变量                                              │
│  • 死代码检测                                                │
│  • 工具：Clang -Wall -Wextra                                 │
│                                                             │
│  Level 3: 控制流分析                                        │
│  • 空指针解引用                                              │
│  • 内存泄漏                                                  │
│  • 缓冲区溢出                                                │
│  • 工具：Clang Static Analyzer, Infer                      │
│                                                             │
│  Level 4: 符号执行                                          │
│  • 路径敏感分析                                              │
│  • 复杂条件求解                                              │
│  • 深层缺陷检测                                              │
│  • 工具：KLEE, CodeQL, Coverity                            │
│                                                             │
│  Level 5: 形式化验证                                        │
│  • 数学正确性证明                                            │
│  • 全覆盖验证                                                │
│  • 工具：Frama-C, Coq, Isabelle                            │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 1.2 静态分析类型

| 分析类型 | 描述 | 示例工具 |
|:---------|:-----|:---------|
| **Lint检查** | 代码风格、简单错误 | cppcheck, cpplint |
| **数据流分析** | 变量定义-使用链 | Clang CSA, GCC -fanalyzer |
| **控制流分析** | 可达路径、循环分析 | Clang CSA |
| **污点分析** | 外部输入追踪 | CodeQL, Coverity |
| **符号执行** | 路径条件求解 | KLEE, angr |
| **模型检测** | 状态空间遍历 | CBMC, Infer |

---

## 2. Clang Static Analyzer

### 2.1 CSA架构

```
Clang Static Analyzer架构：

源代码
   │
   ▼
┌─────────────────┐
│  Clang Frontend │  AST生成
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  Path-sensitive │  路径敏感分析
│  Analysis Engine│  符号执行
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│  Bug Reporter   │  报告生成
└─────────────────┘

核心概念：
• ExplodedGraph: 程序状态的爆炸图
• ProgramState: 程序状态（存储、约束）
• Checker: 特定缺陷模式的检查器
```

### 2.2 使用CSA

```bash
# ========== 基本用法 ==========

# 使用scan-build包装编译
scan-build gcc -c program.c

# 分析整个项目
scan-build make

# 生成HTML报告
scan-build -o ./reports make

# 使用特定检查器
scan-build --enable-checker security \
           --enable-checker unix \
           --enable-checker core \
           make

# 禁用某些检查器
scan-build --disable-checker core.NullDereference make

# ========== 检查器列表 ==========

# 核心检查器
scan-build --list-checkers | grep core

# 主要检查器类别：
# core.*           - 核心分析（空指针、未定义行为）
# cplusplus.*      - C++特定
# deadcode.*       - 死代码检测
# nullability.*    - 可空性检查
# optin.*          - 可选检查（性能等）
# security.*       - 安全相关
# unix.*           - Unix API使用
# alpha.*          - 实验性检查器
```

### 2.3 自定义Checker开发

```cpp
// NullDereferenceChecker.cpp 简化示例

#include "ClangSACheckers.h"
#include "clang/StaticAnalyzer/Core/BugReporter/BugType.h"
#include "clang/StaticAnalyzer/Core/Checker.h"
#include "clang/StaticAnalyzer/Core/PathSensitive/CheckerContext.h"

using namespace clang;
using namespace ento;

namespace {

class NullDereferenceChecker : public Checker<check::Location> {
  mutable std::unique_ptr<BugType> BT;

  void reportNullDereference(CheckerContext &C,
                             const Expr *E,
                             bool isLoad) const;

public:
  void checkLocation(SVal location, bool isLoad, const Stmt *S,
                     CheckerContext &C) const;
};

void NullDereferenceChecker::checkLocation(SVal location, bool isLoad,
                                           const Stmt *S,
                                           CheckerContext &C) const {
  // 检查位置是否可能为空
  Optional<DefinedSVal> DV = location.getAs<DefinedSVal>();
  if (!DV)
    return;

  ConstraintManager &CM = C.getConstraintManager();
  ConditionTruthVal isNull = CM.isNull(C, *DV);

  // 如果确定为NULL
  if (isNull.isConstrainedTrue()) {
    reportNullDereference(C, cast<Expr>(S), isLoad);
  }
  // 如果可能为NULL（需要分叉）
  else if (!isNull.isConstrainedFalse()) {
    // 生成两个路径：NULL和非NULL
    // NULL路径报告错误
    if (ExplodedNode *N = C.generateErrorNode()) {
      reportNullDereference(C, cast<Expr>(S), isLoad);
    }
  }
}

void NullDereferenceChecker::reportNullDereference(
    CheckerContext &C, const Expr *E, bool isLoad) const {
  if (!BT)
    BT.reset(new BugType(this, "Null pointer dereference",
                         "Core"));

  ExplodedNode *N = C.generateErrorNode();
  if (!N)
    return;

  auto R = std::make_unique<PathSensitiveBugReport>(
      *BT, "Dereference of null pointer", N);
  R->addRange(E->getSourceRange());
  C.emitReport(std::move(R));
}

} // anonymous namespace

void ento::registerNullDereferenceChecker(CheckerManager &mgr) {
  mgr.registerChecker<NullDereferenceChecker>();
}
```

---

## 3. CodeQL

### 3.1 CodeQL基础

```
CodeQL架构：

源代码                      数据库                      查询
   │                          │                          │
   ▼                          ▼                          ▼
┌─────────┐    extraction   ┌─────────┐    QL查询     ┌─────────┐
│   C/C++ │ ───────────────>│ CodeQL  │ ────────────>│  结果   │
│   Java  │                 │ Database│              │ (漏洞)  │
│   Python│                 │ (关系型)│              │         │
└─────────┘                 └─────────┘              └─────────┘

CodeQL数据库包含：
• 抽象语法树(AST)
• 控制流图(CFG)
• 数据流图(DFG)
• 调用图
• 类型信息
```

### 3.2 CodeQL使用

```bash
# ========== 创建数据库 ==========

# C/C++项目
codeql database create cpp-database \
    --language=cpp \
    --command="make"

# 或者使用自定义构建命令
codeql database create cpp-database \
    --language=cpp \
    --command="cmake --build build"

# ========== 运行查询 ==========

# 使用标准查询套件
codeql database analyze cpp-database \
    codeql/cpp/ql/src/Security/CWE \
    --format=sarifv2.1.0 \
    --output=results.sarif

# 使用所有查询
codeql database analyze cpp-database \
    codeql/cpp/ql/src/codeql-suites/cpp-security-and-quality.qls \
    --format=sarifv2.1.0 \
    --output=results.sarif

# ========== 编写自定义查询 ==========
```

### 3.3 CodeQL查询示例

```ql
// 自定义查询：查找未检查的malloc返回值

/**
 * @name Unchecked return value of malloc
 * @description The return value of malloc is not checked for NULL
 * @kind problem
 * @problem.severity warning
 * @precision high
 * @id cpp/unchecked-malloc
 */

import cpp

from FunctionCall call
where
  call.getTarget().hasName("malloc") and
  not exists(IfStmt ifstmt |
    ifstmt.getCondition().getAChild*() = call.getParent()
  ) and
  not exists(Variable v |
    v.getInitializer().getExpr() = call and
    exists(IfStmt ifstmt |
      ifstmt.getCondition().getAChild*() = v.getAnAccess()
    )
  )
select call, "Return value of malloc is not checked for NULL"
```

```ql
// 数据流查询：污点追踪

/**
 * @name User-controlled data to system call
 * @description Finding paths from user input to system calls
 */

import cpp
import semmle.code.cpp.dataflow.TaintTracking
import semmle.code.cpp.security.Security

class UserInputToSystemCallConfig extends TaintTracking::Configuration {
  UserInputToSystemCallConfig() { this = "UserInputToSystemCall" }

  override predicate isSource(DataFlow::Node source) {
    // 用户输入源
    source.asExpr() instanceof UserInput
  }

  override predicate isSink(DataFlow::Node sink) {
    // 系统调用汇点
    exists(FunctionCall call |
      call.getTarget().hasName("system") and
      sink.asExpr() = call.getArgument(0)
    )
    or
    exists(FunctionCall call |
      call.getTarget().hasName("execve") and
      sink.asExpr() = call.getArgument(0)
    )
  }
}

from UserInputToSystemCallConfig config, DataFlow::Node source, DataFlow::Node sink
where config.hasFlow(source, sink)
select sink, "User-controlled data flows to system call", source, sink
```

---

## 4. Coverity

### 4.1 Coverity分析引擎

```
Coverity Analysis:

┌─────────────────────────────────────────────────────────────┐
│  1. 编译器集成                                              │
│  • cov-build 拦截编译命令                                    │
│  • 捕获编译选项和宏定义                                       │
│  • 生成编译数据库                                            │
├─────────────────────────────────────────────────────────────┤
│  2. 语义分析                                                │
│  • 过程间分析（IPA）                                         │
│  • 路径敏感分析                                              │
│  • 统计驱动分析                                              │
├─────────────────────────────────────────────────────────────┤
│  3. 缺陷检测                                                │
│  • 250+检查器                                               │
│  • 安全、可靠性、性能                                        │
│  • 低误报率                                                  │
├─────────────────────────────────────────────────────────────┤
│  4. 报告与追踪                                              │
│  • Web界面管理                                               │
│  • 缺陷分类与优先级                                          │
│  • 趋势分析                                                  │
└─────────────────────────────────────────────────────────────┘
```

### 4.2 Coverity使用

```bash
# ========== 构建捕获 ==========

# 配置编译器
cov-configure --compiler gcc --comptype gcc

# 捕获构建
cov-build --dir cov-int make

# 或者使用编译命令数据库
cov-build --dir cov-int --json-compilation-db compile_commands.json

# ========== 分析 ==========

cov-analyze --dir cov-int \
    --aggressiveness-level high \
    --all \
    --enable-constraint-fpp \
    --enable-fnptr \
    --enable-virtual

# 特定检查器
cov-analyze --dir cov-int --checker NO_EFFECT

# ========== 报告 ==========

cov-format-errors --dir cov-int --html-output html-report

# 提交到Coverity Connect服务器
cov-commit-defects --dir cov-int \
    --host coverity-server \
    --port 8080 \
    --stream myproject \
    --user username
```

### 4.3 Coverity建模文件

```c
// 用户建模文件：帮助Coverity理解自定义API

// 告诉Coverity malloc返回的内存需要释放
// coverity[+alloc]
void *malloc(size_t size);

// coverity[+free]
void free(void *ptr);

// 自定义锁函数建模
// coverity[+lock]
void my_mutex_lock(my_mutex_t *mutex);

// coverity[+unlock]
void my_mutex_unlock(my_mutex_t *mutex);

// 告诉Coverity函数不会返回
// coverity[+kill]
void fatal_error(const char *msg);

// 告诉Coverity函数会清零内存
// coverity[+set]
void secure_zero(void *ptr, size_t len);
```

---

## 5. 其他静态分析工具

### 5.1 Cppcheck

```bash
# Cppcheck - 快速、轻量级

# 基本检查
cppcheck --enable=all --inconclusive --std=c11 src/

# 特定检查类别
cppcheck --enable=warning,style,performance,portability,information,missingInclude src/

# 生成XML报告
cppcheck --enable=all --xml --xml-version=2 src/ 2> cppcheck-results.xml

# 抑制误报
cppcheck --suppressions-list=suppressions.txt src/

# 并行检查（更快）
cppcheck -j 4 --enable=all src/
```

### 5.2 Infer (Facebook)

```bash
# Infer - 专注于内存安全和并发

# 分析单个文件
infer -- gcc -c program.c

# 分析整个项目
infer -- make

# 增量分析
infer --continue -- make

# 查看报告
infer explore --html

# 特定检查器
infer --biabduction-only -- make  # 内存错误
infer --racerd-only -- make        # 竞态条件
infer --pulse-only -- make         # 空安全
```

### 5.3 工具对比

| 特性 | Clang CSA | CodeQL | Coverity | Infer |
|:-----|:----------|:-------|:---------|:------|
| 开源 | 是 | 部分 | 否 | 是 |
| 速度 | 中 | 慢 | 慢 | 快 |
| 精度 | 高 | 高 | 最高 | 中 |
| 误报率 | 低 | 低 | 最低 | 低 |
| CI集成 | 好 | 好 | 好 | 好 |
| 自定义 | 中等 | 强 | 中等 | 弱 |

---

## 6. 静态分析集成到CI/CD

```yaml
# .github/workflows/static-analysis.yml
name: Static Analysis

on: [push, pull_request]

jobs:
  clang-analyzer:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v3

    - name: Install Clang
      run: sudo apt-get install -y clang-tools

    - name: Run Clang Static Analyzer
      run: |
        scan-build -o scan-results \
          --status-bugs \
          make

    - name: Upload results
      uses: actions/upload-artifact@v3
      with:
        name: clang-analyzer-report
        path: scan-results/

  codeql:
    runs-on: ubuntu-latest
    permissions:
      security-events: write
    steps:
    - uses: actions/checkout@v3

    - name: Initialize CodeQL
      uses: github/codeql-action/init@v2
      with:
        languages: cpp
        queries: security-extended,security-and-quality

    - name: Build
      run: make

    - name: Perform CodeQL Analysis
      uses: github/codeql-action/analyze@v2

  cppcheck:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v3

    - name: Install cppcheck
      run: sudo apt-get install -y cppcheck

    - name: Run cppcheck
      run: |
        cppcheck --enable=all --error-exitcode=1 \
          --inline-suppr \
          --suppress=missingIncludeSystem \
          src/
```

---

## 关联导航

### 前置知识

- [编译器前端与AST](../02_Formal_Semantics_and_Physics/11_Compiler_Internals/01_AST_and_IR.md)
- [Clang工具链](./README.md)

### 后续延伸

- [动态分析工具](./05_Dynamic_Analysis_Tools.md)
- [形式化验证](../02_Formal_Semantics_and_Physics/13_Formal_Methods/01_Correctness_by_Construction.md)

### 参考

- Clang Static Analyzer文档
- CodeQL文档 (<https://codeql.github.com/>)
- Coverity文档
- Static Program Analysis, Flemming Nielson
