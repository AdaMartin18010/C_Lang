# 形式化验证代码提取：从证明到可执行程序

> **层级**: L6

## 一、提取理论基础

### Paulin-Mohring提取

从CoC到ML的提取算法：

**擦除规则**：

- 提取(Prop) = Unit
- 提取(Type) = 具体类型
- 提取(Π(x:A).B) =
  - 如果A:Prop，提取(B)
  - 否则提取(A) → 提取(B)

**定理**: 提取保持观察语义。

## 二、CompCert验证编译器

### 编译正确性

CompCert证明：

```
源程序 ≡ 编译后程序
```

**模拟关系**：

- 前进模拟: 源语言 →*目标语言 →*
- 后退模拟: 目标语言 →*源语言 →*

**定理**: CompCert生成的汇编语义等价于源C程序。

## 三、HACL*验证密码学

### 形式化验证的加密原语

**Chacha20-Poly1305验证**：

```fstar
val chacha20_encrypt:
  key:uint8_p{length key = 32} →
  nonce:uint8_p{length nonce = 12} →
  counter:UInt32.t →
  plain:uint8_p →
  cipher:uint8_p →
  Stack unit
  (requires fun h -> ...)
  (ensures fun h0 _ h1 -> ...)
```

提取到C后性能接近OpenSSL。

## 四、Fiat-Crypto

### 密码学算术生成

**蒙哥马利模乘形式化**：

生成针对特定素数优化的汇编。

**性能**: 与手写汇编相当。

## 五、Lean 4的提取策略

### 数学代码提取

**示例：矩阵乘法验证提取**：

```lean
def matMul {n m p : Nat} (A : Matrix n m) (B : Matrix m p) : Matrix n p := ...

theorem matMul_assoc {n m p q} (A : Matrix n m) (B : Matrix m p) (C : Matrix p q) :
  (A * B) * C = A * (B * C) := ...
```

提取到优化BLAS例程。

## 六、参考

- Paulin-Mohring (1989)
- Leroy "CompCert" (2009-2023)
- HACL* (2017)
