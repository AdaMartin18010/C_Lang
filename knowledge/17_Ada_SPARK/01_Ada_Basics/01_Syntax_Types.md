# Ada语法与强类型系统

## 1. Ada程序基本结构

### 1.1 Hello World

```ada
-- hello.adb
with Ada.Text_IO;  -- 导入包（类似C的#include）

procedure Hello is  -- 主程序入口
begin
   Ada.Text_IO.Put_Line ("Hello, World!");  -- 语句以分号结束
end Hello;
```

对比C语言：

```c
#include <stdio.h>

int main(void) {
    printf("Hello, World!\n");
    return 0;
}
```

**关键区别**:

- Ada使用 `procedure` 而非 `int main`
- 包导入使用 `with` 而非 `#include`
- 字符串使用双引号 `"` 而非C的字符串语法
- 语句结束用 `;` 但块结构更清晰

### 1.2 程序单元结构

```ada
-- 规范文件 (package_spec.ads)
package My_Package is
   -- 公共接口声明
   procedure Public_Procedure (X : Integer);
   Max_Size : constant := 100;
end My_Package;

-- 实现文件 (package_body.adb)
package body My_Package is
   -- 私有实现
   procedure Private_Helper is
   begin
      null;  -- 空操作
   end Private_Helper;

   procedure Public_Procedure (X : Integer) is
   begin
      -- 实现
   end Public_Procedure;
end My_Package;
```

对比C语言的头文件/源文件分离：

```c
// my_package.h
#ifndef MY_PACKAGE_H
#define MY_PACKAGE_H
void public_procedure(int x);
#define MAX_SIZE 100
#endif

// my_package.c
#include "my_package.h"
static void private_helper(void) { }
void public_procedure(int x) { }
```

---

## 2. 强类型系统

### 2.1 基本类型声明

```ada
-- Ada强类型：不同类型即使表示相同也不兼容

-- 定义距离类型（基于Float）
type Meters is new Float;
type Kilometers is new Float;

-- 定义时间类型
type Seconds is new Float;
type Hours is new Float;

-- 使用示例
Dist_M : Meters := 100.0;
Dist_KM : Kilometers := 0.1;
T : Seconds := 10.0;

-- Speed : Float := Dist_M / T;  -- 编译错误！类型不匹配
Speed_M_S : Meters := Meters (Float (Dist_M) / Float (T));  -- 必须显式转换
```

对比C语言的弱类型：

```c
typedef float Meters;
typedef float Kilometers;
typedef float Seconds;

Meters dist_m = 100.0f;
Seconds t = 10.0f;

// C允许隐式转换，危险！
float speed = dist_m / t;  // 编译通过，但单位可能错误
```

### 2.2 范围约束类型

```ada
-- 定义有限范围的整数类型
type Percentage is range 0 .. 100;
type Small_Int is range -10 .. 10;
type Index is range 1 .. 1000;

-- 编译器在赋值时检查范围
P : Percentage := 50;     -- OK
-- P := 150;              -- 编译错误！超出范围
-- P := P + 60;           -- 运行时检查，可能溢出

-- 浮点数范围
type Probability is new Float range 0.0 .. 1.0;
```

对比C语言：

```c
typedef unsigned char Percentage;  // 只能依赖命名约定

Percentage p = 50;
p = 150;  // C允许！导致数据损坏
```

### 2.3 子类型

```ada
-- 子类型：基于现有类型的约束，保持兼容性

subtype Natural is Integer range 0 .. Integer'Last;
subtype Positive is Integer range 1 .. Integer'Last;

-- 数组索引常用子类型
subtype Row_Index is Integer range 1 .. 100;
subtype Col_Index is Integer range 1 .. 50;

Matrix : array (Row_Index, Col_Index) of Float;
-- 类型系统保证索引始终在有效范围
```

### 2.4 枚举类型

```ada
-- 强类型枚举
type Color is (Red, Green, Blue);
type Day is (Mon, Tue, Wed, Thu, Fri, Sat, Sun);
type Log_Level is (Debug, Info, Warning, Error, Fatal);

-- 使用枚举
C : Color := Red;
Today : Day := Mon;

-- 枚举属性（编译器自动提供）
Color'First      -- Red
Color'Last       -- Blue
Color'Succ (Red) -- Green
Day'Pos (Mon)    -- 0
Day'Val (3)      -- Thu
```

对比C语言枚举：

```c
enum Color { Red, Green, Blue };
enum Color c = Red;

// C枚举本质上是整数
int x = Red;  // 允许！类型安全弱
c = 5;        // 允许！危险
```

---

## 3. 复合类型

### 3.1 数组

```ada
-- 数组声明：索引类型 + 元素类型

-- 一维数组
type Vector is array (1 .. 10) of Float;
type Byte_Array is array (Natural range <>) of Byte;  -- 不定长

-- 二维数组
type Matrix is array (1 .. 3, 1 .. 3) of Float;

-- 带命名索引的数组
type Week_Temperature is array (Day) of Float;
Temps : Week_Temperature := (Mon => 20.5, Tue => 22.0, others => 0.0);

-- 数组操作（自带边界检查）
V : Vector := (others => 0.0);  -- 初始化为0
V (1) := 3.14;                   -- 安全访问
-- V (15) := 1.0;                -- 编译错误！索引越界

-- 数组切片
Sub_V : Vector := V (1 .. 5);  -- 前5个元素
```

对比C语言数组：

```c
float v[10];
v[0] = 3.14f;
v[15] = 1.0f;  // C允许！缓冲区溢出
```

