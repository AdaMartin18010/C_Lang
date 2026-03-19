# Rust vs C 技术深度对比

> **文档状态**: 🆕 新建
> **创建日期**: 2026-03-18
> **分析深度**: 语言核心机制

---

## 执行摘要

Rust和C都是系统编程语言，提供对硬件的低级控制，但在安全保证方面有根本差异：

```
┌─────────────────────────────────────────────────────────────┐
│                 Rust vs C 核心差异                          │
├─────────────────────────────────────────────────────────────┤
│  C: 信任程序员，提供完全自由                                 │
│     → 高性能但易出错                                         │
│                                                             │
│  Rust: 编译器强制执行安全规则                                │
│     → 同等性能，编译时保证安全                               │
└─────────────────────────────────────────────────────────────┘
```

---

## 1. 内存管理对比

### C: 手动内存管理

```c
// C代码 - 需要程序员管理所有内存
typedef struct {
    char* name;
    int age;
} Person;

Person* person_create(const char* name, int age) {
    Person* p = malloc(sizeof(Person));
    if (!p) return NULL;

    p->name = malloc(strlen(name) + 1);
    if (!p->name) {
        free(p);  // 容易遗漏!
        return NULL;
    }

    strcpy(p->name, name);
    p->age = age;
    return p;
}
```

### Rust: 所有权系统

```rust
// Rust代码 - 编译器管理内存
struct Person {
    name: String,
    age: u32,
}

fn main() {
    let p = Person { name: String::from("Alice"), age: 30 };
    // p在作用域结束时自动释放
}
```

---

## 2. 借用检查器 (Borrow Checker)

### 核心规则: Aliasing XOR Mutation

```rust
// 可以同时有多个不可变引用
let x = 5;
let r1 = &x;
let r2 = &x;
println!("{} {}", r1, r2);  // OK

// 或者一个可变引用
let mut y = 10;
let r3 = &mut y;
// let r4 = &y;  // 编译错误!
```

---

## 3. 内存安全保证对比

| 安全问题 | C | Rust | 说明 |
|:---------|:--|:-----|:-----|
| **缓冲区溢出** | ❌ 可能 | ✅ 编译时阻止 | Rust检查索引边界 |
| **Use-after-free** | ❌ 可能 | ✅ 编译时阻止 | 所有权系统 |
| **Double-free** | ❌ 可能 | ✅ 编译时阻止 | 单一所有权 |
| **空指针解引用** | ❌ 可能 | ✅ 编译时阻止 | Option<T> |
| **数据竞争** | ❌ 可能 | ✅ 编译时阻止 | 借用检查器 |

---

## 4. 性能对比

| 测试 | C (GCC -O3) | Rust (release) | 说明 |
|:-----|:------------|:---------------|:-----|
| **Binary trees** | 1.0x | 1.0x | 相当 |
| **Fannkuch-redux** | 1.0x | 0.95x | Rust略快 |
| **Mandelbrot** | 1.0x | 1.0x | 相当 |
| **Regex-redux** | 1.0x | 1.1x | Rust更快 |

---

## 5. 与C互操作

### Rust调用C

```rust
extern "C" {
    fn c_function(x: i32) -> i32;
}

pub struct CResource {
    handle: *mut libc::c_void,
}

impl Drop for CResource {
    fn drop(&mut self) {
        unsafe { c_cleanup(self.handle) }
    }
}
```

---

## 总结

### 选择C的场景

- ✅ 极端资源受限 (KB级内存)
- ✅ 已有大规模C代码库
- ✅ 需要认证 (DO-178C等)

### 选择Rust的场景

- ✅ 新系统开发
- ✅ 安全关键应用
- ✅ 并发密集型系统

**最后更新**: 2026-03-18


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
