# LLVM IR 入门教程 (LLVM IR Tutorial)

> **层级定位**: 02 Formal Semantics and Physics / 12 Compiler Optimization
> **主题**: LLVM 中间表示语法、指令集与编程实践
> **难度级别**: L3 应用 → L4 分析
> **预估学习时间**: 8-12 小时
> **对标资料**: LLVM Kaleidoscope Tutorial / LLVM Language Reference Manual

## 概述

LLVM IR（Intermediate Representation）是 LLVM 编译器基础设施的核心。
它是一种低级的、与目标无关的虚拟指令集，同时具有足够的高级特性（如类型系统、SSA 形式、结构化控制流），使其既适合作为编译器前端的目标，又适合作为编译器后端的输入。
本教程系统阐述 LLVM IR 的语法、指令集和编程方法，对齐 LLVM 官方 Kaleidoscope 教程和 Language Reference Manual。

---

## 1. LLVM IR 核心特性

| 特性 | 说明 | 优势 |
|:-----|:-----|:-----|
| **SSA 形式** | 每个变量只赋值一次 | 简化数据流分析和优化 |
| **三地址码** | 每条指令最多一个结果、两个操作数 | 易于分析和转换 |
| **强类型** | 显式类型注解（无隐式转换） | 类型安全、错误检测 |
| **无限寄存器** | 使用 `%` 命名的虚拟寄存器 | 无需考虑物理寄存器限制 |
| **结构化控制流** | `br`, `ret`, 无 goto | 简化控制流分析 |

---

## 2. IR 格式基础

### 2.1 基本结构

```llvm
; LLVM IR 文件示例
source_filename = "test.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

; 全局变量
@global_var = global i32 42

; 函数定义
define i32 @add(i32 %a, i32 %b) {
entry:
  %sum = add i32 %a, %b
  ret i32 %sum
}
```

### 2.2 类型系统

```llvm
; 基本类型
i1        ; 1-bit 布尔值
i8        ; 8-bit 整数 (char)
i32       ; 32-bit 整数 (int)
i64       ; 64-bit 整数 (long)
float     ; 32-bit IEEE 浮点
double    ; 64-bit IEEE 浮点

; 复合类型
[10 x i32]           ; 10 个 i32 的数组
{i32, float}         ; 结构体
{i32, float}*        ; 结构体指针
void (i32, i32)*     ; 函数指针

; 向量类型
<4 x float>          ; 4 元素 float 向量 (SSE)
<8 x float>          ; 8 元素 float 向量 (AVX)
```

### 2.3 常量与全局值

```llvm
; 整数常量
i32 42
i64 -17

; 浮点常量
float 3.140000e+00
double 2.718281828459045

; 字符串常量 (以 null 结尾)
@str = private unnamed_addr constant [14 x i8] c"Hello, World!\00"

; 零初始化
zeroinitializer     ; 用于数组/结构体的 { 0, 0, ... }
```

---

## 3. 指令集详解

### 3.1 算术运算

```llvm
; 整数运算
%add  = add i32 %x, %y        ; x + y
%sub  = sub i32 %x, %y        ; x - y
%mul  = mul i32 %x, %y        ; x * y
%sdiv = sdiv i32 %x, %y       ; 有符号除法
%udiv = udiv i32 %x, %y       ; 无符号除法
%srem = srem i32 %x, %y       ; 有符号取模

; 浮点运算
%fadd = fadd float %a, %b
%fmul = fmul float %a, %b
%fdiv = fdiv float %a, %b

; 位运算
%and  = and i32 %x, %y        ; 按位与
%or   = or  i32 %x, %y        ; 按位或
%xor  = xor i32 %x, %y        ; 按位异或
%shl  = shl i32 %x, 2         ; 左移
%lshr = lshr i32 %x, 3        ; 逻辑右移
%ashr = ashr i32 %x, 3        ; 算术右移
```

### 3.2 内存访问

```llvm
; 栈分配
%ptr = alloca i32             ; 在栈上分配 i32

; 存储
store i32 42, ptr %ptr        ; *ptr = 42

; 加载
%val = load i32, ptr %ptr     ; val = *ptr

; 地址计算 (GEP = GetElementPtr)
; %arr 是 i32[10]* 类型
%elem_ptr = getelementptr [10 x i32], ptr %arr, i64 0, i64 5
; arr[5] 的地址
```

### 3.3 类型转换

```llvm
; 截断
%small = trunc i32 %big to i8        ; 32-bit → 8-bit

; 零扩展
%zbig  = zext i8 %small to i32       ; 无符号扩展

; 符号扩展
%sbig  = sext i8 %small to i32       ; 有符号扩展

; 浮点/整数转换
%fval  = sitofp i32 %ival to float   ; int → float
%ival  = fptosi float %fval to i32   ; float → int (有符号)

; 指针转换
%iptr  = ptrtoint ptr %ptr to i64    ; 指针 → 整数
%ptr2  = inttoptr i64 %iptr to ptr   ; 整数 → 指针

; 位类型转换 (bitcast)
%ival  = bitcast float %fval to i32  ; float 的位模式解释为 i32
```

### 3.4 比较与条件

