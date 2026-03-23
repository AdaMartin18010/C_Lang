# SPARK形式化验证概述

## 1. 什么是SPARK？

### 1.1 定义与定位

SPARK是Ada的子集，专为**高完整性软件**设计，提供形式化验证能力：

```
SPARK = Ada子集 + 合约注解 + 形式化验证工具

Ada完整语言          SPARK子集
     │                   │
     ├── 访问类型(指针)   │ ✗ 排除(别名分析困难)
     ├── goto语句        │ ✗ 排除(控制流复杂)
     ├── 任务(Task)      │ ✗ 受限( Ravenscar子集)
     ├── 异常处理        │ △ 受限
     └── 动态分配        │ △ 受限
                         │
                         ├── 前置条件(Pre)
                         ├── 后置条件(Post)
                         ├── 循环不变式(Loop Invariant)
                         ├── 类型不变式(Type Invariant)
                         └── 断言(Assert)
```

### 1.2 SPARK验证等级

| 等级 | 名称 | 能力 | 工具 | 目标 |
|------|------|------|------|------|
| **Stone** | 流分析 | 数据流、控制流、信息依赖 | gnatprove | 编码规范合规 |
| **Bronze** | 基础证明 | 运行时错误消除(除零、溢出、越界) | gnatprove + Alt-Erzo | 无运行时错误(RTE) |
| **Silver** | 功能证明 | 合约验证(Pre/Post/Invariant) | gnatprove + CVC4/Z3 | 功能正确性 |
| **Gold** | 完整证明 | 复杂属性、循环终止、信息 flow | SPARK Pro | 数学级正确性 |

---

## 2. SPARK工具链

### 2.1 gnatprove

```bash
# 基本验证命令
gnatprove -P project.gpr          # 验证整个项目
gnatprove -P project.gpr --level=4  # 最高证明努力
gnatprove -P project.gpr --mode=flow  # 仅流分析(Stone)
gnatprove -P project.gpr --mode=prove # 完全证明(Silver+)

# 输出解读
# ✓ - 证明通过
# ✗ - 证明失败
# ? - 超时(需要人工介入)
# C - 条件通过(有前提)
```

### 2.2 项目配置示例

```ada
-- 项目文件 (project.gpr)
project SPARK_Demo is
   for Source_Dirs use ("src");
   for Object_Dir use "obj";

   package Prove is
      for Proof_Switches ("Ada") use (
         "--level=4",           -- 证明努力级别
         "--no-loop-unrolling", -- 不展开循环
         "--prover=z3,cvc4"     -- 使用的SMT求解器
      );
   end Prove;
end SPARK_Demo;
```

---

## 3. 合约编程

### 3.1 前置条件与后置条件

```ada
-- 规范文件 (math.ads)
package Math with
   SPARK_Mode  -- 启用SPARK分析
is
   -- 整数除法（安全版本）
   function Safe_Divide (Num, Den : Integer) return Integer
      with
         Pre  => Den /= 0,                    -- 前置：除数非零
         Post => Safe_Divide'Result = Num / Den;  -- 后置：结果正确

   -- 数组查找
   function Find (Arr : Array_Type; Target : Integer) return Natural
      with
         Pre  => Arr'Length > 0,              -- 数组非空
         Post => (Find'Result in Arr'Range)   -- 结果在范围内
                  and then Arr (Find'Result) = Target;

end Math;

-- 实现文件 (math.adb)
package body Math with SPARK_Mode is
   function Safe_Divide (Num, Den : Integer) return Integer is
   begin
      return Num / Den;  -- SPARK证明除数不会为零
   end Safe_Divide;

   function Find (Arr : Array_Type; Target : Integer) return Natural is
   begin
      for I in Arr'Range loop
         if Arr (I) = Target then
            return I;  -- 找到目标
         end if;
      end loop;

      -- 如果未找到，Post条件会失败
      -- SPARK会检查所有路径是否满足合约
      return Arr'First;  -- 应该处理未找到情况
   end Find;
end Math;
```

### 3.2 类型不变式

```ada
-- 确保类型始终满足特定条件
package Stack_Package with SPARK_Mode is
   Max_Size : constant := 100;

   type Stack is private
      with Type_Invariant => Size (Stack) <= Max_Size;
   -- 栈大小永远不会超过Max_Size

   function Size (S : Stack) return Natural;

   procedure Push (S : in out Stack; Value : Integer)
      with
         Pre  => not Is_Full (S),
         Post => Size (S) = Size (S)'Old + 1;  -- 大小增加1

   function Is_Full (S : Stack) return Boolean;

private
   type Stack_Rep is array (1 .. Max_Size) of Integer;

   type Stack is record
      Data : Stack_Rep;
      Top  : Natural range 0 .. Max_Size := 0;
   end record;

   function Size (S : Stack) return Natural is (S.Top);
   function Is_Full (S : Stack) return Boolean is (S.Top = Max_Size);
end Stack_Package;
```

---

## 4. 循环验证

### 4.1 循环不变式

