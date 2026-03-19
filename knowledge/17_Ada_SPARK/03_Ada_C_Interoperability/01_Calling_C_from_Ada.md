# 从Ada调用C代码

## 1. 概述

### 1.1 为什么需要Ada-C互操作？

```
典型应用场景:
├── 遗留系统迁移 (Legacy C code reuse)
├── 硬件驱动 (Device drivers written in C)
├── 第三方库 (Third-party C libraries)
├── 性能关键部分 (Performance-critical C code)
└── 逐步现代化 (Incremental modernization)
```

### 1.2 基本机制

```ada
-- Ada通过pragma Import/Export/Convention实现互操作
pragma Import (Convention => C, Entity => C_Function, External_Name => "c_function");
pragma Export (Convention => C, Entity => Ada_Procedure, External_Name => "ada_procedure");
```

---

## 2. 导入C函数

### 2.1 基本导入

**C代码 (math_ops.c)**:

```c
// C函数定义
extern "C" {
    double calculate_area(double radius);
    int process_data(const char* input, char* output, size_t out_size);
    void* allocate_buffer(size_t size);
    void free_buffer(void* ptr);
}

double calculate_area(double radius) {
    return 3.14159 * radius * radius;
}
```

**Ada绑定 (math_ops.ads)**:

```ada
-- Ada规范文件
with Interfaces.C;
with Interfaces.C.Strings;

package Math_Ops is
   -- 导入C函数
   function Calculate_Area (Radius : Interfaces.C.double)
      return Interfaces.C.double
      with
         Import => True,
         Convention => C,
         External_Name => "calculate_area";

   -- 使用更友好的Ada接口包装
   function Area (Radius : Float) return Float is
      (Float (Calculate_Area (Interfaces.C.double (Radius))));

end Math_Ops;
```

**使用示例**:

```ada
with Math_Ops;
with Ada.Float_Text_IO;

procedure Test_C_Call is
   R : Float := 5.0;
   A : Float := Math_Ops.Area (R);
begin
   Ada.Float_Text_IO.Put (A);  -- 输出: 7.85398E+01 (≈78.54)
end Test_C_Call;
```

### 2.2 字符串处理

**C代码**:

```c
int string_operation(const char* input, char* output, size_t out_size) {
    if (strlen(input) >= out_size) return -1;
    strcpy(output, "Result: ");
    strcat(output, input);
    return 0;
}
```

**Ada绑定**:

```ada
with Interfaces.C;
with Interfaces.C.Strings;

package String_Ops is
   -- 类型定义
   subtype C_String is Interfaces.C.char_array;
   type C_String_Access is access all Interfaces.C.char;

   -- 导入C函数
   function String_Operation (
      Input  : Interfaces.C.Strings.chars_ptr;
      Output : C_String_Access;
      Size   : Interfaces.C.size_t
   ) return Interfaces.C.int
   with
      Import => True,
      Convention => C,
      External_Name => "string_operation";

   -- Ada友好包装
   function Process_String (Input : String) return String;
   -- 处理字符串分配和转换

end String_Ops;

package body String_Ops is
   function Process_String (Input : String) return String is
      use Interfaces.C, Interfaces.C.Strings;

      C_Input  : chars_ptr := New_String (Input);
      Output_Size : constant size_t := Input'Length + 100;
      C_Output : aliased char_array (0 .. Output_Size);
      Result   : int;
   begin
      Result := String_Operation (C_Input, C_Output (0)'Access, Output_Size);

      Free (C_Input);

      if Result /= 0 then
         raise Constraint_Error with "String operation failed";
      end if;

      return Value (C_Output (0)'Access);
   end Process_String;
end String_Ops;
```

---

## 3. 类型映射

### 3.1 标准类型映射表

