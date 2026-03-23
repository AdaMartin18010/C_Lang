---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../../01_Core_Knowledge_System/05_Engineering_Layer/README.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../../02_Formal_Semantics_and_Physics/README.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../../03_System_Technology_Domains/README.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../../04_Industrial_Scenarios/README.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../../06_Thinking_Representation/README.md) | 核心关联 | 思维表征基础 |
﻿# Ada异常处理与合约编程

## 1. 异常处理机制

### 1.1 概述

Ada提供结构化异常处理机制，用于处理运行时错误和异常情况：

```ada
begin
   -- 正常代码
exception
   when Exception_1 =>
      -- 处理异常1
   when Exception_2 =>
      -- 处理异常2
   when others =>
      -- 处理其他所有异常
end;
```

### 1.2 预定义异常

| 异常 | 触发条件 |
|------|---------|
| `Constraint_Error` | 违反约束条件（范围越界、数组越界等） |
| `Program_Error` | 程序逻辑错误（如非法返回） |
| `Storage_Error` | 内存分配失败 |
| `Tasking_Error` | 任务通信错误 |
| `Numeric_Error` | 数值错误（除零、溢出） |
| `End_Error` | 文件结束 |
| `Data_Error` | 数据格式错误 |

### 1.3 异常处理示例

```ada
with Ada.Text_IO;
with Ada.Integer_Text_IO;

procedure Exception_Demo is
   X, Y, Result : Integer;
begin
   Ada.Text_IO.Put ("Enter dividend: ");
   Ada.Integer_Text_IO.Get (X);

   Ada.Text_IO.Put ("Enter divisor: ");
   Ada.Integer_Text_IO.Get (Y);

   begin
      Result := X / Y;  -- 可能触发Constraint_Error
      Ada.Text_IO.Put_Line ("Result: " & Integer'Image (Result));

   exception
      when Constraint_Error =>
         Ada.Text_IO.Put_Line ("Error: Division by zero!");
         Result := 0;
   end;

exception
   when Ada.IO_Exceptions.Data_Error =>
      Ada.Text_IO.Put_Line ("Error: Invalid input format!");
   when others =>
      Ada.Text_IO.Put_Line ("Error: Unknown error occurred!");
end Exception_Demo;
```

### 1.4 用户定义异常

```ada
package Bank_Account is
   -- 定义异常
   Insufficient_Funds : exception;
   Account_Frozen : exception;
   Invalid_Amount : exception;

   type Account is private;

   procedure Withdraw (Acc : in out Account; Amount : Positive);
   procedure Deposit (Acc : in out Account; Amount : Positive);
   function Get_Balance (Acc : Account) return Natural;

private
   type Account is record
      Balance : Natural := 0;
      Frozen  : Boolean := False;
   end record;
end Bank_Account;

package body Bank_Account is
   procedure Withdraw (Acc : in out Account; Amount : Positive) is
   begin
      if Acc.Frozen then
         raise Account_Frozen with "Account is frozen";
      end if;

      if Amount > Acc.Balance then
         raise Insufficient_Funds with
            "Balance: " & Natural'Image (Acc.Balance) &
            ", Requested: " & Positive'Image (Amount);
      end if;

      Acc.Balance := Acc.Balance - Amount;
   end Withdraw;

   procedure Deposit (Acc : in out Account; Amount : Positive) is
   begin
      if Acc.Frozen then
         raise Account_Frozen;
      end if;

      -- 检查溢出
      if Natural'Last - Acc.Balance < Amount then
         raise Invalid_Amount with "Deposit would cause overflow";
      end if;

      Acc.Balance := Acc.Balance + Amount;
   end Deposit;

   function Get_Balance (Acc : Account) return Natural is
   begin
      return Acc.Balance;
   end Get_Balance;
end Bank_Account;

-- 使用示例
with Bank_Account;
with Ada.Text_IO;

procedure Use_Bank is
   Acc : Bank_Account.Account;
begin
   Bank_Account.Deposit (Acc, 100);

   begin
      Bank_Account.Withdraw (Acc, 200);  -- 会触发异常
   exception
      when Bank_Account.Insufficient_Funds =>
         Ada.Text_IO.Put_Line ("Not enough money!");
   end;

exception
   when E : others =>
      Ada.Text_IO.Put_Line ("Exception: " & Ada.Exceptions.Exception_Message (E));
end Use_Bank;
```

