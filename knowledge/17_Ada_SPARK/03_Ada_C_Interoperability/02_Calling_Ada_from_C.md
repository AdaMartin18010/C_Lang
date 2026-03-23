# 从C调用Ada代码

## 1. 概述

### 1.1 为什么从C调用Ada？

```
典型场景:
├── 逐步迁移遗留C代码到Ada
├── 在C项目中使用Ada的安全关键模块
├── 利用Ada的SPARK验证能力
├── 复用Ada的实时任务功能
└── 混合语言系统开发
```

### 1.2 基本机制

```ada
-- Ada侧: 导出函数给C
procedure Ada_Function (X : Integer)
   with Export, Convention => C, External_Name => "ada_function";

-- C侧: 声明并调用
extern void ada_function(int x);
```

---

## 2. 导出Ada函数到C

### 2.1 简单函数导出

**Ada代码 (math_ops.ads)**:

```ada
package Math_Ops with SPARK_Mode is
   -- 导出的函数必须在包级别声明

   function Safe_Add (A, B : Integer) return Integer
      with Export,
           Convention => C,
           External_Name => "safe_add",
           Pre => (if A > 0 and B > 0 then A <= Integer'Last - B
                   elsif A < 0 and B < 0 then A >= Integer'First - B);

   function Factorial (N : Integer) return Integer
      with Export,
           Convention => C,
           External_Name => "ada_factorial",
           Pre => N >= 0 and N <= 12;  -- 防止溢出

private
   pragma SPARK_Mode (Off);  -- 实现可以用非SPARK
end Math_Ops;
```

**Ada实现 (math_ops.adb)**:

```ada
package body Math_Ops is
   function Safe_Add (A, B : Integer) return Integer is
   begin
      return A + B;
   end Safe_Add;

   function Factorial (N : Integer) return Integer is
      Result : Integer := 1;
   begin
      for I in 2 .. N loop
         Result := Result * I;
      end loop;
      return Result;
   end Factorial;
end Math_Ops;
```

**C代码 (main.c)**:

```c
#include <stdio.h>

// 声明Ada函数
extern int safe_add(int a, int b);
extern int ada_factorial(int n);

int main() {
    int a = 10, b = 20;

    // 调用Ada函数
    int sum = safe_add(a, b);
    printf("Safe add: %d + %d = %d\n", a, b, sum);

    int fact = ada_factorial(5);
    printf("Factorial(5) = %d\n", fact);

    return 0;
}
```

### 2.2 编译链接

```bash
# 编译Ada代码 (生成对象文件和ALI文件)
gcc -c math_ops.adb

# 编译C代码
gcc -c main.c

# 链接 (使用gnatbind和gnatlink)
gnatbind math_ops.ali
gnatlink math_ops.ali main.o -o main

# 或者使用gnatmake一键编译
gnatmake math_ops.adb main.c -o main
```

---

## 3. 处理Ada数据结构

### 3.1 数组传递

**Ada规范**:

```ada
package Array_Ops is
   type Int_Array is array (Natural range <>) of Integer;

   procedure Sum_Array (
      Arr : in Int_Array;
      Len : in Natural;
      Result : out Integer
   ) with Export,
        Convention => C,
        External_Name => "sum_array";
end Array_Ops;

package body Array_Ops is
   procedure Sum_Array (
      Arr : in Int_Array;
      Len : in Natural;
      Result : out Integer
   ) is
   begin
      Result := 0;
      for I in 0 .. Len - 1 loop
         Result := Result + Arr (I);
      end loop;
   end Sum_Array;
end Array_Ops;
```

**C代码**:

```c
#include <stdio.h>

extern void sum_array(int *arr, int len, int *result);

int main() {
    int arr[] = {1, 2, 3, 4, 5};
    int result;

    sum_array(arr, 5, &result);
    printf("Sum = %d\n", result);  // 输出: 15

    return 0;
}
```

### 3.2 结构体映射

**Ada定义**:

```ada
package Data_Types is
   type Point is record
      X, Y : Float;
   end record
   with Convention => C_Pass_By_Copy;

   type Rectangle is record
      Top_Left, Bottom_Right : Point;
   end record
   with Convention => C_Pass_By_Copy;

   function Area (R : Rectangle) return Float
      with Export,
           Convention => C,
           External_Name => "rect_area";
end Data_Types;
```

**C定义**:

```c
#include <stdio.h>

typedef struct {
    float x, y;
} Point;

typedef struct {
    Point top_left;
    Point bottom_right;
} Rectangle;

extern float rect_area(Rectangle r);

int main() {
    Rectangle r = {
        .top_left = {0.0, 0.0},
        .bottom_right = {10.0, 5.0}
    };

    float area = rect_area(r);
    printf("Area = %f\n", area);  // 输出: 50.0

    return 0;
}
```

---

## 4. 回调与任务

### 4.1 C回调Ada

**Ada回调包**:

```ada
package Callbacks is
   -- 定义回调类型
   type Callback_Type is access procedure (Data : Integer)
      with Convention => C;

   -- 注册回调
   procedure Register_Callback (Cb : Callback_Type)
      with Export,
           Convention => C,
           External_Name => "register_callback";

   -- 触发回调
   procedure Trigger_Event (Data : Integer)
      with Export,
           Convention => C,
           External_Name => "trigger_event";

private
   Stored_Callback : Callback_Type := null;
end Callbacks;

package body Callbacks is
   procedure Register_Callback (Cb : Callback_Type) is
   begin
      Stored_Callback := Cb;
   end Register_Callback;

   procedure Trigger_Event (Data : Integer) is
   begin
      if Stored_Callback /= null then
         Stored_Callback (Data);
      end if;
   end Trigger_Event;
end Callbacks;
```

