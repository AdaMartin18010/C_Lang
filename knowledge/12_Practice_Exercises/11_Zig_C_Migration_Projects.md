# Zig-C 迁移项目 (Zig-C Migration Projects)

> **难度**: L3-L5 | **预计时间**: 25-40小时

---

## 项目目标

将C项目逐步迁移到Zig，学习：

- C与Zig的互操作
- Zig的内存安全特性
- 渐进式迁移策略

---

## 项目1: 链表库的Zig重写

### 原C代码

```c
// list.h
typedef struct Node {
    int data;
    struct Node *next;
} Node;

typedef struct {
    Node *head;
    Node *tail;
    size_t len;
} List;

List* list_create(void);
void list_destroy(List *list);
void list_append(List *list, int value);
bool list_remove(List *list, int value);
int* list_to_array(const List *list, size_t *len);
```

### Zig版本

```zig
// list.zig
const std = @import("std");

pub fn List(comptime T: type) type {
    return struct {
        const Self = @This();

        pub const Node = struct {
            data: T,
            next: ?*Node,
        };

        head: ?*Node,
        tail: ?*Node,
        len: usize,
        allocator: std.mem.Allocator,

        pub fn init(allocator: std.mem.Allocator) Self {
            return .{
                .head = null,
                .tail = null,
                .len = 0,
                .allocator = allocator,
            };
        }

        pub fn deinit(self: *Self) void {
            var current = self.head;
            while (current) |node| {
                current = node.next;
                self.allocator.destroy(node);
            }
            self.* = undefined;
        }

        pub fn append(self: *Self, value: T) !void {
            const node = try self.allocator.create(Node);
            node.* = .{ .data = value, .next = null };

            if (self.tail) |tail| {
                tail.next = node;
            } else {
                self.head = node;
            }
            self.tail = node;
            self.len += 1;
        }

        pub fn remove(self: *Self, value: T) bool {
            var current = &self.head;
            while (current.*) |node| {
                if (node.data == value) {
                    current.* = node.next;
                    if (self.tail == node) {
                        self.tail = if (current.* == node) null else current.*;
                    }
                    self.allocator.destroy(node);
                    self.len -= 1;
                    return true;
                }
                current = &node.next;
            }
            return false;
        }

        pub fn toArray(self: Self, allocator: std.mem.Allocator) ![]T {
            var arr = try allocator.alloc(T, self.len);
            var i: usize = 0;
            var current = self.head;
            while (current) |node| : (current = node.next) {
                arr[i] = node.data;
                i += 1;
            }
            return arr;
        }
    };
}
```

### 对比要点

| 特性 | C版本 | Zig版本 |
|:-----|:------|:--------|
| 内存安全 | 手动管理，易泄漏 | 自动释放，无泄漏 |
| 泛型支持 | void* + 宏 | comptime泛型 |
| 错误处理 | 返回值检查 | 显式错误联合 |
| 空指针 | NULL | 可选类型 `?*T` |
| 边界检查 | 无 | 默认开启 |

---

## 项目2: C项目中调用Zig库

### Zig代码（编译为静态库）

```zig
// mathlib.zig
const std = @import("std");

// 导出C兼容的函数
export fn zig_add(a: i32, b: i32) i32 {
    return a + b;
}

export fn zig_sum(arr: [*]const i32, len: usize) i32 {
    var sum: i32 = 0;
    for (0..len) |i| {
        sum += arr[i];
    }
    return sum;
}

// 复杂结构体
const Point = extern struct {
    x: f64,
    y: f64,
};

export fn zig_distance(p1: Point, p2: Point) f64 {
    const dx = p2.x - p1.x;
    const dy = p2.y - p1.y;
    return std.math.sqrt(dx * dx + dy * dy);
}

// 错误码包装
export fn zig_divide(a: i32, b: i32, result: *i32) i32 {
    if (b == 0) return -1;  // 错误码
    result.* = @divTrunc(a, b);
    return 0;  // 成功
}
```

### 编译命令

```bash
# 编译Zig为C兼容的静态库
zig build-lib mathlib.zig -static -O ReleaseSafe

# 生成头文件（手动或使用zig translate-c）
```

### C代码调用Zig库

```c
// main.c
#include <stdio.h>

// Zig生成的函数声明
int zig_add(int a, int b);
int zig_sum(const int *arr, size_t len);
double zig_distance(struct { double x; double y; } p1,
                    struct { double x; double y; } p2);
int zig_divide(int a, int b, int *result);

int main() {
    printf("zig_add(3, 5) = %d\n", zig_add(3, 5));

    int arr[] = {1, 2, 3, 4, 5};
    printf("zig_sum = %d\n", zig_sum(arr, 5));

    int result;
    if (zig_divide(10, 0, &result) == 0) {
        printf("10 / 2 = %d\n", result);
    } else {
        printf("Division error\n");
    }

    return 0;
}
```

### 链接

```bash
gcc main.c libmathlib.a -o main -lm
```

---

## 项目3: Zig中调用C库

### C库（已有代码）

```c
// clib.h
#ifndef CLIB_H
#define CLIB_H

typedef struct {
    char name[64];
    int age;
} Person;

Person* person_create(const char *name, int age);
void person_free(Person *p);
void person_greet(const Person *p);

#endif
```

