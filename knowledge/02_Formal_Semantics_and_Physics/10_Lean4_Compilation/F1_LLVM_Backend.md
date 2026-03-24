# LLVM后端：从Lean IR到机器码

> **层级**: L6 (元理论层)
> **引用**: LLVM文档, Lattner (2004), LLVM SSA形式
> **课程对标**: Stanford CS243, CMU 15-411

---

## 一、LLVM中间表示(IR)理论

### 1.1 SSA形式

**定义 1.1** (静态单赋值SSA)

程序在SSA形式中满足：

- 每个变量只被赋值一次
- 使用Φ-函数合并不同控制流的路径

**示例**：

```llvm
entry:
  %x0 = add i32 1, 2
  br label %loop

loop:
  %x1 = phi i32 [%x0, %entry], [%x2, %loop]
  %x2 = add i32 %x1, 1
  br i1 %cond, label %loop, label %exit
```

### 1.2 LLVM类型系统

**基础类型**：

- iN: N位整数 (i1, i8, i32, i64)
- float, double: 浮点
- pointer: 指针类型
- struct: 结构体
- array: 数组

**函数类型**：

```llvm
define i32 @add(i32 %a, i32 %b) {
  %sum = add i32 %a, %b
  ret i32 %sum
}
```

---

## 二、Lean IR到LLVM的 lowering

### 2.1 类型映射

| Lean IR类型 | LLVM类型 | 说明 |
|:------------|:---------|:-----|
| UInt8 | i8 | 直接映射 |
| UInt32 | i32 | 直接映射 |
| UInt64 | i64 | 直接映射 |
| Float | double | IEEE 754 |
| Object* | %lean_object* | 结构体指针 |
| Array α | { i64, [0 x %lean_object*] } | 大小+可变数组 |

### 2.2 控制流转换

**Lean IR**：

```
let x = if c then a else b
in f x
```

**LLVM IR**：

```llvm
  br i1 %c, label %then, label %else

then:
  br label %merge

else:
  br label %merge

merge:
  %x = phi %lean_object* [%a, %then], [%b, %else]
  call void @f(%lean_object* %x)
```

### 2.3 闭包在LLVM中的表示

**结构体定义**：

```llvm
%closure = type {
  void (%env*, ...)*,  ; 代码指针
  %env*               ; 环境指针
}

%env = type {
  %lean_object*,  ; 自由变量1
  %lean_object*   ; 自由变量2
  ; ...
}
```

**闭包调用**：

```llvm
  %code_ptr = getelementptr %closure, %closure* %clos, i32 0, i32 0
  %env_ptr = getelementptr %closure, %closure* %clos, i32 0, i32 1
  %code = load void (%env*, ...)*, void (%env*, ...)** %code_ptr
  call void %code(%env* %env_ptr, %lean_object* %arg)
```

---

## 三、LLVM优化pass

### 3.1 标准优化序列

**O1/O2/O3优化级别**：

```
- mem2reg: 内存到寄存器提升
- constprop: 常量传播
- inlining: 函数内联
- gvn: 全局值编号
- loop-unroll: 循环展开
- vectorize: 向量化
```

### 3.2 Lean特定优化

**引用计数优化**：

```llvm
; 原始代码
%obj1 = call %lean_object* @lean_alloc(...)
call void @lean_inc(%lean_object* %obj1)
call void @lean_dec(%lean_object* %obj1)
ret %lean_object* %obj1

; 优化后 (inc/dec抵消)
%obj1 = call %lean_object* @lean_alloc(...)
ret %lean_object* %obj1
```

**闭包内联**：
对于已知函数，直接内联而非通过闭包调用。

---

## 四、机器码生成

### 4.1 指令选择

**LLVM指令到x86-64**：

```llvm
%sum = add i64 %a, %b
```

→

```asm
addq %rbx, %rax
```

### 4.2 寄存器分配

**算法**：

- 线性扫描 (O1)
- 图着色 (O2/O3)

**调用约定**：

- 参数：RDI, RSI, RDX, RCX, R8, R9, XMM0-7
- 返回值：RAX, XMM0

### 4.3 代码发射

**目标文件格式**：

- ELF (Linux)
- Mach-O (macOS)
- COFF/PE (Windows)

---

## 五、与C后端对比

| 特性 | C后端 | LLVM后端 |
|:-----|:------|:---------|
| 可读性 | 高 | 低 |
| 优化 | 依赖C编译器 | LLVM优化 |
| 调试 | 易用 | 需要DWARF |
| 编译速度 | 中等 | 快(增量) |
| 平台支持 | 广 | 广 |

---

**参考文献**：

- LLVM Documentation (llvm.org/docs)
- Lattner, C. "LLVM: An Infrastructure for Multi-Stage Optimization" (2004)
