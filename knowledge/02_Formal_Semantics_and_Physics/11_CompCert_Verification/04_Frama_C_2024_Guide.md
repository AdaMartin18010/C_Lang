# Frama-C 2024 完整指南

> **文档版本**: 基于2024年《Frama-C软件验证指南》
> **Frama-C版本**: 30.0 Zinc / 29.0 Copper
> **更新日期**: 2026-03-18

---

## 概述

**Frama-C**是一个用于C代码分析、验证和转换的平台。2024年发布的《Guide to Software Verification with Frama-C》由39位作者共同编写，全面介绍了Frama-C的功能和工业应用。

### 核心组件

```
┌─────────────────────────────────────────────────────────────┐
│                     Frama-C Platform                        │
├─────────────────────────────────────────────────────────────┤
│  Frama-C Kernel (解析、AST、Visitor模式)                      │
├─────────────────────────────────────────────────────────────┤
│  插件生态                                                    │
│  ┌──────────┬──────────┬──────────┬──────────┐             │
│  │   Eva    │   WP     │ PathCrawler│  E-ACSL │             │
│  │(值分析)   │(证明)    │(测试生成) │(运行时) │             │
│  ├──────────┼──────────┼──────────┼──────────┤             │
│  │  LAnnotate│  Mthread │  Security│  Reduc   │             │
│  │ (标注)   │(并发分析) │(安全分析) │(简化)   │             │
│  └──────────┴──────────┴──────────┴──────────┘             │
├─────────────────────────────────────────────────────────────┤
│  ACSL (ANSI/ISO C Specification Language)                   │
└─────────────────────────────────────────────────────────────┘
```

---

## 安装与配置

### 安装方法

```bash
# 方法1: 通过OPAM安装 (推荐)
opam install frama-c

# 方法2: 安装特定版本
opam install frama-c.30.0

# 方法3: 从源码编译
git clone https://git.frama-c.com/pub/frama-c.git
cd frama-c
opam install . --deps-only
make
sudo make install

# 验证安装
frama-c --version
```

### 验证工具链

```bash
# 检查所有组件
frama-c -plugins

# 输出示例:
# [kernel] Available plugins:
#   aio           1.0       Auto-active verification
#   e-acsl        24.1      E-ACSL
#   eva           30.0      Eva
#   from          30.0      From analysis
#   gui           30.0      Gui
#   impact        30.0      Impact analysis
#   inout         30.0      Inout
#   loopanalysis  30.0      Loop Analysis
#   metrics       30.0      Metrics
#   nonterm       1.0       Nonterm
#   pdg           30.0      Pdg
#   report        30.0      Report
#   rte           30.0      Rte
#   scope         30.0      Scope
#   security      1.0       Security
#   server        30.0      Server
#   sparecode     30.0      Sparecode
#   studia        30.0      Studia
#   users         30.0      Users
#   variadic      30.0      Variadic
#   wp            30.0      Wp
```

---

## ACSL规范语言

### 基础语法

```c
/*@
  @requires \valid(p) && \valid(q);
  @requires *p >= 0 && *q >= 0;
  @assigns \nothing;
  @ensures \result == *p + *q;
  @ensures \result >= *p && \result >= *q;
*/
int add(const int* p, const int* q) {
    return *p + *q;
}
```

### 关键谓词

| 谓词 | 含义 | 示例 |
|:-----|:-----|:-----|
| `\valid(p)` | 指针有效 | `\valid(p) && \valid(p+1)` |
| `\valid_read(p)` | 可读 | `\valid_read(src)` |
| `\initialized(p)` | 已初始化 | `\initialized(&x)` |
| `\separated(p,q)` | 内存分离 | `\separated(a,b)` |
| `\old(e)` | 前值 | `\old(*p)` |
| `\result` | 返回值 | `\result > 0` |
| `\forall` | 全称量词 | `\forall integer i; ...` |
| `\exists` | 存在量词 | `\exists integer i; ...` |
| `\let x = e;` | 局部绑定 | `\let x = \old(v); ...` |

### 函数合约

```c
/*@
  @requires n >= 0;
  @requires \valid_read(arr + (0..n-1));
  @assigns \nothing;
  @ensures \result >= 0;
  @ensures \forall integer i; 0 <= i < n ==> \result >= arr[i];
  @ensures \exists integer i; 0 <= i < n && \result == arr[i];
*/
int array_max(const int* arr, int n);

/*@
  @requires \valid(dst + (0..n-1));
  @requires \valid_read(src + (0..n-1));
  @requires \separated(dst+(0..n-1), src+(0..n-1));
  @assigns dst[0..n-1];
  @ensures \forall integer i; 0 <= i < n ==> dst[i] == \old(src[i]);
  @ensures \result == dst;
*/
char* my_memcpy(char* dst, const char* src, size_t n);
```

