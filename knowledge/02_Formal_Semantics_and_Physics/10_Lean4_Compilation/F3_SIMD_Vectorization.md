# SIMD向量化：自动向量化与SIMD指令生成

> **层级**: L6 (元理论层)
> **引用**: Kennedy & Allen (2001), Larsen & Amarasinghe (2000)
> **课程对标**: CMU 15-618, Stanford CS149

---

## 一、SIMD理论基础

### 1.1 数据并行模型

**定义 1.1** (SIMD: Single Instruction Multiple Data)

单条指令同时操作多个数据元素。

**向量长度**：

- SSE: 128-bit (4 x float)
- AVX: 256-bit (8 x float)
- AVX-512: 512-bit (16 x float)

### 1.2 向量类型理论

**定义 1.2** (向量类型)

```
Vec n α  -- 长度为n的向量，元素类型α
```

**操作**：

- map: (α → β) → Vec n α → Vec n β
- zipWith: (α → β → γ) → Vec n α → Vec n β → Vec n γ
- reduce: (α → α → α) → α → Vec n α → α
- gather/scatter: 不规则访问

---

## 二、自动向量化算法

### 2.1 循环向量化

**定理 2.1** (向量化合法性)

循环可以向量化如果：

- 无循环携带依赖
- 内存访问连续或对齐
- 无分支或分支可预测

**依赖分析**：

```
for i:
  a[i] = a[i-1] + 1  -- 循环携带依赖，不能向量化

for i:
  a[i] = b[i] + c[i]  -- 无依赖，可向量化
```

### 2.2 SLP向量化

**Superword Level Parallelism** (Larsen & Amarasinghe, 2000)：

将独立标量操作打包为向量操作。

**示例**：

```c
// 原始标量代码
a = x1 + y1;
b = x2 + y2;
c = x3 + y3;
d = x4 + y4;

// 向量化后
v1 = {x1, x2, x3, x4};
v2 = {y1, y2, y3, y4};
v3 = v1 + v2;
// 提取结果到a, b, c, d
```

---

## 三、Lean 4的向量化支持

### 3.1 SIMD原语

```lean
structure FloatVec4 where
  private mk ::
  data : @&ByteArray  -- 128-bit对齐

def FloatVec4.add (a b : FloatVec4) : FloatVec4 :=
  -- 生成ADDPS指令
  FloatVec4.mk (simd_add_f32x4 a.data b.data)
```

### 3.2 自动向量化启发式

**编译器决策**：

```
if loop_trip_count < 4 then
  顺序执行
else if has_loop_carried_dependency then
  检查是否可重排
else
  向量化
```

**成本模型**：

```
cost(vectorized) = setup_cost + (n/vec_width) * vec_op_cost
cost(scalar) = n * scalar_op_cost

vectorize if cost(vectorized) < cost(scalar)
```

---

## 四、内存对齐与边界处理

### 4.1 对齐要求

**定义 4.1** (内存对齐)

地址addr是align-byte对齐如果：

```
addr % align == 0
```

**SIMD对齐**：

- SSE: 16-byte对齐
- AVX: 32-byte对齐

### 4.2 Peel-循环-Remainder策略

**处理不对齐访问**：

```
// Prologue: 处理不对齐头部
while (ptr % 32 != 0) {
  *ptr = scalar_op(*ptr);
  ptr++;
}

// Main loop: 向量化
while (remaining >= 8) {
  simd_store(simd_op(simd_load(ptr)));
  ptr += 8;
  remaining -= 8;
}

// Epilogue: 处理尾部
while (remaining > 0) {
  *ptr = scalar_op(*ptr);
  ptr++;
  remaining--;
}
```

---

## 五、LLVM向量指令

### 5.1 IR中的向量类型

```llvm
%vec4 = type <4 x float>

define <4 x float> @add_vec(<4 x float> %a, <4 x float> %b) {
  %sum = fadd <4 x float> %a, %b
  ret <4 x float> %sum
}
```

### 5.2 向量化Pass

**LoopVectorize**：

- 识别可向量化循环
- 生成向量IR
- 处理归约

**SLPVectorizer**：

- 基本块内向量化
- 打包独立指令

---

**参考文献**：

- Kennedy, K. & Allen, R. "Optimizing Compilers for Modern Architectures" (2001)
- Larsen, S. & Amarasinghe, S. "Exploiting Superword Level Parallelism" (2000)
