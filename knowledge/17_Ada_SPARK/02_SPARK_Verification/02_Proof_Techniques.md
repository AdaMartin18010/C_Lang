# SPARK证明技术深入

## 1. 证明等级详解

### 1.1 Stone (流分析)

```ada
-- Stone级别自动检查的内容:
-- 1. 所有变量在使用前已初始化
-- 2. 无无效代码 (死代码)
-- 3. 参数模式正确性

procedure Stone_Example (X : in Integer; Y : out Integer) with SPARK_Mode is
   Z : Integer;  -- 警告: 可能未初始化
begin
   if X > 0 then
      Z := X;
   end if;

   -- 错误: Z可能未初始化
   Y := Z;  -- gnatprove会报告这里
end Stone_Example;

-- 修复后
procedure Stone_Fixed (X : in Integer; Y : out Integer) with SPARK_Mode is
   Z : Integer := 0;  -- 初始化
begin
   if X > 0 then
      Z := X;
   end if;

   Y := Z;  -- OK
end Stone_Fixed;
```

### 1.2 Bronze (运行时错误消除)

```ada
-- Bronze级别证明: 无运行时错误
-- - 无除零
-- - 无数组越界
-- - 无整数溢出
-- - 无断言失败

procedure Bronze_Example (X, Y : Integer; Arr : in out Int_Array)
   with SPARK_Mode
is
begin
   -- 需要Pre条件确保Y/=0
   Arr (1) := X / Y;  -- 可能除零错误

   -- 需要检查索引范围
   Arr (X) := 0;      -- 可能越界
end Bronze_Example;

-- 修复后
procedure Bronze_Fixed (X, Y : Integer; Arr : in out Int_Array)
   with SPARK_Mode,
        Pre => Y /= 0 and X in Arr'Range
is
begin
   Arr (1) := X / Y;  -- OK: Y/=0
   Arr (X) := 0;      -- OK: X在范围内
end Bronze_Fixed;
```

### 1.3 Silver (功能正确性)

```ada
-- Silver级别: 证明功能正确性
package Silver_Example with SPARK_Mode is
   function Factorial (N : Natural) return Natural
      with
         Pre  => N <= 12,  -- 防止溢出
         Post => (if N = 0 then Factorial'Result = 1
                  else Factorial'Result = N * Factorial (N - 1));

   procedure Increment_All (Arr : in out Int_Array)
      with
         Post => (for all I in Arr'Range =>
                  Arr (I) = Arr'Old (I) + 1);
end Silver_Example;

package body Silver_Example with SPARK_Mode is
   function Factorial (N : Natural) return Natural is
   begin
      if N = 0 then
         return 1;
      else
         return N * Factorial (N - 1);
      end if;
   end Factorial;

   procedure Increment_All (Arr : in out Int_Array) is
   begin
      for I in Arr'Range loop
         Arr (I) := Arr (I) + 1;

         -- 循环不变式
         pragma Loop_Invariant
           (for all J in Arr'First .. I =>
            Arr (J) = Arr'Loop_Entry (J) + 1);
         pragma Loop_Invariant
           (for all J in I + 1 .. Arr'Last =>
            Arr (J) = Arr'Loop_Entry (J));
      end loop;
   end Increment_All;
end Silver_Example;
```

---

## 2. 循环验证技术

### 2.1 循环不变式

```ada
-- 循环不变式: 在每次循环迭代开始时保持为真的条件

package Loop_Verification with SPARK_Mode is
   type Int_Array is array (Positive range <>) of Integer;

   function Sum (Arr : Int_Array) return Integer
      with Post => (if Arr'Length = 0 then Sum'Result = 0
                    else Sum'Result = Sum'Result'Old);

private
   function Partial_Sum (Arr : Int_Array; Last : Positive) return Integer
      with Pre => Last in Arr'Range,
           Post => Partial_Sum'Result =
                   (for all I in Arr'First .. Last =>
                    Partial_Sum'Result >= Arr (I));  -- 简化示例
end Loop_Verification;

package body Loop_Verification with SPARK_Mode is
   function Sum (Arr : Int_Array) return Integer is
      Result : Integer := 0;
   begin
      for I in Arr'Range loop
         -- 不变式1: Result等于已处理元素的和
         pragma Loop_Invariant
           (Result = (for all J in Arr'First .. I - 1 =>
                      Result = Result'Loop_Entry + Arr (J)));

         -- 不变式2: 未处理元素保持不变
         pragma Loop_Invariant
           (for all J in I .. Arr'Last => Arr (J) = Arr'Loop_Entry (J));

         Result := Result + Arr (I);
      end loop;

      return Result;
   end Sum;
end Loop_Verification;
```

### 2.2 循环变体 (终止证明)

```ada
-- 循环变体: 证明循环会终止

package Termination with SPARK_Mode is
   procedure Binary_Search (Arr : Sorted_Array; Target : Integer;
                            Found : out Boolean; Index : out Natural)
      with
         Pre  => Is_Sorted (Arr),
         Post => (Found and then Index in Arr'Range and then
                  Arr (Index) = Target)
                 or else
                 (not Found and then
                  (for all I in Arr'Range => Arr (I) /= Target));
end Termination;

package body Termination with SPARK_Mode is
   procedure Binary_Search (Arr : Sorted_Array; Target : Integer;
                            Found : out Boolean; Index : out Natural) is
      Low  : Natural := Arr'First;
      High : Natural := Arr'Last;
   begin
      Found := False;
      Index := Arr'First;

      while Low <= High loop
         -- 循环变体: High - Low 递减，确保终止
         pragma Loop_Variant (Decreases => High - Low);

         -- 不变式: 如果目标存在，必在[Low, High]内
         pragma Loop_Invariant
           (for all I in Arr'Range =>
            (if Arr (I) = Target then I in Low .. High));

         Mid := Low + (High - Low) / 2;

         if Arr (Mid) = Target then
            Found := True;
            Index := Mid;
            return;
         elsif Arr (Mid) < Target then
            Low := Mid + 1;
         else
            High := Mid - 1;
         end if;
      end loop;
   end Binary_Search;
end Termination;
```

