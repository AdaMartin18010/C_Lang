# 线性逻辑与资源敏感计算

## 目录

- [线性逻辑与资源敏感计算](#线性逻辑与资源敏感计算)
  - [目录](#目录)
  - [1. 逻辑系统对比概述](#1-逻辑系统对比概述)
    - [1.1 从经典逻辑到线性逻辑](#11-从经典逻辑到线性逻辑)
    - [1.2 结构规则的对比](#12-结构规则的对比)
    - [1.3 资源解释示例](#13-资源解释示例)
  - [2. 线性逻辑基础](#2-线性逻辑基础)
    - [2.1 连接词分类](#21-连接词分类)
    - [2.2 乘法性连接词](#22-乘法性连接词)
    - [2.3 加法性连接词](#23-加法性连接词)
    - [2.4 示例：咖啡机协议](#24-示例咖啡机协议)
  - [3. 线性蕴涵与线性否定](#3-线性蕴涵与线性否定)
    - [3.1 线性蕴涵](#31-线性蕴涵)
    - [3.2 资源转换视角](#32-资源转换视角)
    - [3.3 线性否定](#33-线性否定)
    - [3.4 线性蕴涵的编程示例](#34-线性蕴涵的编程示例)
  - [4. 模态算子 !A 和 ?A](#4-模态算子-a-和-a)
    - [4.1 指数模态](#41-指数模态)
    - [4.2 !A (Of Course) 的规则](#42-a-of-course-的规则)
    - [4.3 ?A (Why Not) 的规则](#43-a-why-not-的规则)
    - [4.4 模态的 Curry-Howard 对应](#44-模态的-curry-howard-对应)
    - [4.5 Rust 中的模态对应](#45-rust-中的模态对应)
  - [5. 在编程语言中的应用](#5-在编程语言中的应用)
    - [5.1 Rust 所有权系统](#51-rust-所有权系统)
    - [5.2 所有权转移示例](#52-所有权转移示例)
    - [5.3 RAII 与线性逻辑](#53-raii-与线性逻辑)
  - [6. 会话类型](#6-会话类型)
    - [6.1 会话类型概述](#61-会话类型概述)
    - [6.2 客户-服务器协议示例](#62-客户-服务器协议示例)
    - [6.3 带选择的协议](#63-带选择的协议)
    - [6.4 Rust 中的会话类型实现](#64-rust-中的会话类型实现)
  - [7. 资源敏感的类型系统](#7-资源敏感的类型系统)
    - [7.1 资源类型系统原理](#71-资源类型系统原理)
    - [7.2 类型系统分类](#72-类型系统分类)
    - [7.3 效果系统与线性类型](#73-效果系统与线性类型)
    - [7.4 内存管理的类型安全](#74-内存管理的类型安全)
  - [8. 形式化推理与证明](#8-形式化推理与证明)
    - [8.1 相继式演算](#81-相继式演算)
    - [8.2 证明标准化](#82-证明标准化)
    - [8.3 计算解释](#83-计算解释)
  - [9. 参考与延伸阅读](#9-参考与延伸阅读)
    - [经典文献](#经典文献)
    - [相关文件](#相关文件)
    - [总结](#总结)

---

## 1. 逻辑系统对比概述

### 1.1 从经典逻辑到线性逻辑

线性逻辑（Linear Logic）由 Jean-Yves Girard 于 1987 年提出，是经典逻辑和直觉主义逻辑的重要扩展。
它引入了对**资源敏感性**的显式处理。

| 逻辑系统 | 核心特征 | 蕴涵解释 | 资源观点 |
|---------|---------|---------|---------|
| **经典逻辑** | 真值永恒、排中律成立 | $A \to B$：如果A真则B真 | 无资源概念 |
| **直觉主义逻辑** | 构造性证明、无排中律 | $A \to B$：A的证明转换为B的证明 | 隐含资源概念 |
| **线性逻辑** | 资源敏感、收缩/弱化受限 | $A \multimap B$：消耗A产生B | 显式资源管理 |

### 1.2 结构规则的对比

在相继式演算（Sequent Calculus）中，线性逻辑限制了经典逻辑中的结构规则：

```
┌────────────────────────────────────────────────────────────────┐
│                    结构规则对比                                 │
├────────────────────────────────────────────────────────────────┤
│  弱化 (Weakening)                                              │
│    经典:  Γ ⊢ Δ  ⇒  Γ, A ⊢ Δ    (可以任意添加假设)              │
│    线性:  禁止无限制弱化                                        │
│                                                                │
│  收缩 (Contraction)                                            │
│    经典:  Γ, A, A ⊢ Δ  ⇒  Γ, A ⊢ Δ  (假设可无限复制)            │
│    线性:  禁止无限制收缩                                        │
│                                                                │
│  交换 (Exchange)                                               │
│    经典与线性:  都允许假设重排                                  │
└────────────────────────────────────────────────────────────────┘
```

### 1.3 资源解释示例

```
经典逻辑推导:
  从 "我有 1 元钱" 推出 "我有 1 元钱 且 我有 1 元钱"
  (通过收缩规则，同一命题可被多次使用)

线性逻辑推导:
  $1 不能凭空变成 $1 ⊗ $1
  每个资源必须被精确地消耗一次
```

---

## 2. 线性逻辑基础

### 2.1 连接词分类

线性逻辑将连接词分为两类：**乘法性**（multiplicative）和**加法性**（additive）：

| 类别 | 合取 | 析取 | 单位元 | 含义 |
|-----|------|------|--------|------|
| **乘法性** | $A \otimes B$ | $A ⅋ B$ | $1$ / $⊥$ | 同时使用所有资源 |
| **加法性** | $A \& B$ | $A \oplus B$ | $\top$ / $0$ | 选择使用资源 |

### 2.2 乘法性连接词

```
张量 (Tensor) - ⊗:
  A ⊗ B 表示 "同时拥有 A 和 B"

  证明规则:
  ────────────────────────── (⊗R)
  Γ ⊢ A    Δ ⊢ B
  ──────────────────────────
  Γ, Δ ⊢ A ⊗ B

  资源解释: 左侧资源被分割为两部分，分别证明A和B

抛物线 (Par) - ⅋:
  A ⅋ B 表示 "如果不消耗 A 就消耗 B"

  证明规则:
  Γ, A, B ⊢ Δ
  ────────────────────────── (⅋L)
  Γ, A ⅋ B ⊢ Δ
```

### 2.3 加法性连接词

```
与 (With) - &:
  A & B 表示 "可选择 A 或 B"

  证明规则:
  Γ ⊢ A    Γ ⊢ B
  ────────────────────────── (&R)
  Γ ⊢ A & B

  资源解释: 同一资源集可以分别证明A和B，但运行时只能选其一

加 (Plus) - ⊕:
  A ⊕ B 表示 "A 或 B"

  证明规则:
  Γ ⊢ A
  ────────────────────────── (⊕R₁)
  Γ ⊢ A ⊕ B

  Γ ⊢ B
  ────────────────────────── (⊕R₂)
  Γ ⊢ A ⊕ B
```

### 2.4 示例：咖啡机协议

```
线性逻辑描述的咖啡机:

┌─────────────────────────────────────────────────────────────┐
│  Coin ⊗ (Coffee ⊕ Tea) ⊸ (Cup ⊗ (HotDrink ⅋ Change))       │
│                                                             │
│  解释:                                                      │
│  - 投入硬币 (Coin) 和 选择 (Coffee 或 Tea)                  │
│  - 获得杯子 (Cup) 和 热饮与找零的并行结果                   │
└─────────────────────────────────────────────────────────────┘
```

---

## 3. 线性蕴涵与线性否定

### 3.1 线性蕴涵

线性蕴涵 $A \multimap B$（读作 "A lollipop B"）是线性逻辑的核心：

```
定义: A ⊸ B 等价于 A^⊥ ⅋ B

证明规则:
Γ, A ⊢ B
────────────────────────── (⊸R)
Γ ⊢ A ⊸ B

Γ ⊢ A    Δ, B ⊢ C
────────────────────────── (⊸L)
Γ, Δ, A ⊸ B ⊢ C
```

### 3.2 资源转换视角

```
A ⊸ B 的资源解释:

  输入: 1个 A 类型的资源
  输出: 1个 B 类型的资源

  线性函数 f : A ⊸ B

  f(x) 恰好消耗 x 一次，产生一个 B

对比经典函数:
  g : A → B
  g(x) 可以读取 x 零次、一次或多次
```

### 3.3 线性否定

线性否定 $A^⊥$ 表示对偶资源（债务/债权的对偶）：

```
双重否定消去:
  (A^⊥)^⊥ ≡ A

德摩根对偶:
  (A ⊗ B)^⊥ ≡ A^⊥ ⅋ B^⊥
  (A ⅋ B)^⊥ ≡ A^⊥ ⊗ B^⊥
  (A & B)^⊥ ≡ A^⊥ ⊕ B^⊥
  (A ⊕ B)^⊥ ≡ A^⊥ & B^⊥

  ⊤^⊥ ≡ 0
  1^⊥ ≡ ⊥
```

### 3.4 线性蕴涵的编程示例

```rust
// Rust 中的线性类型示例

// String 具有线性性质：只能被移动，不能被隐式复制
fn consume_string(s: String) -> usize {
    s.len()  // s 被消耗，返回长度
}  // s 在这里被释放

fn main() {
    let s = String::from("hello");

    let len = consume_string(s);
    // println!("{}", s);  // 编译错误：s 已被移动

    println!("长度: {}", len);
}

// 线性函数的显式类型签名（概念上）
// concat : String ⊸ String ⊸ String
fn concat(a: String, b: String) -> String {
    format!("{}{}", a, b)  // 消耗 a 和 b，产生新 String
}
```

---

## 4. 模态算子 !A 和 ?A

### 4.1 指数模态

线性逻辑引入两个模态算子来恢复经典逻辑的表达能力：

| 模态 | 名称 | 含义 | 资源解释 |
|-----|------|------|---------|
| **!A** | Of Course / Bang | 可任意复制的资源 | 存储在内存中的持久数据 |
| **?A** | Why Not | 可任意丢弃的资源 | 可能成功也可能失败的计算 |

### 4.2 !A (Of Course) 的规则

```
推导规则:

!Γ ⊢ B        !Γ ⊢ ?Δ, A
────────────   ──────────── (!R)
!Γ ⊢ !B        !Γ ⊢ ?Δ, !A

弱化:  Γ ⊢ Δ          收缩:  Γ, !A, !A ⊢ Δ
───────────── (!W)    ───────────────────── (!C)
Γ, !A ⊢ Δ             Γ, !A ⊢ Δ

对偶关系:  (!A)^⊥ ≡ ?(A^⊥)
```

### 4.3 ?A (Why Not) 的规则

```
推导规则:

Γ, A ⊢ ?Δ
────────── (?L)
Γ, ?A ⊢ ?Δ

弱化:  Γ ⊢ Δ          收缩:  Γ ⊢ ?A, ?A, Δ
──────────── (?W)     ───────────────────── (?C)
Γ ⊢ ?A, Δ             Γ ⊢ ?A, Δ
```

### 4.4 模态的 Curry-Howard 对应

```
┌─────────────────────────────────────────────────────────────────┐
│              ! 和 ? 的编程语言对应                               │
├─────────────────────────────────────────────────────────────────┤
│  !A 对应:                                                       │
│    - 可复制类型 (Copy type)                                     │
│    - 共享引用 (&T)                                              │
│    - 垃圾回收堆中的持久对象                                     │
│                                                                 │
│  ?A 对应:                                                       │
│    - 可选类型 (Option)                                          │
│    - 异常/失败可能性                                            │
│    - 非确定性计算                                               │
└─────────────────────────────────────────────────────────────────┘
```

### 4.5 Rust 中的模态对应

```rust
// !A - 可复制类型 (Copy trait)
#[derive(Clone, Copy)]
struct Point { x: i32, y: i32 }

fn use_point(p: Point) -> Point {
    // p 被复制而不是移动
    p
}

fn main() {
    let p = Point { x: 1, y: 2 };
    let p2 = use_point(p);
    let p3 = use_point(p);  // 可以再次使用 p（Copy）

    // String 不是 Copy，具有线性性质
    let s = String::from("hello");
    let s2 = s;  // s 被移动
    // let s3 = s;  // 编译错误
}
```

---

## 5. 在编程语言中的应用

### 5.1 Rust 所有权系统

Rust 的类型系统是线性逻辑在工业级编程语言中的最直接应用：

```
Rust 所有权规则 vs 线性逻辑:

┌─────────────────────────────────────────────────────────────────┐
│  Rust 规则              │  线性逻辑对应                        │
├─────────────────────────────────────────────────────────────────┤
│  每个值有唯一所有者      │  A （线性资源）                      │
│  所有者离开作用域时释放  │  资源消耗                            │
│  move 语义              │  ⊗ (张量积)                          │
│  &mut T 可变借用         │  对资源的暂时授权                    │
│  &T 共享借用             │  !A (可复制的引用)                   │
│  Drop trait             │  资源释放协议                        │
└─────────────────────────────────────────────────────────────────┘
```

### 5.2 所有权转移示例

```rust
// 线性类型系统的实际应用

struct FileHandle {
    fd: i32,
}

impl FileHandle {
    // new : () ⊸ FileHandle
    fn new(path: &str) -> Option<FileHandle> {
        // 打开文件...
        Some(FileHandle { fd: 42 })
    }

    // read : FileHandle ⊸ (FileHandle ⊗ Vec<u8>)
    fn read(self) -> (FileHandle, Vec<u8>) {
        // 读取数据...
        (self, vec![1, 2, 3])
    }

    // close : FileHandle ⊸ ()
    fn close(self) {
        // 关闭文件描述符...
        println!("文件 {} 已关闭", self.fd);
    }
}

fn process_file() {
    if let Some(handle) = FileHandle::new("data.txt") {
        let (handle, data) = handle.read();
        println!("读取 {} 字节", data.len());
        handle.close();  // 必须调用，否则编译错误
    }
}
```

### 5.3 RAII 与线性逻辑

```cpp
// C++ RAII 的线性逻辑解释

class LinearResource {
public:
    // 构造函数：创建资源
    LinearResource() : data(new int[1000]) {
        std::cout << "资源创建\n";
    }

    // 禁用拷贝（线性性质）
    LinearResource(const LinearResource&) = delete;
    LinearResource& operator=(const LinearResource&) = delete;

    // 允许移动（所有权转移）
    LinearResource(LinearResource&& other) noexcept
        : data(other.data) {
        other.data = nullptr;
        std::cout << "资源移动\n";
    }

    // 析构函数：资源消耗
    ~LinearResource() {
        delete[] data;
        std::cout << "资源释放\n";
    }

private:
    int* data;
};

// 使用模式
void use_linear() {
    LinearResource r1;           // 创建
    LinearResource r2 = std::move(r1);  // 所有权转移
}  // r2 被释放，r1 已空
```

---

## 6. 会话类型

### 6.1 会话类型概述

会话类型（Session Types）是线性逻辑在进程演算中的应用，用于描述通信协议：

```
会话类型语法:

  S, T ::= !A.S      (发送 A 后继续 S)
         | ?A.S      (接收 A 后继续 S)
         | S ⊕ T     (选择：左分支 S 或右分支 T)
         | S & T     (提供：左分支 S 或右分支 T)
         | end       (会话结束)
         | rec X.S   (递归类型)
         | X         (类型变量)
```

### 6.2 客户-服务器协议示例

```
服务器类型 (提供加法服务):
  Server = ?Int.?Int.!Int.end

  解释:
  - ?Int: 接收第一个整数
  - ?Int: 接收第二个整数
  - !Int: 发送结果
  - end:  会话结束

客户端类型 (使用加法服务):
  Client = !Int.!Int.?Int.end

  解释:
  - !Int: 发送第一个整数
  - !Int: 发送第二个整数
  - ?Int: 接收结果
  - end:  会话结束

对偶关系: Client = Server^⊥
```

### 6.3 带选择的协议

```
文件服务协议:

  Server = &{
             Read:  ?Filename.!(File ⊕ Error).end,
             Write: ?Filename.?Content.!(Ok ⊕ Error).end,
             Close: end
           }

  Client = ⊕{
             Read:  !Filename.?(File ⊕ Error).end,
             Write: !Filename.!Content.?(Ok ⊕ Error).end,
             Close: end
           }

解释:
  &{} 表示服务器提供多个选项供客户端选择
  ⊕{} 表示客户端选择一个选项执行
```

### 6.4 Rust 中的会话类型实现

```rust
// 使用类型状态模式实现会话类型

// 定义类型级别的状态
struct Init;
struct Ready;
struct Closed;

// 文件会话通道
struct FileChannel<State> {
    socket: TcpStream,
    _state: PhantomData<State>,
}

impl FileChannel<Init> {
    // new : () ⊸ FileChannel<Init>
    fn new(addr: &str) -> io::Result<FileChannel<Init>> {
        let socket = TcpStream::connect(addr)?;
        Ok(FileChannel { socket, _state: PhantomData })
    }

    // authenticate : FileChannel<Init> ⊸ FileChannel<Ready>
    fn authenticate(mut self, token: &str) -> io::Result<FileChannel<Ready>> {
        self.socket.write_all(token.as_bytes())?;
        Ok(FileChannel { socket: self.socket, _state: PhantomData })
    }
}

impl FileChannel<Ready> {
    // read_file : FileChannel<Ready> ⊸ (FileChannel<Ready> ⊸ Vec<u8>)
    fn read_file(mut self, path: &str) -> io::Result<(FileChannel<Ready>, Vec<u8>)> {
        self.socket.write_all(path.as_bytes())?;
        let mut buffer = vec![];
        self.socket.read_to_end(&mut buffer)?;
        Ok((FileChannel { socket: self.socket, _state: PhantomData }, buffer))
    }

    // close : FileChannel<Ready> ⊸ ()
    fn close(self) {
        // 发送关闭消息并关闭连接
        drop(self.socket);
    }
}

// 使用示例
fn use_session() -> io::Result<()> {
    let channel = FileChannel::new("server:8080")?;
    let channel = channel.authenticate("secret")?;
    let (channel, data) = channel.read_file("/etc/passwd")?;
    println!("读取 {} 字节", data.len());
    channel.close();
    Ok(())
}
```

---

## 7. 资源敏感的类型系统

### 7.1 资源类型系统原理

```
资源敏感类型系统的核心原则:

1. 线性性 (Linearity)
   - 每个资源必须被使用恰好一次
   - 防止资源泄漏和重复释放

2. 仿射性 (Affinity)
   - 资源可以被使用零次或一次
   - 允许资源被丢弃但不允许复制

3. 相关性质 (Relevance)
   - 资源必须被使用至少一次
   - 防止资源泄漏
```

### 7.2 类型系统分类

```
┌─────────────────────────────────────────────────────────────────┐
│                    子结构类型系统谱系                            │
├─────────────────────────────────────────────────────────────────┤
│                                                                 │
│  结构规则          弱化       收缩                                │
│  ─────────────────────────────────────                          │
│  线性逻辑          ✗          ✗     (最严格)                    │
│  仿射逻辑          ✓          ✗                                   │
│  相关逻辑          ✗          ✓                                   │
│  经典/直觉主义     ✓          ✓     (最宽松)                    │
│                                                                 │
│  类型系统实例:                                                  │
│  - 线性: Rust 所有权系统, Clean, LinearML                       │
│  - 仿射: Alms, ATS, Rust (部分)                                 │
│  - 相关: 多数函数式语言                                         │
│  - 经典: C, Java, Haskell                                       │
│                                                                 │
└─────────────────────────────────────────────────────────────────┘
```

### 7.3 效果系统与线性类型

```haskell
-- Haskell 中的线性类型扩展 (LinearTypes)

-- 线性函数类型: a %1 -> b
-- 表示恰好使用参数一次

import Prelude.Linear

-- 线性箭头版本的头函数
headLinear :: [a] %1 -> a
headLinear (x:_) = x
headLinear [] = error "空列表"

-- 所有权转移示例
moveExample :: FilePath %1 -> IO ()
moveExample path = do
    handle <- openFile path
    -- handle 被消耗后，path 不能再被使用
    contents <- readFile handle
    closeFile handle
    print contents
```

### 7.4 内存管理的类型安全

```rust
// 线性类型确保内存安全

// 1. 防止 use-after-free
fn use_after_free_prevented() {
    let s = String::from("hello");
    let r = s;  // s 被移动
    // println!("{}", s);  // 编译错误！
    println!("{}", r);     // 正确
}

// 2. 防止 double-free
fn double_free_prevented() {
    let s = Box::new(42);
    let s2 = s;  // s 被移动
    // drop(s);   // 编译错误：s 已被移动
    drop(s2);    // 正确
}

// 3. 防止内存泄漏（通过 RAII）
fn no_memory_leak() {
    let _guard = File::open("data.txt").unwrap();
}  // guard 自动关闭文件
```

---

## 8. 形式化推理与证明

### 8.1 相继式演算

```
线性逻辑的相继式形式:

  Γ ⊢ Δ

  其中:
  - Γ 是线性假设（必须全部使用）
  - Δ 是线性结论（必须全部产生）

核心公理:
  ─────────────
  A ⊢ A

切规则 (Cut):
  Γ ⊢ A    Δ, A ⊢ B
  ───────────────────────
  Γ, Δ ⊢ B
```

### 8.2 证明标准化

```
线性逻辑的重要性质:

1. 切消除 (Cut Elimination)
   任何证明都可以转换为无切证明
   对应于程序的执行（β-归约）

2. 证明网 (Proof Nets)
   消除证明中的无关顺序信息
   直接对应于并行计算

3. 博弈语义 (Game Semantics)
   证明 = 策略
   反驳 = 反策略
   交互 = 证明验证
```

### 8.3 计算解释

```
Curry-Howard-Lambek 对应:

┌─────────────────────────────────────────────────────────────────┐
│  逻辑          类型理论          范畴论         编程              │
├─────────────────────────────────────────────────────────────────┤
│  命题          类型              对象           数据类型          │
│  证明          项                态射           程序              │
│  证明归约      β-归约            复合           执行              │
│  ⊗             对组类型          张量积          并发组合         │
│  ⊸             线性函数类型      闭范畴          资源转换函数      │
│  !             可拷贝类型        余代数          持久存储         │
└─────────────────────────────────────────────────────────────────┘
```

---

## 9. 参考与延伸阅读

### 经典文献

1. **Jean-Yves Girard** (1987). "Linear Logic". *Theoretical Computer Science*
   - 线性逻辑的奠基论文

2. **Philip Wadler** (1990). "Linear Types can Change the World!"
   - 线性类型在编程语言中的应用

3. **Frank Pfenning** (2002). "Linear Logic"
   - 线性逻辑教程

### 相关文件

- [01_Linear_Logic_Theory.md](./01_Linear_Logic_Theory.md) - 线性逻辑理论基础
- [02_Resource_Types.md](./02_Resource_Types.md) - 资源类型系统详解
- [05_Game_Semantics目录](../01_Game_Semantics/readme.md) - 博弈语义与交互模型

### 总结

线性逻辑为计算机科学提供了强大的理论基础：

1. **资源管理** - 通过线性类型确保资源的正确使用
2. **并发编程** - 会话类型描述安全的通信协议
3. **内存安全** - 所有权系统防止内存错误
4. **证明理论** - 切消除对应程序执行

关键洞察：

- 将**真值**替换为**资源**带来了新的计算范式
- **线性蕴涵** $A \multimap B$ 是函数式编程的核心
- **模态算子** !A 协调了线性与经典逻辑
- **Curry-Howard 对应** 将证明与程序统一

---

*文档版本: 1.0*
*最后更新: 2026-03-13*


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21  
> **维护者**: AI Code Review
