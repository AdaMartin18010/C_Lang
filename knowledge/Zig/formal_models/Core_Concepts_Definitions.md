# Zig 核心概念定义与原理模型

## 1. 类型系统理论

### 1.1 类型定义

```text
定义 1.1 (Zig 类型)
Zig 中的类型 τ 定义为以下之一:

τ ::=
    | ι           (标量类型)
    | [N]τ        (数组类型，N 为编译时常量)
    | []τ         (切片类型)
    | *τ          (单元素指针)
    | [*]τ        (多元素指针)
    | ?τ          (可选类型)
    | E!τ         (错误联合类型，E 为错误集)
    | struct {f₁:τ₁, ..., fₙ:τₙ}  (结构体)
    | union(E) {f₁:τ₁, ..., fₙ:τₙ} (联合类型)
    | fn(τ₁,...,τₙ) τᵣ            (函数类型)
    | type                        (类型本身)

其中:
- ι ∈ {bool, void, noreturn, i8, i16, ..., u8, u16, ..., f16, f32, ...}
- N ∈ 编译时常量正整数
- E 为错误集
```

### 1.2 类型规则

```text
公理 1.1 (子类型关系)
1. τ <: τ (自反性)
2. if τ₁ <: τ₂ and τ₂ <: τ₃ then τ₁ <: τ₃ (传递性)
3. []const τ <: []τ (协变)
4. *const τ <: *τ (协变)
5. comptime_int <: iN, uN, fN (自动转换)

定理 1.1 (类型安全)
若表达式 e 在类型环境 Γ 下有类型 τ (记作 Γ ⊢ e : τ),
则 e 在求值过程中不会出现类型错误。

证明概要:
通过对 e 的结构归纳，验证每个语法构造的类型规则保持安全性。
```

---

## 2. 内存模型原理

### 2.1 内存区域定义

```
定义 2.1 (内存区域)
程序地址空间划分为以下不相交区域:

Memory = Code ∪ StaticData ∪ Heap ∪ Stack ∪ Unused

其中:
- Code: 机器指令，只读
- StaticData = Data ∪ BSS ∪ ROData
  - Data: 已初始化可写全局数据
  - BSS: 未初始化零填充全局数据
  - ROData: 只读数据(字符串字面量等)
- Heap: 动态分配内存，向上增长
- Stack: 函数调用帧，向下增长
- Unused: 不可访问区域(包括 null 页)
```

### 2.2 指针语义

```
定义 2.2 (指针)
指针 p: *τ 是一个地址值 a，满足:

1. 有效性: a ∈ ValidAddresses(τ)
2. 对齐: a mod align(τ) = 0
3. 非空: a ≠ 0 (除非标记 allowzero)
4. 可变性: *p 可写当且仅当 p: *τ (非 *const τ)

定义 2.3 (切片)
切片 s: []τ 是一个 (ptr, len) 对，满足:

1. ptr: [*]τ 指向连续内存
2. len: usize 为元素数量
3. ∀i ∈ [0, len). valid(ptr + i * sizeof(τ))
4. 内存区域: [ptr, ptr + len * sizeof(τ)) ⊆ Heap ∨ Stack
```

### 2.3 生命周期模型

```
定义 2.4 (变量生命周期)
对于变量声明 var x: τ = init:

lifetime(x) = [begin_decl, end_scope)

其中:
- begin_decl: 声明执行时刻
- end_scope: 包含声明的作用域结束时刻

规则 2.1 (栈变量)
若 x 为栈变量，则:
1. lifetime(x) 结束后，&x 变为无效
2. x 的存储在 end_scope 自动释放

规则 2.2 (堆变量)
若 x 为堆分配 (allocator.create)，则:
1. lifetime(x) 与分配器关联
2. 必须显式调用 allocator.destroy(x)
3. defer 确保在 scope 结束时释放
```

---

## 3. 错误处理模型

### 3.1 错误集代数

```
定义 3.1 (错误集)
错误集 E 是错误标签的有限集合:
E = {e₁, e₂, ..., eₙ}

操作:
1. 并集: E₁ ∪ E₂ = E₁ || E₂
2. 子集: E₁ ⊆ E₂ ⇔ ∀e ∈ E₁. e ∈ E₂
3. anyerror = ⋃{E | E 是合法错误集}

定义 3.2 (错误联合类型)
E!τ 表示值类型为:
- 错误值 e ∈ E，或
- 成功值 v: τ

内存表示: tag + payload (tag 指示是错误还是成功)
```

### 3.2 传播规则

```
规则 3.1 (try 传播)
若 e: E!τ，则 try e: τ，且可能传播错误。

类型规则:
Γ ⊢ e : E!τ    Γ, x:τ ⊢ e' : τ'
───────────────────────────────── (try-success)
Γ ⊢ (try e) : τ

Γ ⊢ e : E!τ    Γ ⊢ handle: E → τ'
───────────────────────────────── (try-error)
Γ ⊢ (try e catch handle) : τ ∪ τ'

规则 3.2 (errdefer)
errdefer s 在包含作用域因错误退出时执行 s。

语义:
scope_exit_with_error() {
    execute(errdefer_chain);
}
```

---

## 4. 编译时求值模型

### 4.1 Comptime 语义

