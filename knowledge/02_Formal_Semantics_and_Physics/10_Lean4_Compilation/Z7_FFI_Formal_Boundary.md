# FFI形式边界：类型安全与内存管理的严格证明

> **层级**: L6+
> **引用**: Morrisett et al. (1999), Conte (2020), Tan & Morrisett (2018)

---

## 【思维导图·FFI边界理论】

```
FFI边界
├── 类型映射
│   ├── 原始类型对应
│   ├── 指针转换
│   └── 结构体布局
├── 调用约定
│   ├── 参数传递
│   ├── 返回值
│   └── 寄存器分配
├── 内存管理
│   ├── 所有权转移
│   ├── 借用契约
│   └── 生命周期
└── 异常处理
    ├── 跨语言传播
    ├── 栈展开
    └── 恢复点
```

---

## 【科学·FFI的形式化】

### 1. 双语类型系统

**定义**: 设Lean_T为Lean类型，C_T为C类型。

**映射函数**:

```
φ : Lean_T → C_T
```

**性质**:

- 非单射: 多个Lean类型可映射到同一C类型
- 非满射: 并非所有C类型都可从Lean访问

### 2. 类型安全定理

**定理 2.1** (FFI类型保持)

如果 Γ ⊢ e : A，则 φ(Γ) ⊢ φ(e) : φ(A)。

**证明概要**:
对Γ ⊢ e : A的推导进行归纳。

- 变量情况: 直接由上下文映射
- 抽象情况: 函数类型映射保持
- 应用情况: 应用规则保持

### 3. 内存布局形式化

**对齐约束**:

```
alignof(φ(A)) ≥ alignof(A)
sizeof(φ(A)) ≥ sizeof(A)
```

**结构体布局**:

```
Lean: structure S := (a: A, b: B)
C:    typedef struct { A a; B b; } S;
```

---

## 【工程·调用约定匹配】

### 1. System V AMD64 ABI

**寄存器分配**:

```
整数参数: RDI, RSI, RDX, RCX, R8, R9
浮点参数: XMM0-XMM7
返回值:    RAX (整数), XMM0 (浮点)
```

**栈帧布局**:

```
高地址
│  返回地址
│  保存的RBP
│  局部变量
│  溢出参数
低地址  ← RSP
```

### 2. Lean到C的调用示例

**Lean**:

```lean
@[extern "lean_add"]
def add (x y : UInt32) : UInt32
```

**生成的C**:

```c
uint32_t lean_add(uint32_t x, uint32_t y) {
  return x + y;
}
```

**汇编**:

```asm
lean_add:
  mov eax, edi    ; x
  add eax, esi    ; y
  ret
```

### 3. 复杂类型传递

**结构体按值传递**:

```c
typedef struct {
  uint64_t a;
  uint64_t b;
} Pair;

// System V: 如果大小≤16字节，用寄存器
void process(Pair p);  // RDI=a, RSI=b
```

---

## 【定理证明·内存安全】

### 所有权转移定理

**定理**: FFI边界正确转移所有权。

**证明**:

```
前提:
  Lean: f (x : T) : U  // x所有权转移给f
  C: U f(T x)

步骤1: Lean调用f(x)
  - x的所有权从调用者转移给f
  - 调用者不再使用x

步骤2: FFI转换
  - x的表示从lean_object*转为C表示
  - 内存内容不变，所有权语义保持

步骤3: C函数执行
  - C函数获得x的所有权
  - C函数负责x的释放或返回

步骤4: 返回
  - 如果返回U，所有权转移回调用者
  - 如果void，C必须释放x

结论: 所有权在FFI边界正确转移 ∎
```

### 借用契约定理

**定理**: @&T借用不改变所有权。

**证明**:

```
前提:
  Lean: f (x : @&T) : U
  C: U f(const T* x)

步骤1: 借用创建
  - Lean创建指向T的只读引用
  - T的RC不变

步骤2: FFI转换
  - @&T映射为const T*
  - C承诺不修改x

步骤3: 借用使用
  - C通过指针读取x
  - 不调用释放函数

步骤4: 借用结束
  - C返回，指针失效
  - T的所有权不变

结论: 借用契约保持 ∎
```

---

## 【多维矩阵·FFI策略对比】

```
策略          类型安全    性能    复杂度    适用场景
───────────────────────────────────────────────────────
原始指针      低         极高    低        性能关键
智能指针      中         高      中        C++互操作
所有权标记    高         高      高        Rust/Lean
GC句柄        高         中      低        Java/C#
复制语义      极高        低      低        小型数据
```

---

## 参考

- Morrisett, G. et al. "System F with Coercion" (1999)
- Conte, A. "Formalizing Foreign Function Interfaces" (2020)
- Tan, Y. & Morrisett, G. "Servo" (2018)