```ada
procedure Binary_Search (Arr : Sorted_Array; Target : Integer; Found : out Boolean)
   with
      Pre  => Arr'Length > 0 and then Is_Sorted (Arr),
      Post => Found = (for some E of Arr => E = Target)
is
   Low  : Natural := Arr'First;
   High : Natural := Arr'Last;
begin
   Found := False;

   while Low <= High loop
      -- 循环不变式：目标如果在数组中，必在[Low, High]范围内
      pragma Loop_Invariant (Low in Arr'Range and High in Arr'Range);
      pragma Loop_Invariant (for all I in Arr'Range =>
         (if Arr (I) = Target then I in Low .. High));
      pragma Loop_Variant (Decreases => High - Low);  -- 终止性证明

      Mid : constant Natural := Low + (High - Low) / 2;

      if Arr (Mid) = Target then
         Found := True;
         return;
      elsif Arr (Mid) < Target then
         Low := Mid + 1;
      else
         High := Mid - 1;
      end if;
   end loop;
end Binary_Search;
```

### 4.2 循环变体（终止证明）

```ada
procedure Bubble_Sort (Arr : in out Array_Type) with SPARK_Mode is
begin
   for I in reverse Arr'Range loop
      pragma Loop_Variant (Decreases => I);  -- I递减，保证终止

      for J in Arr'First .. I - 1 loop
         pragma Loop_Variant (Increases => J);  -- J递增
         pragma Loop_Invariant (Sorted (Arr (J + 1 .. I)));  -- 已排序部分

         if Arr (J) > Arr (J + 1) then
            Swap (Arr (J), Arr (J + 1));
         end if;
      end loop;
   end loop;
end Bubble_Sort;
```

---

## 5. 高级证明技术

### 5.1 幽灵代码（Ghost Code）

用于证明但不影响运行时：

```ada
package Ghost_Example with SPARK_Mode is
   -- 幽灵类型：仅用于证明
   type Ghost_Counter is private with Ghost;

   procedure Increment (X : in out Integer)
      with
         Post => X = X'Old + 1;
private
   type Ghost_Counter is record
      Count : Natural := 0;
   end record;
end Ghost_Example;

package body Ghost_Example with SPARK_Mode is
   procedure Increment (X : in out Integer) is
      Ghost_Count : Ghost_Counter with Ghost;  -- 幽灵变量
   begin
      X := X + 1;
      Ghost_Count.Count := Ghost_Count.Count + 1;  -- 仅用于证明

      -- 证明：Ghost_Count.Count = X - Initial_X
   end Increment;
end Ghost_Example;
```

### 5.2 假设与公理

```ada
-- 声明数学公理（需谨慎使用）
package Math_Axioms with SPARK_Mode is
   -- 假设：加法交换律（实际应证明或作为公理）
   function Add_Commutative (A, B : Integer) return Boolean
      with Ghost, Import, Post => Add_Commutative'Result = (A + B = B + A);
end Math_Axioms;
```

---

## 6. 工业案例分析

### 6.1 NVIDIA自动驾驶固件

```
项目: NVIDIA DRIVE安全固件
规模: 50,000+行SPARK代码
验证等级: Silver
成果:
  - 100%运行时错误消除
  - 关键算法合约验证
  - DO-178C认证支撑
方法:
  - 从C逐步迁移到SPARK
  - 关键模块完全形式化验证
  - 组合测试与证明
```

### 6.2 Thales ExoMars

```
项目: 欧洲火星探测器导航软件
规模: 10,000+行SPARK代码
验证等级: Gold
成果:
  - 完全形式化验证
  - 数学级正确性保证
  - 零运行时错误
技术:
  - 浮点运算精确建模
  - 复杂轨道算法验证
  - 自动代码生成
```

---

## 7. SPARK vs 其他验证方法

| 方法 | 保证级别 | 自动化 | 适用场景 | 工具 |
|------|---------|--------|---------|------|
| **SPARK** | 高 | 高 | 嵌入式实时 | gnatprove |
| **MISRA C** | 中 | 中 | 汽车电子 | PC-lint |
| **CompCert** | 最高 | 低 | 编译器 | Coq |
| **CBMC** | 高 | 高 | C代码验证 | CBMC |
| **Rust borrowck** | 中 | 高 | 系统编程 | rustc |

---

## 8. 学习路径

```
阶段1: Stone (数据流分析)
├── 理解SPARK语言子集
├── 学习流分析概念
└── 消除未初始化变量警告

阶段2: Bronze (运行时错误)
├── 添加范围约束
├── 处理数组边界
└── 消除除零/溢出风险

阶段3: Silver (功能证明)
├── 设计合约(Pre/Post)
├── 编写循环不变式
└── 验证功能正确性

阶段4: Gold (完整验证)
├── 幽灵代码
├── 信息flow分析
└── 复杂属性证明
```

---

**文档版本**: 1.0
**创建日期**: 2026-03-19
**维护者**: C_Lang Knowledge Base Team


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
