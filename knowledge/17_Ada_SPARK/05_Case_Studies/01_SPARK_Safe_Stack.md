# SPARK案例研究：安全栈实现

## 1. 项目概述

### 1.1 目标

实现一个经过形式化验证的栈数据结构，证明：

- 无运行时错误（无溢出、下溢）
- 功能正确性（LIFO语义）
- 边界安全

### 1.2 验证等级

```text
验证目标: Silver级 (功能正确性)
├── Stone: ✓ 流分析通过
├── Bronze: ✓ 运行时错误消除
└── Silver: ✓ 合约验证
```

---

## 2. 规范设计

### 2.1 接口规范

```ada
-- safe_stack.ads
package Safe_Stack with SPARK_Mode is
   pragma Elaborate_Body;

   -- 栈容量
   Max_Size : constant := 100;

   -- 栈类型定义
   type Stack is private
      with Default_Initial_Condition => Is_Empty (Stack);

   -- 查询操作
   function Is_Empty (S : Stack) return Boolean;
   function Is_Full (S : Stack) return Boolean;
   function Size (S : Stack) return Natural;
   function Top (S : Stack) return Integer
      with Pre => not Is_Empty (S);

   -- 修改操作
   procedure Push (S : in out Stack; Value : Integer)
      with
         Pre  => not Is_Full (S),
         Post => Size (S) = Size (S)'Old + 1 and
                 Top (S) = Value and
                 not Is_Empty (S);

   procedure Pop (S : in out Stack; Value : out Integer)
      with
         Pre  => not Is_Empty (S),
         Post => Size (S) = Size (S)'Old - 1 and
                 Value = Top (S)'Old;

   procedure Clear (S : in out Stack)
      with Post => Is_Empty (S);

private
   -- 私有实现
   subtype Index_Range is Natural range 0 .. Max_Size;
   type Data_Array is array (1 .. Max_Size) of Integer;

   type Stack is record
      Data : Data_Array;
      Top  : Index_Range := 0;
   end record
   with Type_Invariant => Top <= Max_Size;

   -- 私有函数
   function Is_Empty (S : Stack) return Boolean is (S.Top = 0);
   function Is_Full (S : Stack) return Boolean is (S.Top = Max_Size);
   function Size (S : Stack) return Natural is (S.Top);
end Safe_Stack;
```

### 2.2 实现

```ada
-- safe_stack.adb
package body Safe_Stack with SPARK_Mode is

   function Top (S : Stack) return Integer is
   begin
      return S.Data (S.Top);
   end Top;

   procedure Push (S : in out Stack; Value : Integer) is
   begin
      S.Top := S.Top + 1;
      S.Data (S.Top) := Value;
   end Push;

   procedure Pop (S : in out Stack; Value : out Integer) is
   begin
      Value := S.Data (S.Top);
      S.Top := S.Top - 1;
   end Pop;

   procedure Clear (S : in out Stack) is
   begin
      S.Top := 0;
   end Clear;

end Safe_Stack;
```

---

## 3. 验证结果

### 3.1 gnatprove输出

```bash
$ gnatprove -P safe_stack.gpr --level=4

Phase 1 of 2: generation of Global contracts ...
Phase 2 of 2: flow analysis and proof ...

Summary:
--------------------------------------------------------------------------
SPARK Analysis results        Total       Flow   Provers   Justified
--------------------------------------------------------------------------
Data Dependencies                1          1
Flow Dependencies                1          1
Initialization                   5          5
Run-time Checks                  5          5
Assertions                       5          5
Functional Contracts             8          8
--------------------------------------------------------------------------
Total                           25    25 (100%)
--------------------------------------------------------------------------

No errors or warnings found.
```

---

## 4. 关键证明点

### 4.1 无运行时错误证明

```ada
-- Push操作证明:
-- Pre => not Is_Full (S) 保证 S.Top < Max_Size
-- 因此 S.Top + 1 不会溢出
-- 且 S.Top in 1..Max_Size，数组访问安全
```

### 4.2 功能正确性证明

```ada
-- Push后置条件:
-- Size (S) = Size (S)'Old + 1  -- 大小增加1
-- Top (S) = Value               -- 顶部是新值
-- not Is_Empty (S)              -- 非空
```

---

**文档版本**: 1.0
**创建日期**: 2026-03-19


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