| C类型 | Ada类型 | 包 |
|-------|---------|-----|
| `char` | `Interfaces.C.char` | Interfaces.C |
| `short` | `Interfaces.C.short` | Interfaces.C |
| `int` | `Interfaces.C.int` | Interfaces.C |
| `long` | `Interfaces.C.long` | Interfaces.C |
| `long long` | `Interfaces.C.long_long` | Interfaces.C |
| `unsigned char` | `Interfaces.C.unsigned_char` | Interfaces.C |
| `unsigned int` | `Interfaces.C.unsigned` | Interfaces.C |
| `size_t` | `Interfaces.C.size_t` | Interfaces.C |
| `float` | `Interfaces.C.C_float` | Interfaces.C |
| `double` | `Interfaces.C.double` | Interfaces.C |
| `char*` | `Interfaces.C.Strings.chars_ptr` | Interfaces.C.Strings |
| `void*` | `System.Address` | System |

### 3.2 结构体映射

**C代码**:

```c
typedef struct {
    int x, y;
    double value;
    char name[32];
} Point;

double calculate_distance(Point* p1, Point* p2);
```

**Ada绑定**:

```ada
with Interfaces.C;

package Point_Pkg is
   -- C结构体映射
   type Point is record
      X, Y  : Interfaces.C.int;
      Value : Interfaces.C.double;
      Name  : Interfaces.C.char_array (0 .. 31);
   end record
   with Convention => C_Pass_By_Copy;  -- 重要!

   -- 或者使用Ada控制布局
   for Point use record
      X     at 0  range 0 .. 31;
      Y     at 4  range 0 .. 31;
      Value at 8  range 0 .. 63;
      Name  at 16 range 0 .. 255;
   end record;

   type Point_Access is access all Point;

   -- 导入C函数
   function Calculate_Distance (P1, P2 : Point_Access)
      return Interfaces.C.double
   with
      Import => True,
      Convention => C,
      External_Name => "calculate_distance";

end Point_Pkg;
```

---

## 4. 回调函数

### 4.1 从C调用Ada回调

**C代码**:

```c
typedef void (*Callback)(int value, void* user_data);

void register_callback(Callback cb, void* user_data);
void trigger_event(int value);
```

**Ada绑定**:

```ada
with Interfaces.C;

package Callback_System is
   -- 回调类型定义
   type Callback_Type is access procedure (
      Value : Interfaces.C.int;
      User_Data : System.Address
   ) with Convention => C;

   -- 导入C函数
   procedure Register_Callback (
      Cb : Callback_Type;
      User_Data : System.Address
   ) with
      Import => True,
      Convention => C,
      External_Name => "register_callback";

   procedure Trigger_Event (Value : Interfaces.C.int)
   with
      Import => True,
      Convention => C,
      External_Name => "trigger_event";

   -- Ada包装: 类型安全回调
   generic
      type User_Data_Type is private;
   package Typed_Callbacks is
      type Ada_Callback_Type is access procedure (
         Value : Integer;
         Data : User_Data_Type
      );

      procedure Register (Callback : Ada_Callback_Type; Data : User_Data_Type);
   end Typed_Callbacks;

end Callback_System;
```

---

## 5. 内存管理

### 5.1 C分配，Ada使用

```ada
with System;
with Interfaces.C;

package Memory_Interop is
   -- 导入C内存函数
   function Malloc (Size : Interfaces.C.size_t) return System.Address
   with Import => True, Convention => C, External_Name => "malloc";

   procedure Free (Ptr : System.Address)
   with Import => True, Convention => C, External_Name => "free";

   -- Ada包装: 自动管理
   generic
      type Element_Type is private;
   function C_Alloc (Count : Natural) return access Element_Type;

end Memory_Interop;

package body Memory_Interop is
   function C_Alloc (Count : Natural) return access Element_Type is
      Size : constant Interfaces.C.size_t :=
         Element_Type'Size / System.Storage_Unit * Interfaces.C.size_t (Count);
      Ptr : constant System.Address := Malloc (Size);
   begin
      if Ptr = System.Null_Address then
         raise Storage_Error with "C malloc failed";
      end if;
      -- 注意: 实际返回需要转换
      return null;  -- 简化示例
   end C_Alloc;
end Memory_Interop;
```

