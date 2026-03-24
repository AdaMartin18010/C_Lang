# FFI形式化：Lean 4与C的边界理论

> **层级**: L6 (元理论层)
> **引用**: FFI规范, Morrisett et al. (1999), Conte (2020)
> **课程对标**: MIT 6.035, Stanford CS143

---

## 一、FFI的数学模型

### 1.1 双语系统的类型

**定义 1.1** (双语类型系统)

设 Lean_T 为Lean 4的类型集合，C_T 为C的类型集合。

FFI边界是映射：

```
φ : Lean_T → C_T
```

**性质**：

- φ不一定是单射（多个Lean类型可能映射到同一个C类型）
- φ不一定是满射（并非所有C类型都可从Lean访问）

### 1.2 类型安全的边界

**定义 1.2** (FFI类型安全)

对于所有 t : Lean_T，
如果 Lean ⊢ e : t，则 C ⊢ φ(e) : φ(t)。

---

## 二、Lean 4到C的类型映射

### 2.1 原始类型的映射

| Lean 4类型 | C类型 | 映射φ |
|:-----------|:------|:------|
| UInt8 | uint8_t | 恒等 |
| UInt16 | uint16_t | 恒等 |
| UInt32 | uint32_t | 恒等 |
| UInt64 | uint64_t | 恒等 |
| Float | double | IEEE 754 |
| Bool | uint8_t | 0/1编码 |
| Char | uint32_t | Unicode scalar |

### 2.2 装箱类型的映射

**定义 2.1** (lean_object)

```c
typedef struct lean_object {
    uint16_t m_rc;      // 引用计数
    uint16_t m_tag;     // 类型标签
    uint16_t m_sz;      // 对象大小
    uint16_t m_other;   // 保留字段
    void* m_data[];     // 数据
} lean_object;
```

所有Lean对象在C中都是 lean_object*。

---

## 三、调用约定的匹配

### 3.1 调用约定形式化

**定义 3.1** (调用约定)

调用约定是四元组 (ArgPass, RetPass, StackClean, RegSave)：

- ArgPass : 参数传递方式（寄存器/栈）
- RetPass : 返回值传递方式
- StackClean : 栈清理责任（调用者/被调用者）
- RegSave : 寄存器保存责任

### 3.2 System V AMD64 ABI

**参数传递**：

- 整数/指针：RDI, RSI, RDX, RCX, R8, R9
- 浮点：XMM0-XMM7
- 返回值：RAX, XMM0

**Lean 4到C的适配**：

```c
// Lean调用C
extern "C" lean_object* lean_mk_string(const char* s);

// C调用Lean
lean_object* my_lean_function(lean_object* arg1, lean_object* arg2);
```

---

## 四、内存管理的边界

### 4.1 所有权转移

**定义 4.1** (所有权规则)

- C函数接收lean_object*：获得所有权（必须dec_ref）
- C函数返回lean_object*：转移所有权给调用者

**示例**：

```c
// 正确：接收所有权并释放
void process(lean_object* obj) {
    // 使用obj
    lean_dec(obj);  // 释放
}

// 错误：内存泄漏
void bad_process(lean_object* obj) {
    // 使用obj但不释放
}
```

### 4.2 借用契约

**定义 4.2** (借用规则)

- @&T 参数：不获取所有权，调用者保留所有权
- 借用期间，被借用对象必须有效

---

## 五、FFI的正确性

### 5.1 形式化契约

**定理 5.1** (FFI语义保持)

如果 Lean 函数 f : A → B 通过FFI导出到C，
对于所有 a : A，有：

```
φ(f a) = f_C φ(a)
```

其中 f_C 是C中的对应函数。

### 5.2 类型安全证明

**引理 5.2** (边界类型安全)

对于任何通过 @extern 标记的函数，
Lean类型检查器确保：

1. C类型存在对应
2. 调用约定匹配
3. 内存管理契约明确

---

**参考文献**：

- Morrisett, G. et al. "System F with Coercion" (1999)
- Conte, A. "Formalizing Foreign Function Interfaces" (2020)