### 3.2 记录类型（结构体）

```ada
-- 记录定义（类似C的结构体）
type Point is record
   X, Y : Float;
end record;

type Person is record
   Name    : String (1 .. 20);
   Age     : Natural;
   Email   : String (1 .. 50);
end record;

-- 记录初始化
P : Point := (X => 1.0, Y => 2.0);
Origin : Point := (0.0, 0.0);  -- 位置参数

-- 记录访问
P.X := 3.0;
```

### 3.3 变体记录（带判别式）

```ada
-- 类型安全的联合体
type Shape_Kind is (Circle, Rectangle, Triangle);

type Shape (Kind : Shape_Kind) is record
   case Kind is
      when Circle =>
         Radius : Float;
      when Rectangle =>
         Width, Height : Float;
      when Triangle =>
         A, B, C : Float;
   end case;
end record;

-- 使用变体记录
C : Shape (Circle) := (Kind => Circle, Radius => 5.0);
-- C.Width := 10.0;  -- 编译错误！Circle没有Width字段
```

对比C语言联合体：

```c
union Shape {
    struct { float radius; } circle;
    struct { float w, h; } rect;
};
// C无法防止访问错误的联合成员
```

---

## 4. 控制流

### 4.1 条件语句

```ada
-- if语句
if X > 0 then
   Put_Line ("Positive");
elsif X < 0 then
   Put_Line ("Negative");
else
   Put_Line ("Zero");
end if;

-- 条件表达式（类似C的三元运算符）
Sign : Integer := (if X > 0 then 1 elsif X < 0 then -1 else 0);

-- case语句（必须覆盖所有可能值）
case Today is
   when Mon .. Fri =>
      Put_Line ("Weekday");
   when Sat | Sun =>
      Put_Line ("Weekend");
end case;  -- 编译器检查穷尽性
```

### 4.2 循环

```ada
-- for循环（范围确定）
for I in 1 .. 10 loop
   Put_Line (Integer'Image (I));
end loop;

-- 反向循环
for I in reverse 1 .. 10 loop
   -- 10, 9, ..., 1
end loop;

-- 数组迭代
for C of Color loop
   Process (C);
end loop;

-- while循环
Count : Natural := 0;
while Count < 100 loop
   Count := Count + 1;
end loop;

-- 无限循环（必须有exit）
loop
   Get_Input (X);
   exit when X = 0;  -- 条件退出
   Process (X);
end loop;
```

对比C语言循环：

```c
// C的for循环更灵活但更危险
for (int i = 0; i <= 10; i++) { }  // 容易写错边界
for (int i = 10; i >= 0; i--) { }  // i >= 0 总是true（无符号）！
```

---

## 5. 子程序

### 5.1 过程（Procedure）

```ada
-- 过程定义（无返回值，类似void函数）
procedure Swap (A, B : in out Integer) is
   Temp : Integer;
begin
   Temp := A;
   A := B;
   B := Temp;
end Swap;

-- 调用
X, Y : Integer := (1, 2);
Swap (X, Y);  -- 现在X=2, Y=1

-- 参数模式
-- in     : 只读输入（默认）
-- out    : 只写输出
-- in out : 读写
```

### 5.2 函数（Function）

```ada
-- 函数定义（必须返回值）
function Square (X : Float) return Float is
begin
   return X * X;
end Square;

-- 表达式函数（简洁定义）
function Cube (X : Float) return Float is (X * X * X);

-- 使用
Area : Float := Square (5.0);  -- 25.0

-- 前置/后置条件（合约编程）
function Sqrt (X : Float) return Float
   with Pre => X >= 0.0,      -- 前置条件
        Post => Sqrt'Result >= 0.0 and then
                abs (Sqrt'Result * Sqrt'Result - X) < 0.001
is
begin
   -- 实现
   return X ** 0.5;
end Sqrt;

-- Y := Sqrt (-1.0);  -- 运行时错误！违反Pre条件
```

对比C语言函数：

```c
// C没有内置合约检查
float sqrt(float x) {
    // 必须手动检查
    if (x < 0) { /* 错误处理 */ }
    return sqrtf(x);
}
```

---

## 6. 类型安全对比总结

### 6.1 常见错误防护对比

| 错误类型 | C语言 | Ada |
|---------|-------|-----|
| 缓冲区溢出 | ❌ 无防护 | ✅ 范围检查 |
| 整数溢出 | ❌ 未定义行为 | ✅ 运行时检查 |
| 类型混淆 | ❌ 允许隐式转换 | ✅ 强类型分离 |
| 空指针解引用 | ❌ 运行时崩溃 | ✅ 可访问性检查 |
| 数组越界 | ❌ 无检查 | ✅ 索引约束检查 |
| 未初始化变量 | ⚠️ 警告（有时） | ✅ 强制初始化 |
| 悬垂指针 | ❌ 允许使用 | ✅ 访问类型受限 |

### 6.2 学习建议

从C迁移到Ada的关键心态转变：

1. **拥抱约束**: Ada的限制是为了安全，不是阻碍
2. **显式优于隐式**: 类型转换必须显式写出
3. **编译时投资**: 花更多时间让编译器帮你检查
4. **合约思维**: 用Pre/Post条件思考函数契约

---

## 7. 后续学习路径

```
01_Syntax_Types.md (本文档)
    ↓
02_Packages_Modules.md - 包系统与模块化
    ↓
03_Task_Concurrency.md - 任务与并发编程
    ↓
04_Exceptions_Contracts.md - 异常与合约编程
    ↓
SPARK验证 → 01_SPARK_Overview.md
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