---

## 2. 高级异常处理

### 2.1 异常传播

```ada
procedure Level_3 is
begin
   raise Constraint_Error with "Error at level 3";
exception
   when others =>
      Ada.Text_IO.Put_Line ("Handled at level 3");
      raise;  -- 重新抛出
end Level_3;

procedure Level_2 is
begin
   Level_3;
exception
   when others =>
      Ada.Text_IO.Put_Line ("Handled at level 2");
      raise;  -- 继续传播
end Level_2;

procedure Level_1 is
begin
   Level_2;
exception
   when E : others =>
      Ada.Text_IO.Put_Line ("Handled at level 1: " &
         Ada.Exceptions.Exception_Message (E));
end Level_1;
```

### 2.2 异常信息获取

```ada
with Ada.Exceptions;

procedure Exception_Info is
begin
   -- 某些可能出错的代码
   null;
exception
   when E : others =>
      declare
         Name : constant String := Ada.Exceptions.Exception_Name (E);
         Msg  : constant String := Ada.Exceptions.Exception_Message (E);
      begin
         Ada.Text_IO.Put_Line ("Exception Name: " & Name);
         Ada.Text_IO.Put_Line ("Exception Message: " & Msg);
      end;
end Exception_Info;
```

---

## 3. 合约编程 (Design by Contract)

### 3.1 概述

Ada 2012引入合约（Contracts），允许在子程序中声明：

- **前置条件 (Pre)**：调用前必须为真的条件
- **后置条件 (Post)**：返回时必须为真的条件
- **类型不变式 (Type Invariant)**：类型值始终满足的条件

### 3.2 前置与后置条件

```ada
package Math_Ops is
   -- 平方根函数
   function Sqrt (X : Float) return Float
      with
         Pre  => X >= 0.0,                    -- 前置：输入必须非负
         Post => Sqrt'Result >= 0.0 and then  -- 后置：结果非负
                 abs (Sqrt'Result ** 2 - X) < 0.001;  -- 近似正确

   -- 数组查找
   function Find (Arr : Array_Type; Target : Integer) return Natural
      with
         Pre  => Arr'Length > 0,              -- 数组非空
         Post => (Find'Result in Arr'Range and then  -- 结果在范围内
                  Arr (Find'Result) = Target)        -- 找到目标
                 or else
                 (for all I in Arr'Range => Arr (I) /= Target);  -- 或不存在

   -- 交换两个变量
   procedure Swap (X, Y : in out Integer)
      with
         Post => X = Y'Old and Y = X'Old;     -- 交换后特性

   -- 'Old表示进入子程序时的值
end Math_Ops;

package body Math_Ops is
   function Sqrt (X : Float) return Float is
   begin
      -- 实现（牛顿迭代法等）
      return X ** 0.5;  -- 简化实现
   end Sqrt;

   procedure Swap (X, Y : in out Integer) is
      Temp : constant Integer := X;
   begin
      X := Y;
      Y := Temp;
   end Swap;

   function Find (Arr : Array_Type; Target : Integer) return Natural is
   begin
      for I in Arr'Range loop
         if Arr (I) = Target then
            return I;
         end if;
      end loop;
      return Arr'First;  -- 未找到
   end Find;
end Math_Ops;
```

### 3.3 类型不变式