```llvm
; 整数比较
%eq  = icmp eq  i32 %x, %y    ; x == y
%ne  = icmp ne  i32 %x, %y    ; x != y
%slt = icmp slt i32 %x, %y    ; 有符号 x < y
%ult = icmp ult i32 %x, %y    ; 无符号 x < y

; 浮点比较
%feq = fcmp oeq float %a, %b  ; ordered equal (非 NaN)
%flt = fcmp olt float %a, %b  ; ordered less than

; 选择
%max = select i1 %cond, i32 %x, i32 %y   ; cond ? x : y
```

### 3.5 控制流

```llvm
; 无条件跳转
br label %next

; 条件跳转
br i1 %cond, label %if_true, label %if_false

; 返回
ret i32 %val
ret void

;  switch
switch i32 %val, label %default [
  i32 0, label %case_0
  i32 1, label %case_1
  i32 2, label %case_2
]

; 函数调用
%result = call i32 @add(i32 %a, i32 %b)
```

### 3.6 PHI 节点 (SSA 关键)

```llvm
; PHI 节点: 根据控制流来源选择值
entry:
  br i1 %cond, label %loop, label %exit

loop:
  %i = phi i32 [ 0, %entry ], [ %next_i, %loop ]
  %next_i = add i32 %i, 1
  %done = icmp sge i32 %next_i, 10
  br i1 %done, label %exit, label %loop

exit:
  %final = phi i32 [ 0, %entry ], [ %next_i, %loop ]
  ret i32 %final
```

---

## 4. 完整示例：C → LLVM IR

### C 源代码

```c
int factorial(int n) {
    int result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}
```

### 生成的 LLVM IR

```llvm
define i32 @factorial(i32 %n) {
entry:
  %cmp_init = icmp slt i32 %n, 2
  br i1 %cmp_init, label %return, label %loop

loop:
  %i = phi i32 [ 2, %entry ], [ %i_next, %loop ]
  %result = phi i32 [ 1, %entry ], [ %result_next, %loop ]
  %result_next = mul i32 %result, %i
  %i_next = add i32 %i, 1
  %cmp = icmp sle i32 %i_next, %n
  br i1 %cmp, label %loop, label %return

return:
  %ret_val = phi i32 [ 1, %entry ], [ %result_next, %loop ]
  ret i32 %ret_val
}
```

---

## 5. 工具链实践

### 5.1 生成 LLVM IR

```bash
# 从 C 生成 LLVM IR
gcc -S -emit-llvm -O2 program.c -o program.ll

# 或使用 Clang
clang -S -emit-llvm -O2 program.c -o program.ll

# 生成可读性更好的 IR (无优化)
clang -S -emit-llvm -O0 -Xclang -disable-O0-optnone program.c -o program.ll
```

### 5.2 IR 优化

```bash
# 运行优化遍
opt -passes="mem2reg,instcombine,simplifycfg,gvn" input.ll -o optimized.ll

# 查看应用的优化
opt -passes="print-changed" input.ll -o /dev/null
```

### 5.3 IR → 汇编/目标文件

```bash
# IR → 汇编
llc program.ll -o program.s

# IR → 目标文件
llc -filetype=obj program.ll -o program.o

# 直接链接执行
clang program.ll -o program
```

### 5.4 JIT 编译 (使用 LLVM ORC JIT)

```cpp
#include "llvm/ExecutionEngine/Orc/LLJIT.h"

std::unique_ptr<llvm::orc::LLJIT> JIT;
llvm::orc::LLJITBuilder().create(JIT);

// 加载 IR 模块并执行
auto M = llvm::parseIR(...);
JIT->addIRModule(std::move(M));
auto Entry = JIT->lookup("main");
int (*Main)() = Entry->toPtr<int()>();
int Result = Main();
```

---

## 6. 编写自定义 Pass

```cpp
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"

struct MyPass : public llvm::PassInfoMixin<MyPass> {
    llvm::PreservedAnalyses run(llvm::Function &F,
                                 llvm::FunctionAnalysisManager &) {
        for (auto &BB : F) {
            for (auto &I : BB) {
                if (auto *Add = dyn_cast<llvm::BinaryOperator>(&I)) {
                    if (Add->getOpcode() == llvm::Instruction::Add) {
                        // 将所有 x + 0 替换为 x
                        if (auto *Const = dyn_cast<llvm::ConstantInt>(
                                Add->getOperand(1))) {
                            if (Const->isZero()) {
                                Add->replaceAllUsesWith(Add->getOperand(0));
                                Add->eraseFromParent();
                            }
                        }
                    }
                }
            }
        }
        return llvm::PreservedAnalyses::none();
    }
};
```

---

## 权威参考

- **LLVM Language Reference Manual**: <https://llvm.org/docs/LangRef.html>
- **LLVM Kaleidoscope Tutorial**: <https://llvm.org/docs/tutorial/>
- **Writing an LLVM Pass**: <https://llvm.org/docs/WritingAnLLVMPass.html>
- **LLVM IR Cheat Sheet**: <https://github.com/llvm/llvm-project/blob/main/llvm/docs/GettingStartedTutorials.rst>

## 待补充内容

- [ ] LLVM IR 中的异常处理 (`landingpad`, `invoke`)
- [ ] 内联汇编在 LLVM IR 中的表示
- [ ] Debug 信息生成 (`DIBuilder`)
- [ ] MLIR 简介：LLVM IR 的下一代基础设施

---

*最后更新: 2026-05-10*