```
定义 4.1 (编译时值)
值 v 是编译时值当且仅当:
1. v 的字面量可直接求值，或
2. v 是仅依赖编译时值的表达式结果，或
3. v 由 comptime 函数返回

定义 4.2 (Comptime 函数)
函数 f 是 comptime 函数当且仅当:
1. 标记为 comptime fn，或
2. 参数类型包含 comptime 标记，或
3. 返回类型需要编译时求值

定理 4.1 (编译时求值终止性)
所有 comptime 计算在编译期间终止。

保证:
- 禁止无限循环
- 限制递归深度
- 限制计算复杂度
```

### 4.2 类型生成

```
定义 4.3 (类型构造)
@Type(info: std.builtin.Type) → type

构造规则:
1. 根据 info 的 tag 确定类型类别
2. 递归构造字段/元素类型
3. 验证类型合法性
4. 返回唯一类型标识

性质:
- 幂等: @Type(@typeInfo(T)) ≡ T
- 单射: T₁ ≠ T₂ ⇒ @typeInfo(T₁) ≠ @typeInfo(T₂)
```

---

## 5. 并发内存模型

### 5.1 原子操作语义

```
定义 5.1 (内存序)
内存序 mo ∈ {Monotonic, Acquire, Release, AcqRel, SeqCst}

含义:
- Monotonic: 仅保证原子性，无顺序保证
- Acquire: 此操作后的读写不会重排到此操作前
- Release: 此操作前的读写不会重排到此操作后
- AcqRel: 同时具有 Acquire 和 Release 语义
- SeqCst: 顺序一致性，全局有序

规则 5.1 (Acquire-Release 同步)
线程 A 执行 store(x, v, Release)
线程 B 执行 load(x, Acquire)
若 B 读到 v，则 A 在 store 前的所有写入对 B 可见。

形式化:
store_A(x, v, Release) <_hb load_B(x, v, Acquire)
⇒ ∀w. w <_A store_A(x) ⇒ w <_B load_B(x)
```

### 5.2 数据竞争自由

```
定义 5.2 (数据竞争)
程序 P 存在数据竞争当且仅当:
∃两个非原子访问 m₁, m₂:
- m₁ 和 m₂ 访问同一内存位置
- m₁ 和 m₂ 至少有一个是写
- m₁ 和 m₂ 无 happens-before 关系

定理 5.1 (Zig 安全保证)
使用原子操作和 Mutex 的程序是无数据竞争的。

证明:
- 原子操作提供顺序一致性
- Mutex 的 lock/unlock 构成 happens-before 边
- 所有共享内存访问通过同步原语保护
```

---

## 6. 模块系统模型

### 6.1 模块定义

```
定义 6.1 (模块)
模块 M = (exports, imports, decls)

其中:
- exports: 公开声明集合 {pub decl}
- imports: 导入模块集合 {@import("path")}
- decls: 所有声明集合

定义 6.2 (可见性)
声明 d 在模块 M 中可见当且仅当:
1. d 标记为 pub，或
2. d 与引用在同一模块
```

### 6.2 导入解析

```
规则 6.1 (导入解析)
@import(path) 的解析顺序:

1. 若 path 以 @ 开头: 编译器内置
2. 若 path 为相对路径: 相对当前文件
3. 若 path 为包名: 查找 build.zig.zon 依赖
4. 若 path 以 std 开头: 标准库

形式化:
resolve(path, current_file) =
    case path of
        "@builtin" → builtin_module
        "./" ++ _ | "../" ++ _ → relative(path, current_file)
        name → package(name) <| std_module(name)
```

---

## 7. 构建系统模型

### 7.1 构建图

```
定义 7.1 (构建图)
构建图 G = (V, E, root)

其中:
- V: 构建步骤集合 {compile, link, install, run, ...}
- E: 依赖边集合 {(v₁, v₂) | v₁ 依赖 v₂}
- root: 根步骤

性质:
- G 必须是无环有向图 (DAG)
- 拓扑排序给出执行顺序
```

### 7.2 增量构建

```
定义 7.2 (缓存键)
步骤 s 的缓存键 K(s) = hash(inputs, command, env)

增量规则:
若缓存中存在 K(s) 的结果且输入未改变，
则跳过 s 的执行。

正确性:
∀s. cache[K(s)] = result(s) ⇒ deterministic(s)
```

---

## 8. 安全属性形式化

### 8.1 内存安全

```
定义 8.1 (内存安全)
程序 P 是内存安全的当且仅当:

1. 无空指针解引用
   ∀p. p ≠ null ⇒ valid(p)

2. 无悬垂指针
   ∀p. lifetime(pointee(p)) 未结束

3. 无缓冲区溢出
   ∀slice s. ∀i. 0 ≤ i < s.len ⇒ valid(s.ptr[i])

4. 无 use-after-free
   ∀p. dealloc(p) ⇒ ¬access(p)

Zig 保证:
- Debug 模式: 运行时检查检测上述错误
- ReleaseSafe: 安全检查保留
- 安全抽象: Allocator, 切片边界检查
```

### 8.2 类型安全

```
定理 8.1 (类型安全)
类型正确的 Zig 程序不会出现:
- 类型混淆
- 未定义行为 (由类型错误引起)
- 非法转换

形式化:
Γ ⊢ P : ok ⇒ ¬∃e ∈ P. type_error(e)
```

---

> **文档类型**: 形式化定义与原理
> **用途**: 理论基础、安全证明
> **更新**: 2026-03-12
