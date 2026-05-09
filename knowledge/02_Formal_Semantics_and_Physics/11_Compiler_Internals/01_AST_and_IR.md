# 编译器前端：AST与IR

> **层级定位**: 02 Formal Semantics and Physics / 11 Compiler Internals
> **难度级别**: L5
> **预估学习时间**: 12-15 小时

---

## 目录

- [编译器前端：AST与IR](#编译器前端ast与ir)
  - [目录](#目录)
  - [1. 编译器架构概览](#1-编译器架构概览)
    - [1.1 编译流程](#11-编译流程)
    - [1.2 主要组件](#12-主要组件)
  - [2. 抽象语法树(AST)](#2-抽象语法树ast)
    - [2.1 AST结构](#21-ast结构)
    - [2.2 Clang AST节点类型](#22-clang-ast节点类型)
    - [2.3 AST遍历示例](#23-ast遍历示例)
  - [3. 中间表示(IR)](#3-中间表示ir)
    - [3.1 LLVM IR概述](#31-llvm-ir概述)
    - [3.2 LLVM IR示例](#32-llvm-ir示例)
    - [3.3 IR指令类型](#33-ir指令类型)
  - [4. SSA形式](#4-ssa形式)
    - [4.1 为什么需要SSA](#41-为什么需要ssa)
    - [4.2 PHI节点](#42-phi节点)
    - [4.3 使用LLVM C++ API生成IR](#43-使用llvm-c-api生成ir)
  - [5. 优化Pass](#5-优化pass)
    - [5.1 Pass类型](#51-pass类型)
    - [5.2 常用优化Pass](#52-常用优化pass)
  - [关联导航](#关联导航)
    - [前置知识](#前置知识)
    - [后续延伸](#后续延伸)
    - [参考](#参考)

## 1. 编译器架构概览

### 1.1 编译流程

```
源代码                    前端                    优化器                  后端
   │                       │                       │                      │
   ▼                       ▼                       ▼                      ▼
┌─────────┐    词法分析    ┌─────────┐   语义分析   ┌─────────┐   代码生成   ┌─────────┐
│ .c文件   │ ──────────→ │  Tokens │ ─────────→ │   AST   │ ────────→ │   IR    │
└─────────┘              └─────────┘              └────┬────┘            └────┬────┘
                                                       │                       │
                                              中间代码生成                      │
                                              (LLVM IR/                        │
                                               GIMPLE等)                        │
                                                                                │
                                                           优化 passes          │
                                                           ┌─────┐              │
                                                           │IR优化│              │
                                                           └─────┘              │
                                                                                │
                                                                                ▼
                                                                         ┌─────────┐
                                                                         │ 汇编代码 │
                                                                         └─────────┘
```

### 1.2 主要组件

| 组件 | 输入 | 输出 | 代表实现 |
|:-----|:-----|:-----|:---------|
| **Lexer** | 字符流 | Token流 | Flex, Clang Lexer |
| **Parser** | Token流 | CST/Parse Tree | Bison, Recursive Descent |
| **AST Builder** | Parse Tree | AST | Clang AST |
| **Sema** | AST | 带类型的AST | Clang Sema |
| **IR Gen** | AST | IR | LLVM IR Gen, GCC GIMPLE |

---

## 2. 抽象语法树(AST)

### 2.1 AST结构

```c
// C代码
int add(int a, int b) {
    return a + b;
}

// AST表示 (简化)
FunctionDecl
├── Type: int (int(int, int))
├── Name: "add"
├── Params
│   ├── ParmDecl: int a
│   └── ParmDecl: int b
└── Body: CompoundStmt
    └── ReturnStmt
        └── BinaryOperator: +
            ├── DeclRefExpr: a
            └── DeclRefExpr: b
```

### 2.2 Clang AST节点类型

```cpp
// Clang AST核心节点
namespace clang {
    // 声明
    class Decl;
    class FunctionDecl : public Decl;
    class VarDecl : public Decl;
    class ParmVarDecl : public VarDecl;

    // 语句
    class Stmt;
    class CompoundStmt : public Stmt;
    class ReturnStmt : public Stmt;
    class IfStmt : public Stmt;
    class ForStmt : public Stmt;

    // 表达式
    class Expr : public Stmt;
    class BinaryOperator : public Expr;
    class UnaryOperator : public Expr;
    class DeclRefExpr : public Expr;
    class IntegerLiteral : public Expr;
    class CallExpr : public Expr;

    // 类型
    class QualType;
    class Type;
    class BuiltinType : public Type;
    class PointerType : public Type;
    class FunctionType : public Type;
}
```

### 2.3 AST遍历示例

```cpp
// Clang AST Visitor
#include "clang/AST/AST.h"
#include "clang/AST/RecursiveASTVisitor.h"

class FunctionVisitor : public RecursiveASTVisitor<FunctionVisitor> {
public:
    bool VisitFunctionDecl(FunctionDecl *FD) {
        // 获取函数名
        std::string name = FD->getNameAsString();

        // 获取返回类型
        QualType retType = FD->getReturnType();

        // 获取参数数量
        unsigned numParams = FD->getNumParams();

        llvm::outs() << "Function: " << name
                     << ", Returns: " << retType.getAsString()
                     << ", Params: " << numParams << "\n";

        return true;  // 继续遍历
    }
};
```

---

## 3. 中间表示(IR)

### 3.1 LLVM IR概述

```
LLVM IR特性：
- SSA (Static Single Assignment) 形式
- 三地址码
- 无限寄存器
- 强类型
- 平台无关
```

### 3.2 LLVM IR示例

```llvm
; C代码:
; int add(int a, int b) {
;     return a + b;
; }

; LLVM IR:
define i32 @add(i32 %a, i32 %b) {
entry:
    %sum = add i32 %a, %b
    ret i32 %sum
}

; C代码:
; int factorial(int n) {
;     if (n <= 1) return 1;
;     return n * factorial(n - 1);
; }

define i32 @factorial(i32 %n) {
entry:
    %cmp = icmp sle i32 %n, 1
    br i1 %cmp, label %return1, label %recurse

return1:
    ret i32 1

recurse:
    %sub = sub i32 %n, 1
    %call = call i32 @factorial(i32 %sub)
    %mul = mul i32 %n, %call
    ret i32 %mul
}
```

### 3.3 IR指令类型

| 类别 | 示例指令 | 说明 |
|:-----|:---------|:-----|
| **算术** | `add`, `sub`, `mul`, `udiv`, `sdiv` | 整数运算 |
| **位运算** | `and`, `or`, `xor`, `shl`, `lshr`, `ashr` | 位操作 |
| **内存** | `alloca`, `load`, `store`, `getelementptr` | 内存访问 |
| **比较** | `icmp`, `fcmp` | 整数/浮点比较 |
| **控制流** | `br`, `switch`, `ret` | 分支和返回 |
| **转换** | `trunc`, `zext`, `sext`, `bitcast` | 类型转换 |
| **调用** | `call`, `invoke`, `callbr` | 函数调用 |

---

## 4. SSA形式

### 4.1 为什么需要SSA

```
非SSA代码：           SSA形式：

x = 5                 x1 = 5
y = x + 1             y1 = x1 + 1
x = 10                x2 = 10
z = x + y             z1 = x2 + y1

SSA优势：
- 每个变量只赋值一次
- 简化数据流分析
- 方便优化实现
```

### 4.2 PHI节点

```llvm
; if-else 转换为 SSA
; if (cond) x = a; else x = b;
; return x;

entry:
    %cond = ...
    br i1 %cond, label %then, label %else

then:
    %x.then = add i32 %a, 0
    br label %merge

else:
    %x.else = add i32 %b, 0
    br label %merge

merge:
    ; PHI节点：根据来源选择值
    %x = phi i32 [%x.then, %then], [%x.else, %else]
    ret i32 %x
```

### 4.3 使用LLVM C++ API生成IR

```cpp
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

using namespace llvm;

// 创建模块
LLVMContext context;
std::unique_ptr<Module> module = std::make_unique<Module>("my_module", context);
IRBuilder<> builder(context);

// 创建函数类型：int(int, int)
Type *intType = Type::getInt32Ty(context);
FunctionType *funcType = FunctionType::get(intType, {intType, intType}, false);

// 创建函数
Function *func = Function::Create(funcType, Function::ExternalLinkage, "add", module.get());

// 创建基本块
BasicBlock *entry = BasicBlock::Create(context, "entry", func);
builder.SetInsertPoint(entry);

// 获取参数
auto args = func->args();
Value *a = args.begin();
Value *b = args.begin() + 1;

// 生成加法
Value *sum = builder.CreateAdd(a, b, "sum");

// 返回
builder.CreateRet(sum);

// 输出IR
module->print(outs(), nullptr);
```

---

## 5. 优化Pass

### 5.1 Pass类型

```cpp
// LLVM Pass框架
#include "llvm/IR/PassManager.h"

// 函数级Pass
class MyFunctionPass : public PassInfoMixin<MyFunctionPass> {
public:
    PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM) {
        // 对每个函数执行优化
        bool modified = false;

        for (BasicBlock &BB : F) {
            for (Instruction &I : BB) {
                // 分析/转换指令
                if (auto *binOp = dyn_cast<BinaryOperator>(&I)) {
                    // 优化：x + 0 → x
                    if (binOp->getOpcode() == Instruction::Add) {
                        Value *lhs = binOp->getOperand(0);
                        Value *rhs = binOp->getOperand(1);

                        if (auto *rhsConst = dyn_cast<ConstantInt>(rhs)) {
                            if (rhsConst->isZero()) {
                                binOp->replaceAllUsesWith(lhs);
                                binOp->eraseFromParent();
                                modified = true;
                            }
                        }
                    }
                }
            }
        }

        return modified ? PreservedAnalyses::none() : PreservedAnalyses::all();
    }
};
```

### 5.2 常用优化Pass

| Pass名称 | 功能 |
|:---------|:-----|
| **Mem2Reg** | 将内存提升为寄存器 (promote alloca) |
| **GVN** | 全局值编号 (消除冗余计算) |
| **SCCP** | 稀疏条件常量传播 |
| **Inlining** | 函数内联 |
| **Loop Unroll** | 循环展开 |
| **Dead Code Elimination** | 死代码删除 |
| **InstCombine** | 指令组合优化 |

---

## 关联导航

### 前置知识

- [编译过程概述](../README.md)
- [C语言文法](../../../01_Core_Knowledge_System/00_Introduction/02_C_Language_Grammar.md)

### 后续延伸

- [后端与代码生成](./02_Backend_Code_Generation.md)
- [优化技术](./03_Optimization_Techniques.md)

### 参考

- LLVM documentation: <https://llvm.org/docs/>
- "Engineering a Compiler" by Cooper & Torczon