```ada
package Safe_Strings is
   -- 非空字符串类型
   type Non_Empty_String is private
      with Type_Invariant => Length (Non_Empty_String) > 0;

   function Length (S : Non_Empty_String) return Natural;
   function Create (S : String) return Non_Empty_String
      with Pre => S'Length > 0;

   function Get (S : Non_Empty_String) return String;
   procedure Append (S : in out Non_Empty_String; C : Character);

private
   type Non_Empty_String is record
      Data : String (1 .. 100);
      Len  : Natural range 1 .. 100;
   end record;

   function Length (S : Non_Empty_String) return Natural is (S.Len);
end Safe_Strings;

package body Safe_Strings is
   function Create (S : String) return Non_Empty_String is
      Result : Non_Empty_String;
   begin
      Result.Len := S'Length;
      Result.Data (1 .. Result.Len) := S;
      return Result;
   end Create;

   function Get (S : Non_Empty_String) return String is
   begin
      return S.Data (1 .. S.Len);
   end Get;

   procedure Append (S : in out Non_Empty_String; C : Character) is
   begin
      if S.Len < 100 then
         S.Len := S.Len + 1;
         S.Data (S.Len) := C;
      end if;
   end Append;
end Safe_Strings;
```

### 3.4 默认初始条件

```ada
package Counters is
   type Counter is private
      with Default_Initial_Condition => Get (Counter) = 0;

   procedure Increment (C : in out Counter);
   function Get (C : Counter) return Natural;

private
   type Counter is record
      Value : Natural := 0;
   end record;

   function Get (C : Counter) return Natural is (C.Value);
end Counters;
```

---

## 4. SPARK合约编程

### 4.1 SPARK与Ada合约

```ada
package Math_SPARK with SPARK_Mode is
   -- SPARK中的合约会被形式化验证

   function Factorial (N : Natural) return Natural
      with
         Pre  => N <= 20,  -- 防止溢出
         Post => (if N = 0 then Factorial'Result = 1
                  else Factorial'Result = N * Factorial (N - 1));

   procedure Division (Dividend : in Integer;
                       Divisor  : in Integer;
                       Quotient : out Integer;
                       Remainder : out Integer)
      with
         Pre  => Divisor /= 0,
         Post => Dividend = Quotient * Divisor + Remainder and
                 abs Remainder < abs Divisor;

end Math_SPARK;

package body Math_SPARK with SPARK_Mode is
   function Factorial (N : Natural) return Natural is
   begin
      if N = 0 then
         return 1;
      else
         return N * Factorial (N - 1);
      end if;
   end Factorial;

   procedure Division (Dividend : in Integer;
                       Divisor  : in Integer;
                       Quotient : out Integer;
                       Remainder : out Integer) is
   begin
      Quotient := Dividend / Divisor;
      Remainder := Dividend rem Divisor;
   end Division;
end Math_SPARK;
```

### 4.2 循环不变式

```ada
package Search with SPARK_Mode is
   type Int_Array is array (Positive range <>) of Integer;

   function Linear_Search (Arr : Int_Array; Target : Integer) return Natural
      with
         Post => (Linear_Search'Result = 0 and then
                  (for all I in Arr'Range => Arr (I) /= Target))
                 or else
                 (Linear_Search'Result in Arr'Range and then
                  Arr (Linear_Search'Result) = Target);

end Search;

package body Search with SPARK_Mode is
   function Linear_Search (Arr : Int_Array; Target : Integer) return Natural is
   begin
      for I in Arr'Range loop
         -- 循环不变式
         pragma Loop_Invariant
           (for all J in Arr'First .. I - 1 => Arr (J) /= Target);

         if Arr (I) = Target then
            return I;
         end if;
      end loop;

      return 0;
   end Linear_Search;
end Search;
```

---

## 5. 最佳实践

### 5.1 异常 vs 合约

| 场景 | 推荐方法 |
|------|---------|
| 可预期的错误条件 | 前置条件 + 异常 |
| 编程错误 | 断言/合约（不捕获） |
| 外部输入错误 | 异常处理 |
| 内部逻辑验证 | 合约 |

### 5.2 检查清单

- [ ] 所有公共子程序都有适当的Pre/Post
- [ ] 关键类型有Type_Invariant
- [ ] 异常处理在适当层级
- [ ] SPARK代码无运行时错误风险

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