```c
// clib.c
#include "clib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Person* person_create(const char *name, int age) {
    Person *p = malloc(sizeof(Person));
    strncpy(p->name, name, sizeof(p->name) - 1);
    p->name[sizeof(p->name) - 1] = '\0';
    p->age = age;
    return p;
}

void person_free(Person *p) {
    free(p);
}

void person_greet(const Person *p) {
    printf("Hello, I'm %s, %d years old.\n", p->name, p->age);
}
```

### Zig包装器

```zig
// c_wrapper.zig
const std = @import("std");

// 导入C头文件
const c = @cImport({
    @cInclude("clib.h");
});

pub const Person = extern struct {
    name: [64]u8,
    age: c_int,

    // Zig风格的方法
    pub fn create(name: []const u8, age: i32) !*Person {
        const c_name = try std.heap.c_allocator.dupeZ(u8, name);
        defer std.heap.c_allocator.free(c_name);

        const p = c.person_create(c_name.ptr, age);
        if (p == null) return error.OutOfMemory;
        return @ptrCast(p);
    }

    pub fn destroy(self: *Person) void {
        c.person_free(@ptrCast(self));
    }

    pub fn greet(self: *const Person) void {
        c.person_greet(@ptrCast(self));
    }

    pub fn getName(self: *const Person) []const u8 {
        return std.mem.sliceTo(&self.name, 0);
    }
};

// 更安全的包装
pub const SafePerson = struct {
    ptr: *Person,

    pub fn init(name: []const u8, age: i32) !SafePerson {
        return .{ .ptr = try Person.create(name, age) };
    }

    pub fn deinit(self: SafePerson) void {
        self.ptr.destroy();
    }

    pub fn greet(self: SafePerson) void {
        self.ptr.greet();
    }
};
```

### 使用示例

```zig
// main.zig
const std = @import("std");
const c_wrapper = @import("c_wrapper.zig");

pub fn main() !void {
    // 使用安全包装
    var person = try c_wrapper.SafePerson.init("Alice", 30);
    defer person.deinit();
    person.greet();

    // 直接使用（需要手动管理内存）
    const p = try c_wrapper.Person.create("Bob", 25);
    defer p.destroy();
    p.greet();
}
```

---

## 项目4: 逐步迁移策略

### 阶段1: 从C调用Zig的单元测试

```zig
// test_runner.zig - 替换C的测试框架
const std = @import("std");

// 导入被测试的C代码
extern fn legacy_function(input: i32) i32;

test "legacy function works" {
    try std.testing.expectEqual(legacy_function(5), 10);
}

test "edge cases" {
    try std.testing.expectEqual(legacy_function(0), 0);
    try std.testing.expectEqual(legacy_function(-5), -10);
}
```

### 阶段2: 用Zig重写性能关键模块

```zig
// parser.zig - 替换C解析器
const std = @import("std");

pub const Parser = struct {
    allocator: std.mem.Allocator,
    input: []const u8,
    pos: usize,

    pub fn parse(self: *Parser) !Ast {
        // Zig实现，比C更安全
    }
};
```

### 阶段3: 完整Zig重写

```zig
// main.zig
pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    // 纯Zig实现
}
```

---

## 项目5: 内存分配器对比

### C版本（malloc）

```c
void* process_data(const void *input, size_t len) {
    void *buffer = malloc(len);
    if (!buffer) return NULL;

    // 处理...

    return buffer;  // 调用者负责释放
}
```

### Zig版本（显式分配器）

```zig
fn processData(allocator: std.mem.Allocator, input: []const u8) ![]u8 {
    var buffer = try allocator.alloc(u8, input.len);
    errdefer allocator.free(buffer);  // 失败时自动释放

    // 处理...

    return buffer;  // 调用者负责释放
}

// 使用
const result = try processData(arena.allocator(), input);
defer arena.allocator().free(result);
```

---

## 项目6: 错误处理对比

### C版本

```c
ErrorCode do_something(const char *input, char **output) {
    if (!input) return ERR_NULL_INPUT;

    *output = malloc(100);
    if (!*output) return ERR_NO_MEMORY;

    FILE *f = fopen(input, "r");
    if (!f) {
        free(*output);
        return ERR_FILE_NOT_FOUND;
    }

    // ... 更多错误检查

    return SUCCESS;
}
```

### Zig版本

```zig
const Error = error{
    NullInput,
    FileNotFound,
    // NoMemory是Zig内置
};

fn doSomething(allocator: std.mem.Allocator, input: []const u8) Error![]u8 {
    if (input.len == 0) return error.NullInput;

    var buffer = try allocator.alloc(u8, 100);
    errdefer allocator.free(buffer);

    var file = std.fs.cwd().openFile(input, .{})
        catch return error.FileNotFound;
    defer file.close();

    // ... 错误自动传播

    return buffer;
}
```

---

## 迁移检查清单

- [ ] 为C代码编写Zig绑定测试
- [ ] 识别内存安全问题并用Zig重写
- [ ] 逐步替换模块，保持接口兼容
- [ ] 使用Zig的comptime优化性能关键代码
- [ ] 利用Zig的交叉编译能力简化构建
- [ ] 验证性能没有退化

---

> **返回导航**: [练习模块总览](./README.md) | [知识库总览](../README.md)