---

## 6. 完整示例: 封装C库

### 6.1 C库 (mylib.c)

```c
#include <stdlib.h>
#include <string.h>

typedef struct {
    int id;
    double values[10];
    int count;
} DataSet;

DataSet* dataset_create(int id) {
    DataSet* ds = malloc(sizeof(DataSet));
    ds->id = id;
    ds->count = 0;
    return ds;
}

void dataset_add(DataSet* ds, double value) {
    if (ds->count < 10) {
        ds->values[ds->count++] = value;
    }
}

double dataset_average(const DataSet* ds) {
    if (ds->count == 0) return 0.0;
    double sum = 0;
    for (int i = 0; i < ds->count; i++) sum += ds->values[i];
    return sum / ds->count;
}

void dataset_free(DataSet* ds) {
    free(ds);
}
```

### 6.2 Ada绑定 (mylib_binding.ads)

```ada
with Interfaces.C;
with Ada.Finalization;

package MyLib_Binding is
   -- C结构体定义
   type C_Double_Array is array (0 .. 9) of Interfaces.C.double
      with Convention => C;

   type DataSet_Rec is record
      Id     : Interfaces.C.int;
      Values : C_Double_Array;
      Count  : Interfaces.C.int;
   end record
   with Convention => C;

   type DataSet_Access is access all DataSet_Rec;

   -- C函数导入
   function Dataset_Create (Id : Interfaces.C.int) return DataSet_Access
   with Import => True, Convention => C, External_Name => "dataset_create";

   procedure Dataset_Add (Ds : DataSet_Access; Value : Interfaces.C.double)
   with Import => True, Convention => C, External_Name => "dataset_add";

   function Dataset_Average (Ds : DataSet_Access) return Interfaces.C.double
   with Import => True, Convention => C, External_Name => "dataset_average";

   procedure Dataset_Free (Ds : DataSet_Access)
   with Import => True, Convention => C, External_Name => "dataset_free";

   -- Ada面向对象包装
   type DataSet is new Ada.Finalization.Limited_Controlled with private;

   function Create (Id : Integer) return DataSet;
   procedure Add (Ds : in out DataSet; Value : Float);
   function Average (Ds : DataSet) return Float;

private
   type DataSet is new Ada.Finalization.Limited_Controlled with record
      Handle : DataSet_Access := null;
   end record;

   overriding procedure Finalize (Ds : in out DataSet);
end MyLib_Binding;

-- 实现
package body MyLib_Binding is
   function Create (Id : Integer) return DataSet is
   begin
      return Result : DataSet do
         Result.Handle := Dataset_Create (Interfaces.C.int (Id));
      end return;
   end Create;

   procedure Add (Ds : in out DataSet; Value : Float) is
   begin
      Dataset_Add (Ds.Handle, Interfaces.C.double (Value));
   end Add;

   function Average (Ds : DataSet) return Float is
   begin
      return Float (Dataset_Average (Ds.Handle));
   end Average;

   procedure Finalize (Ds : in out DataSet) is
   begin
      if Ds.Handle /= null then
         Dataset_Free (Ds.Handle);
         Ds.Handle := null;
      end if;
   end Finalize;
end MyLib_Binding;
```

---

## 7. 最佳实践

### 7.1 安全检查清单

| 检查项 | 说明 | 优先级 |
|--------|------|--------|
| 空指针检查 | C返回的指针可能为null | 🔴 高 |
| 缓冲区大小 | 确保缓冲区足够大 | 🔴 高 |
| 字符串终止 | C字符串需要null终止 | 🟠 中 |
| 内存释放 | 明确释放责任 | 🟠 中 |
| 线程安全 | C代码可能非线程安全 | 🟡 低 |

### 7.2 推荐模式

```ada
-- 模式1: 使用受控类型自动管理资源
-- 模式2: 使用泛型提供类型安全
-- 模式3: 包装函数提供Ada风格接口
-- 模式4: 使用SPARK验证关键绑定
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
