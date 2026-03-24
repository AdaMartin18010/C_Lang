# Lean 4 → C 编译：完整主题索引与理论体系

> **层级**: L6 (元理论层)
> **目标**: 系统梳理 Lean 4 到 C 编译的全部理论主题
> **对标**: Stanford CS242, MIT 6.170, CMU 15-312, INRIA Gallium

---

## 一、理论基础维度

### 1.1 类型论基础

- Martin-Löf 类型论：直觉主义基础、判断与命题
- Calculus of Constructions：Coquand-Huet 演算
- 归纳类型：W-类型、归纳构造子
- 宇宙层级：累积层级、宇宙多态
- 证明无关性：Prop vs Type 分离

### 1.2 范畴论语义

- 笛卡尔闭范畴：指数对象、currying
- 遗忘函子：结构保持映射
- 余代数：无限结构语义
- 单子与计算效应

### 1.3 操作语义

- 大步语义：求值数学描述
- 小步语义：归约关系、合流性
- 上下文等价：可观察行为
- 双模拟：互模拟等价

---

## 二、编译器架构

### 2.1 前端层

- 解析器：递归下降、优先级攀爬
- 宏系统：卫生宏、相位分离
- Elaboration：双向类型检查
- 统一算法：高阶统一

### 2.2 核心层

- 内核类型检查：强规范化
- 归纳定义：正性检查
- 商类型：集合oids

### 2.3 IR 层

- 中间表示：A-normal form、CPS、SSA
- 闭包表示：环境扁平化
- 控制流分析：尾调用识别

### 2.4 代码生成

- C 代码生成策略
- 运行时系统：lean_object
- FFI 边界：调用约定

---

## 三、数据表示

### 3.1 装箱与未装箱

- 值域有限性判据
- 归纳类型的位表示
- 浮点语义：IEEE 754

### 3.2 内存布局

- 对象头部结构
- 数组表示：扁平 vs 指针
- 字符串编码：UTF-8

### 3.3 闭包环境

- 环境扁平化
- 部分应用实现
- 尾调用优化：TCO

---

## 四、运行时系统

### 4.1 内存管理

- 引用计数：增量更新
- 垃圾回收：标记-清除
- 分配策略

### 4.2 异常与控制

- 零成本异常
- 协程与纤程
- 原子操作

### 4.3 并发模型

- 线程安全：不可变数据
- 并行运行时：工作窃取
- 异步 I/O

---

## 五、验证与正确性

### 5.1 编译器正确性

- 语义保持
- 模拟关系
- 完全抽象

### 5.2 类型安全

- 进步性 (Progress)
- 保持性 (Preservation)
- 规范化 (Normalization)

### 5.3 形式化工具

- Lean 4 元编程
- Iris 分离逻辑
- CompCert 验证

---

## 六、待创建深度文档

| 优先级 | 主题 | 目标深度 |
|:------|:-----|:---------|
| P0 | Martin-Löf 类型论与 Lean 内核 | 30KB+ |
| P0 | Calculus of Constructions 与证明擦除 | 25KB+ |
| P0 | 范畴论语义：遗忘函子 | 25KB+ |
| P0 | 操作语义与编译保持 | 20KB+ |
| P1 | 闭包转换理论 | 20KB+ |
| P1 | 引用计数与内存管理 | 20KB+ |
| P1 | 尾调用优化形式化 | 15KB+ |
| P2 | FFI 边界形式化 | 15KB+ |
| P2 | 编译器正确性证明 | 20KB+ |

---

## 七、关键引用文献

1. Martin-Löf, P. "Intuitionistic Type Theory" (1984)
2. Coquand, T. & Huet, G. "The Calculus of Constructions" (1988)
3. Pierce, B.C. "Types and Programming Languages" (2002)
4. Harper, R. "Practical Foundations for Programming Languages" (2016)
5. de Moura, L. et al. "The Lean 4 Theorem Prover" (2021)
6. Leroy, X. "The CompCert C Verified Compiler" (2009-2023)

---

*此索引为 Lean 4 → C 编译理论的完整主题地图，每个主题需展开为深度技术文档*
