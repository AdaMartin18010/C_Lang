# ETH Zurich / EPFL: Verified Software Engineering

> **层级**: L6++ (极致深度)  
> **关联**: 欧洲软件验证传统与Lean

---

## 课程概述

ETH Zurich和EPFL在软件验证领域有卓越贡献：
- 形式化方法
- 程序验证
- 定理证明
- 编译器验证
- 操作系统验证

---

## ETH Zurich传统

### 1.1 Bertrand Meyer与Eiffel

**契约式设计**:
```
require  (前置条件)
do       (实现)
ensure   (后置条件)
```

**定理1.1 (契约保证)**:
```
满足契约的类满足Liskov替换原则
```

### 1.2 Peter Müller的程序验证

**定理1.2 (分离逻辑)**:
```
{Viper验证器保证内存安全}
```

---

## EPFL传统

### 2.1 Martin Odersky与Scala

**类型系统演化**:
```
Java → Scala → Dotty → Scala 3
```

**定理2.1 (DOT演算)**:
```
DOT类型系统 sound
```

### 2.2 Viktor Kunčak与Stainless

**定理2.2 (Scala验证)**:
```
Stainless可验证Scala子集的程序性质
```

---

## 编译器验证

### 3.1 CompCert项目

**定理3.1 (CompCert正确性)**:
```
CompCert编译器生成的代码与源程序语义等价
```

**证明规模**: ~100k行Coq证明

### 3.2 Vellvm

**定理3.2 (LLVM验证)**:
```
Vellvm形式化LLVM IR语义
```

---

## 操作系统验证

### 4.1 seL4

**定理4.1 (seL4功能正确性)**:
```
seL4内核实现与规范一致
```

**代价**: ~25人年，~20万行证明

### 4.2 Ironclad Apps

**定理4.2 (端到端安全)**:
```
从密码学原语到应用的安全证明链
```

---

## 与Lean 4的对比

| 特性 | Coq (CompCert) | Isabelle (seL4) | Lean 4 |
|:-----|:---------------|:----------------|:-------|
| 证明语言 | Gallina | Isar | Lean |
| 自动化 | 中等 | 高 | 高 |
| 性能 | 中等 | 中等 | 高 |
| 工程化 | 成熟 | 成熟 | 发展中 |

---

## 参考资源

- **ETH**: Software Verification Group
- **EPFL**: Laboratory for Automated Reasoning
- **项目**: CompCert, seL4, Stainless