---

## 3. 高级证明模式

### 3.1 归纳类型证明

```ada
package Inductive_Types with SPARK_Mode is
   -- 链表定义
   type List_Node;
   type List is access List_Node;
   type List_Node is record
      Value : Integer;
      Next  : List;
   end record;

   function Length (L : List) return Natural
      with Post => Length'Result >= 0;

   function Contains (L : List; V : Integer) return Boolean
      with Post => Contains'Result =
                   (if L = null then False
                    else L.Value = V or Contains (L.Next, V));

private
   pragma Annotate (GNATprove, Terminating, Length);
   pragma Annotate (GNATprove, Terminating, Contains);
end Inductive_Types;
```

### 3.2 分离逻辑风格

```ada
-- 使用Array_Ptr模拟指针
package Pointer_Proof with SPARK_Mode is
   type Array_Ptr is private;

   function Get (P : Array_Ptr; I : Natural) return Integer
      with Pre => I < Size (P);

   procedure Set (P : in out Array_Ptr; I : Natural; V : Integer)
      with Pre => I < Size (P);

   function Size (P : Array_Ptr) return Natural;

private
   type Int_Array is array (Natural range <>) of Integer;
   type Array_Ptr is access Int_Array;
end Pointer_Proof;
```

---

## 4. 证明策略与调试

### 4.1 gnatprove选项

```bash
# 基础验证
gnatprove -P project.gpr

# 增加证明努力
gnatprove -P project.gpr --level=4

# 特定文件验证
gnatprove -P project.gpr file.adb

# 只检查流分析
gnatprove -P project.gpr --mode=flow

# 只检查证明
gnatprove -P project.gpr --mode=prove

# 报告所有检查
gnatprove -P project.gpr --report=all

# 使用特定prover
gnatprove -P project.gpr --prover=z3,cvc4,altergo
```

### 4.2 常见证明失败与解决

```ada
-- 问题1: 无法证明数组访问安全
procedure Problem_1 (Arr : Int_Array; I : Integer) is
begin
   Arr (I) := 0;  -- 可能失败
end Problem_1;

-- 解决: 添加Pre条件
procedure Solution_1 (Arr : Int_Array; I : Integer)
   with Pre => I in Arr'Range
is
begin
   Arr (I) := 0;  -- OK
end Solution_1;

-- 问题2: 无法证明循环不变式
procedure Problem_2 (Arr : in out Int_Array) is
begin
   for I in Arr'Range loop
      Arr (I) := Arr (I) + 1;
      -- 缺少不变式
   end loop;
end Problem_2;

-- 解决: 添加Loop_Invariant
procedure Solution_2 (Arr : in out Int_Array)
   with Post => (for all I in Arr'Range =>
                 Arr (I) = Arr'Old (I) + 1)
is
begin
   for I in Arr'Range loop
      pragma Loop_Invariant
        (for all J in Arr'First .. I =>
         Arr (J) = Arr'Loop_Entry (J) + 1);
      pragma Loop_Invariant
        (for all J in I + 1 .. Arr'Last =>
         Arr (J) = Arr'Loop_Entry (J));

      Arr (I) := Arr (I) + 1;
   end loop;
end Solution_2;
```

---

## 5. 实战案例: 安全栈

```ada
package Safe_Stack with SPARK_Mode is
   Max_Size : constant := 100;

   type Stack is private
      with Default_Initial_Condition => Is_Empty (Stack);

   function Is_Empty (S : Stack) return Boolean;
   function Is_Full (S : Stack) return Boolean;
   function Size (S : Stack) return Natural;
   function Top (S : Stack) return Integer
      with Pre => not Is_Empty (S);

   procedure Push (S : in out Stack; V : Integer)
      with
         Pre  => not Is_Full (S),
         Post => Size (S) = Size (S)'Old + 1 and
                 Top (S) = V;

   procedure Pop (S : in out Stack; V : out Integer)
      with
         Pre  => not Is_Empty (S),
         Post => Size (S) = Size (S)'Old - 1;

private
   subtype Index_Range is Natural range 0 .. Max_Size;
   type Data_Array is array (1 .. Max_Size) of Integer;

   type Stack is record
      Data : Data_Array;
      Top  : Index_Range := 0;
   end record;

   function Is_Empty (S : Stack) return Boolean is (S.Top = 0);
   function Is_Full (S : Stack) return Boolean is (S.Top = Max_Size);
   function Size (S : Stack) return Natural is (S.Top);
end Safe_Stack;

package body Safe_Stack with SPARK_Mode is
   function Top (S : Stack) return Integer is
   begin
      return S.Data (S.Top);
   end Top;

   procedure Push (S : in out Stack; V : Integer) is
   begin
      S.Top := S.Top + 1;
      S.Data (S.Top) := V;
   end Push;

   procedure Pop (S : in out Stack; V : out Integer) is
   begin
      V := S.Data (S.Top);
      S.Top := S.Top - 1;
   end Pop;
end Safe_Stack;
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