### 循环不变量

```c
/*@
  @requires n >= 0 && \valid_read(arr + (0..n-1));
  @assigns \nothing;
  @ensures \result == \sum(0,n-1,\lambda integer k; arr[k]);
*/
int array_sum(const int* arr, int n) {
    int sum = 0;
    /*@
      @loop invariant 0 <= i <= n;
      @loop invariant sum == \sum(0,i-1,\lambda integer k; arr[k]);
      @loop assigns i, sum;
      @loop variant n - i;
    */
    for (int i = 0; i < n; i++) {
        sum += arr[i];
    }
    return sum;
}
```

---

## Eva插件 (值分析)

### 基础用法

```bash
# 基本分析
frama-c -eva program.c

# 详细输出
frama-c -eva -eva-show-progress program.c

# 生成报表
frama-c -eva -report program.c
```

### 分析示例

```c
// example.c
#include <stdio.h>

int main() {
    int x;
    scanf("%d", &x);

    int y;
    if (x > 0) {
        y = x * 2;
    } else {
        y = -x;
    }

    return y;
}
```

```bash
# 运行Eva分析
frama-c -eva example.c -eva-main main
```

**输出**:

```
[eva] Analyzing function main
[eva] example.c:6: starting to merge loop iterations
[eva] example.c:7:
  x ∈ [--..--]
[eva] example.c:11:
  y ∈ [0..--]  (即使x为负，y也为非负)
[eva] example.c:13:

esult ∈ [0..--]
```

### 警报解读

```c
void risky_function(int* p, int n) {
    // Eva可能报告:
    // [eva:alarm] risky.c:3: Warning:
    //   accessing p[0..n-1]. imprecise due to analysis limitations.
    //   (initialization)

    for (int i = 0; i < n; i++) {
        p[i] = i;  // 如果p未完全初始化，可能有警报
    }
}
```

### 提高精度

```bash
# 增加展开次数
frama-c -eva -eva-unroll-recursive-calls 10 program.c

# 增加slevel (分析分叉深度)
frama-c -eva -slevel 100 program.c

# 针对特定函数增加slevel
frama-c -eva -eva-slevel-function complex_func:1000 program.c
```

---

## WP插件 (演绎验证)

### 基础证明

```bash
# 生成证明义务
frama-c -wp program.c

# 使用特定证明器
frama-c -wp -wp-prover alt-ergo program.c
frama-c -wp -wp-prover z3 program.c
frama-c -wp -wp-prover cvc4 program.c

# 生成证明脚本
frama-c -wp -wp-script program.c
```

### 证明策略

```bash
# 全程自动证明
frama-c -wp -wp-rte program.c

# 交互式证明 (使用Coq)
frama-c -wp -wp-prover coq program.c

# 验证特定函数
frama-c -wp -wp-fct func1,func2 program.c
```

### 内存模型

```bash
# 选择内存模型
frama-c -wp -wp-model Hoare program.c      # 简单Hoare逻辑
frama-c -wp -wp-model Typed program.c      # 类型化内存
frama-c -wp -wp-model Separation program.c # 分离逻辑
```

---

## E-ACSL (运行时验证)

### 生成运行时检查

```bash
# 生成仪器化代码
frama-c -e-acsl -e-acsl-prepare annots.c -ocode instrumented.c

# 编译仪器化代码
gcc -c instrumented.c -I$(frama-c -print-share-path)/e-acsl

# 链接E-ACSL运行时
gcc instrumented.o -leacsl -o program
```

### 示例

```c
// annots.c
/*@
  @requires x != 0;
  @ensures \result == 100 / x;
*/
int divide100(int x);

int main() {
    int r1 = divide100(10);   // OK
    int r2 = divide100(0);    // 运行时错误!
    return 0;
}
```

---

## 工业应用案例

### 案例1: 航天软件验证 (来自2024指南)

```c
// 航天控制软件片段
/*@
  @requires \valid(cmd);
  @requires cmd->id >= 0 && cmd->id < MAX_COMMANDS;
  @requires is_valid_command(cmd);
  @assigns command_log[0..];
  @ensures execution_status(cmd->id) == EXECUTED;
  @behavior success:
    @assumes cmd->priority == HIGH;
    @ensures response_time < MAX_RESPONSE_HIGH;
  @behavior normal:
    @assumes cmd->priority != HIGH;
    @ensures response_time < MAX_RESPONSE_NORMAL;
  @complete behaviors;
  @disjoint behaviors;
*/
int execute_command(const Command* cmd);
```