**C代码**:

```c
#include <stdio.h>

typedef void (*callback_t)(int);

extern void register_callback(callback_t cb);
extern void trigger_event(int data);

void my_callback(int data) {
    printf("Callback called with: %d\n", data);
}

int main() {
    register_callback(my_callback);
    trigger_event(42);
    return 0;
}
```

### 4.2 从C使用Ada任务

```ada
package Task_Service is
   task type Worker is
      entry Process (Data : Integer);
      entry Stop;
   end Worker;

   type Worker_Access is access Worker;

   function Create_Worker return Worker_Access
      with Export,
           Convention => C,
           External_Name => "create_worker";

   procedure Send_Data (W : Worker_Access; Data : Integer)
      with Export,
           Convention => C,
           External_Name => "send_data";

   procedure Destroy_Worker (W : in out Worker_Access)
      with Export,
           Convention => C,
           External_Name => "destroy_worker";
end Task_Service;

package body Task_Service is
   task body Worker is
      Running : Boolean := True;
   begin
      while Running loop
         select
            accept Process (Data : Integer) do
               -- 处理数据
               Ada.Text_IO.Put_Line ("Processing: " & Integer'Image (Data));
            end Process;
         or
            accept Stop do
               Running := False;
            end Stop;
         end select;
      end loop;
   end Worker;

   function Create_Worker return Worker_Access is
   begin
      return new Worker;
   end Create_Worker;

   procedure Send_Data (W : Worker_Access; Data : Integer) is
   begin
      W.Process (Data);
   end Send_Data;

   procedure Destroy_Worker (W : in out Worker_Access) is
   begin
      W.Stop;
      -- 注意: 需要等待任务结束
   end Destroy_Worker;
end Task_Service;
```

---

## 5. 错误处理

### 5.1 返回错误码

```ada
package Error_Handling is
   type Error_Code is (Success, Invalid_Input, Overflow, Division_By_Zero)
      with Convention => C;

   for Error_Code use (
      Success => 0,
      Invalid_Input => -1,
      Overflow => -2,
      Division_By_Zero => -3
   );

   function Safe_Divide (A, B : Integer; Result : out Integer)
      return Error_Code
      with Export,
           Convention => C,
           External_Name => "safe_divide";
end Error_Handling;

package body Error_Handling is
   function Safe_Divide (A, B : Integer; Result : out Integer)
      return Error_Code is
   begin
      if B = 0 then
         return Division_By_Zero;
      end if;

      Result := A / B;
      return Success;
   end Safe_Divide;
end Error_Handling;
```

### 5.2 C端处理

```c
#include <stdio.h>

enum ErrorCode {
    SUCCESS = 0,
    INVALID_INPUT = -1,
    OVERFLOW = -2,
    DIVISION_BY_ZERO = -3
};

extern int safe_divide(int a, int b, int *result);

int main() {
    int result;
    int err;

    err = safe_divide(10, 0, &result);
    if (err != SUCCESS) {
        printf("Error: Division by zero\n");
    }

    err = safe_divide(10, 2, &result);
    if (err == SUCCESS) {
        printf("Result: %d\n", result);
    }

    return 0;
}
```

---

## 6. 完整项目示例

### 6.1 项目结构

```
project/
├── src/
│   ├── main.c
│   ├── math_ops.ads
│   └── math_ops.adb
└── Makefile
```

### 6.2 Makefile

```makefile
# Makefile for Ada-C mixed project

CC = gcc
GNATMAKE = gnatmake
GNATBIND = gnatbind
GNATLINK = gnatlink

CFLAGS = -g -Wall
ADAFLAGS = -g -gnatwa

SRCDIR = src
OBJDIR = obj
BINDIR = bin

ADA_SRCS = $(wildcard $(SRCDIR)/*.adb)
C_SRCS = $(wildcard $(SRCDIR)/*.c)

TARGET = $(BINDIR)/main

.PHONY: all clean

all: $(TARGET)

# 编译Ada代码并生成ALI文件
$(OBJDIR)/math_ops.o: $(SRCDIR)/math_ops.adb $(SRCDIR)/math_ops.ads
 @mkdir -p $(OBJDIR)
 $(GNATMAKE) -c $(ADAFLAGS) -D $(OBJDIR) $<

# 编译C代码
$(OBJDIR)/main.o: $(SRCDIR)/main.c
 @mkdir -p $(OBJDIR)
 $(CC) $(CFLAGS) -c -o $@ $<

# 链接
$(TARGET): $(OBJDIR)/math_ops.o $(OBJDIR)/main.o
 @mkdir -p $(BINDIR)
 cd $(OBJDIR) && $(GNATBIND) math_ops.ali
 cd $(OBJDIR) && $(GNATLINK) math_ops.ali main.o -o ../$(TARGET)

clean:
 rm -rf $(OBJDIR) $(BINDIR)
```

---

## 7. 最佳实践

### 7.1 类型安全清单

- [ ] 所有导出函数使用C调用约定
- [ ] 数组参数同时传递长度
- [ ] 指针参数检查null
- [ ] 枚举类型显式指定值
- [ ] 记录类型使用Convention => C

### 7.2 性能优化

```ada
-- 内联小函数
pragma Inline (Safe_Add);

-- 避免动态分配
-- 使用静态数组而非access类型
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
