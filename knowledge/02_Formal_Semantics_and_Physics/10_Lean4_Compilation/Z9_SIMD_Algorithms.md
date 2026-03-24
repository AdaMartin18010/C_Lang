# SIMD向量化算法：自动向量化的理论与实践

> **层级**: L6+
> **引用**: Kennedy & Allen (2001), Larsen & Amarasinghe (2000), Nuzman et al. (2011)

---

## 目录

- [SIMD向量化算法：自动向量化的理论与实践](#simd向量化算法自动向量化的理论与实践)
  - [目录](#目录)
  - [【思维导图·SIMD技术栈】](#思维导图simd技术栈)
  - [【科学·向量化理论】](#科学向量化理论)
    - [1. 数据并行模型](#1-数据并行模型)
    - [2. 循环向量化的合法性](#2-循环向量化的合法性)
    - [3. SLP (Superword Level Parallelism)](#3-slp-superword-level-parallelism)
  - [【工程·实现技术】](#工程实现技术)
    - [1. 循环剥离(Peeling)](#1-循环剥离peeling)
    - [2. LLVM向量化Pass](#2-llvm向量化pass)
    - [3. 归约向量化](#3-归约向量化)
  - [【多维矩阵·SIMD指令集】](#多维矩阵simd指令集)
  - [【定理·向量化正确性】](#定理向量化正确性)
    - [语义保持定理](#语义保持定理)
  - [参考](#参考)

## 【思维导图·SIMD技术栈】

```
SIMD向量化
├── 硬件支持
│   ├── SSE (128-bit)
│   ├── AVX (256-bit)
│   ├── AVX-512 (512-bit)
│   └── NEON (ARM)
├── 编译技术
│   ├── 循环向量化
│   ├── SLP向量化
│   └── 归约优化
└── 性能优化
    ├── 内存对齐
    ├── 预取
    └── 剥离-循环-余数
```

---

## 【科学·向量化理论】

### 1. 数据并行模型

**定义**: SIMD操作对向量寄存器的所有元素同时执行相同操作。

**向量长度**:

```
SSE:     128-bit = 4 × float32
AVX:     256-bit = 8 × float32
AVX-512: 512-bit = 16 × float32
```

### 2. 循环向量化的合法性

**定义**: 循环可向量化如果无循环携带依赖。

**依赖类型**:

```
真依赖 (RAW):  a[i] = ...; ... = a[i+1]
反依赖 (WAR):  ... = a[i]; a[i+1] = ...
输出依赖(WAW): a[i] = ...; a[i] = ...
```

**向量化条件**:

```
∀依赖距离d: d = 0 ∨ 循环可向量化
```

**定理**: 无依赖循环可向量化。

### 3. SLP (Superword Level Parallelism)

**算法** (Larsen & Amarasinghe, 2000):

```
1. 构建数据依赖图(DDG)
2. 识别独立的同构语句组
3. 打包为向量操作
4. 替换标量代码
```

**示例**:

```c
// 原始代码
a1 = b1 + c1;
a2 = b2 + c2;
a3 = b3 + c3;
a4 = b4 + c4;

// SLP向量化
v_a = vaddps(v_b, v_c);
// 提取a1,a2,a3,a4
```

---

## 【工程·实现技术】

### 1. 循环剥离(Peeling)

**目的**: 处理不对齐访问

**策略**:

```c
// 处理不对齐头部
while (ptr % 32 != 0) {
  *ptr = scalar_op(*ptr);
  ptr++;
}

// 向量化主体
while (remaining >= 8) {
  simd_store(simd_op(simd_load(ptr)));
  ptr += 8;
  remaining -= 8;
}

// 处理尾部
while (remaining > 0) {
  *ptr = scalar_op(*ptr);
  ptr++;
  remaining--;
}
```

### 2. LLVM向量化Pass

**LoopVectorize**:

```
输入: LLVM IR循环
输出: 向量化循环

算法:
1. 依赖分析
2. 收益/成本分析
3. 决定向量宽度
4. 转换循环
```

**SLPVectorizer**:

```
输入: 基本块
输出: 向量化基本块

算法:
1. 构建操作图
2. 查找可打包组
3. 成本模型评估
4. 生成向量代码
```

### 3. 归约向量化

**问题**: 归约有循环携带依赖

```c
sum = 0;
for (i = 0; i < n; i++) {
  sum += a[i];  // 依赖!
}
```

**解决**: 部分归约 + 最终归约

```c
vec_sum = {0, 0, 0, 0};
for (i = 0; i < n; i += 4) {
  vec_sum += simd_load(&a[i]);
}
sum = vec_sum[0] + vec_sum[1] + vec_sum[2] + vec_sum[3];
```

---

## 【多维矩阵·SIMD指令集】

```
特性          SSE      AVX      AVX-512   NEON
────────────────────────────────────────────────
位宽         128      256      512       128
float数      4        8        16        4
double数     2        4        8         2
整数操作     完整      完整      完整      完整
FMA          无       有        有        有
掩码操作     无       无        有        有
 gather      无       部分      完整      部分
```

---

## 【定理·向量化正确性】

### 语义保持定理

**定理**: 向量化保持程序语义。

**证明**:

```
前提: 循环无循环携带依赖

步骤1: 标量循环语义
  for i: a[i] = f(b[i])
  等价于对所有i独立执行

步骤2: 向量循环语义
  for i step VEC_WIDTH:
    simd_store(f(simd_load(&b[i])))
  由于无依赖，所有元素独立

步骤3: 等价性
  标量和向量版本对所有i产生相同结果

结论: 向量化语义正确 ∎
```

---

## 参考

- Kennedy, K. & Allen, R. "Optimizing Compilers for Modern Architectures" (2001)
- Larsen, S. & Amarasinghe, S. "Exploiting Superword Level Parallelism" (2000)
- Nuzman, D. et al. "Auto-Vectorization of Interleaved Data" (2011)