**验证结果**:

- Eva: 证明无运行时错误
- WP: 证明功能正确性
- PathCrawler: 100%分支覆盖

### 案例2: 汽车AUTOSAR集成

```c
// AUTOSAR RTE接口
/*@
  @requires RTE_E_OK == status;
  @requires \valid(Read_data);
  @assigns *Read_data;
  @ensures \result == RTE_E_OK ==> \valid(Read_data);
  @ensures \result == RTE_E_NOK ==> *Read_data == \old(*Read_data);
*/
Std_ReturnType Rte_Read_SensorData(uint32* Read_data);
```

### 案例3: 三菱电机10年经验

根据2024年指南第10章，三菱电机研发欧洲中心(MERCE)的应用：

| 应用领域 | 使用的Frama-C插件 | 效果 |
|:---------|:-----------------|:-----|
| 工业代码验证 | Eva + WP | 发现200+潜在缺陷 |
| 自动测试生成 | PathCrawler | 80%测试覆盖率 |
| 防火墙验证 | Eva + Security | 证明无缓冲区溢出 |

---

## 高级技巧

### 1. 批量分析

```bash
# 分析整个项目
find src -name "*.c" | xargs frama-c -eva -save analyzed.sav

# 加载保存的分析结果
frama-c -load analyzed.sav -report
```

### 2. 自定义插件开发

```ocaml
(* my_plugin.ml - 简单的Frama-C插件 *)
open Cil_types
open Visitor

class print_funcs_visitor = object
  inherit nopCilVisitor

  method! vfunc (f: fundec) =
    Kernel.feedback "Function: %s" f.svar.vname;
    DoChildren
end

let run () =
  let visitor = new print_funcs_visitor in
  Visitor.visitFramacFile visitor (Ast.get())

let () = Db.Main.extend run
```

### 3. 与CI/CD集成

```yaml
# .github/workflows/frama-c.yml
name: Frama-C Verification

on: [push, pull_request]

jobs:
  verify:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v3

    - name: Install Frama-C
      run: |
        sudo apt-get update
        sudo apt-get install -y opam
        opam init --disable-sandboxing
        eval $(opam env)
        opam install -y frama-c

    - name: Run Eva
      run: |
        eval $(opam env)
        frama-c -eva src/*.c -then -report > eva-report.txt

    - name: Run WP
      run: |
        eval $(opam env)
        frama-c -wp -wp-rte src/*.c -wp-report wp-report.json

    - name: Check for alarms
      run: |
        if grep -q "\\[eva:alarm\\]" eva-report.txt; then
          echo "Eva found alarms!"
          exit 1
        fi
```

---

## 故障排除

### 常见问题

**Q: "cannot locate library stdio.h"**

```bash
# 指定头文件路径
frama-c -eva -cpp-extra-args="-I/usr/include" program.c

# 或使用预处理后的文件
gcc -E program.c -o program.i
frama-c -eva program.i
```

**Q: Eva分析超时**

```bash
# 增加超时时间
frama-c -eva -eva-timeout 300 program.c

# 或降低精度以提高速度
frama-c -eva -slevel 10 program.c
```

**Q: WP证明失败**

```bash
# 尝试不同的证明器
frama-c -wp -wp-prover alt-ergo,z3,cvc4 program.c

# 检查是否缺少循环不变量
frama-c -wp -wp-rte -warn-unsigned-overflow program.c
```

---

## 参考资源

### 官方资源

- [Frama-C官网](https://frama-c.com/)
- [ACSL语言参考](https://frama-c.com/download/acsl.pdf)
- [Eva用户手册](https://frama-c.com/download/eva-manual.pdf)
- [WP手册](https://frama-c.com/download/wp-manual.pdf)

### 2024年指南章节

1. 引言 - Frama-C概述
2. ACSL规范语言
3. Eva值分析
4. WP演绎验证
5. PathCrawler测试生成
6. E-ACSL运行时验证
7. 安全分析
8. 并发分析
9. 插件开发
10. 工业案例研究 (三菱电机等)

---

**上一章**: [03_Axiomatic_Semantics_Hoare.md](../00_Core_Semantics_Foundations/03_Axiomatic_Semantics_Hoare.md)
**实践案例**: [05_Practical_Verification_Cases/](../05_Practical_Verification_Cases/)

**最后更新**: 2026-03-18
**Frama-C版本**: 30.0 Zinc
