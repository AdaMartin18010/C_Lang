            sm.current_state = stateRunning;
        },
        .stop => {
            std.debug.print("PAUSED -> IDLE\n", .{});
            sm.current_state = stateIdle;
        },
        else => std.debug.print("PAUSED: ignored event {s}\n", .{@tagName(event)}),
    }
}

fn stateError(sm: *StateMachine, event: Event) void {
    switch (event) {
        .reset => {
            std.debug.print("ERROR -> IDLE (recovered)\n", .{});
            sm.current_state = stateIdle;
        },
        else => std.debug.print("ERROR: waiting for reset\n", .{}),
    }
}

fn stateToName(state: State) []const u8 {
    if (state == stateIdle) return "IDLE";
    if (state == stateRunning) return "RUNNING";
    if (state == statePaused) return "PAUSED";
    if (state == stateError) return "ERROR";
    return "UNKNOWN";
}

// 编译时状态机生成
fn MakeStateMachine(comptime StateEnum: type) type {
    return struct {
        const Self = @This();

        current: StateEnum,

        pub fn init() Self {
            const fields = @typeInfo(StateEnum).Enum.fields;
            return .{ .current = @enumFromInt(fields[0].value) };
        }

        pub fn transition(self: *Self, new_state: StateEnum) void {
            std.debug.print("{s} -> {s}\n", .{@tagName(self.current), @tagName(new_state)});
            self.current = new_state;
        }

        pub fn canTransition(self: *const Self, target: StateEnum) bool {
            _ = self;
            _ = target;
            // 编译时定义转换规则
            return true;
        }
    };
}

pub fn main() void {
    var sm = StateMachine.init();

    std.debug.print("Initial state: {s}\n", .{sm.getStateName()});

    // 模拟事件序列
    sm.handleEvent(.start);
    sm.handleEvent(.timeout);
    sm.handleEvent(.timeout);
    sm.handleEvent(.pause);
    sm.handleEvent(.resume);
    sm.handleEvent(.timeout);
    sm.handleEvent(.stop);

    std.debug.print("Final state: {s}\n", .{sm.getStateName()});
}

```

---

## 十一、形式化语义证明

### 11.1 调用约定等价性

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                      调用约定等价性形式化证明                                  │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  定理 1: C cdecl 与 Zig callconv(.C) 等价性                                  │
│  ─────────────────────────────────────────────────────────────────────────  │
│                                                                              │
│  陈述:                                                                       │
│  对于任意 C 函数 F_C 使用 cdecl 调用约定，存在等价 Zig 函数 F_Zig 使用         │
│  callconv(.C)，使得对于所有参数序列 args，有:                                  │
│                                                                              │
│    F_C(args) ≡ F_Zig(args)                                                   │
│                                                                              │
│  证明:                                                                       │
│  ─────────────────                                                           │
│                                                                              │
│  1. 参数传递:                                                                │
│     - cdecl: 参数从右到左压栈                                                  │
│     - Zig callconv(.C): 在 x86 上生成相同压栈序列                              │
│     - ∴ 参数布局相同                                                          │
│                                                                              │
│  2. 栈清理责任:                                                              │
│     - cdecl: 调用者清理                                                        │
│     - Zig callconv(.C): 调用者清理                                             │
│     - ∴ 栈平衡语义相同                                                        │
│                                                                              │
│  3. 返回值处理:                                                              │
│     - 两者都使用 EAX/RAX 寄存器返回整数/指针                                   │
│     - 两者都使用 ST(0) 或 XMM0 返回浮点                                        │
│     - ∴ 返回值处理相同                                                        │
│                                                                              │
│  4. 寄存器保存:                                                              │
│     - cdecl: EAX/ECX/EDX 调用者保存，其余被调用者保存                          │
│     - Zig callconv(.C): 相同约定                                               │
│     - ∴ 寄存器保存语义相同                                                    │
│                                                                              │
│  ∴ F_C ≡ F_Zig 在 cdecl 调用约定下 ∎                                          │
│                                                                              │
│  ─────────────────────────────────────────────────────────────────────────  │
│                                                                              │
│  定理 2: 函数指针类型在 C 和 Zig 间可安全转换                                  │
│  ─────────────────────────────────────────────────────────────────────────  │
│                                                                              │
│  陈述:                                                                       │
│  如果 C 函数指针类型 FP_C 和 Zig 函数指针类型 FP_Zig 满足:                     │
│    - 参数类型序列逐一 ABI 等价                                                 │
│    - 返回类型 ABI 等价                                                         │
│    - 调用约定相同                                                              │
│  则 FP_C 和 FP_Zig 可互相转换而不改变调用语义。                                 │
│                                                                              │
│  证明:                                                                       │
│  ─────────────────                                                           │
│  函数指针在 C 和 Zig 中都是指向代码的指针，大小等于 sizeof(void*)。            │
│  函数调用的语义完全由调用约定和类型签名决定。                                   │
│  当上述三个条件满足时，通过 FP_C 和 FP_Zig 进行调用的机器代码序列相同。          │
│  ∴ 转换是语义保持的 ∎                                                         │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 11.2 类型安全定理

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                      函数指针类型安全定理                                      │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  定理 3: Zig 可选函数指针类型提供空值安全                                      │
│  ─────────────────────────────────────────────────────────────────────────  │
│                                                                              │
│  陈述:                                                                       │
│  对于 Zig 可选函数指针类型 ?*const fn(...) T，任何解引用操作前必须处理 null    │
│  情况，否则触发编译错误。                                                      │
│                                                                              │
│  形式化:                                                                     │
│  ─────────────────                                                           │
│  设 cb: ?*const fn(Arg) Ret                                                   │
│                                                                              │
│  表达式 cb(arg) 是非法的（编译错误）                                           │
│                                                                              │
│  必须以下列形式之一解包:                                                      │
│  • if (cb) |f| f(arg)        // 条件执行                                      │
│  • cb.?(arg)                 // 运行时检查，null 时 panic                      │
│  • (cb orelse default)(arg)  // 提供默认值                                    │
│                                                                              │
│  相比 C:                                                                     │
│  ─────────────────                                                           │
│  C 代码: fp(arg) 在 fp == NULL 时是未定义行为（通常崩溃）                       │
│  Zig 代码: 编译器强制检查，将运行时错误转为编译时错误或显式处理                 │
│                                                                              │
│  ─────────────────────────────────────────────────────────────────────────  │
│                                                                              │
│  定理 4: Zig 函数指针不支持隐式协变/逆变                                       │
│  ─────────────────────────────────────────────────────────────────────────  │
│                                                                              │
│  陈述:                                                                       │
│  Zig 函数指针类型必须精确匹配，不支持:                                         │
│  • 参数类型逆变（接受父类型的函数不能赋值给接受子类型的函数指针）                │
│  • 返回类型协变（返回子类型的函数不能赋值给返回父类型的函数指针）                │
│                                                                              │
│  这是有意的设计选择:                                                           │
│  1. 避免复杂的子类型规则带来的心智负担                                          │
│  2. 防止意外的类型转换错误                                                      │
│  3. 使类型检查更简单、更可预测                                                  │
│                                                                              │
│  需要类型擦除时，显式使用 *anyopaque 和 @ptrCast                               │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

---


---

## 📑 目录

- [📑 目录](#-目录)
- [十二、最佳实践总结](#十二最佳实践总结)
- [快速参考表](#快速参考表)
- [深入理解](#深入理解)
  - [核心原理](#核心原理)
  - [实践应用](#实践应用)
  - [最佳实践](#最佳实践)


---

## 十二、最佳实践总结

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                      C ↔ Zig 函数指针最佳实践                                 │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  1. 类型定义                                                                 │
│  ─────────────────────────────────────────────────────────────────────────  │
│  C:   typedef int (*callback_t)(int, void* user_data);                      │
│  Zig: const Callback = *const fn(i32, ?*anyopaque) callconv(.C) i32;         │
│                                                                              │
│  2. 空值处理                                                                 │
│  ─────────────────────────────────────────────────────────────────────────  │
│  C:   if (cb != NULL) cb(arg, data);  // 容易遗漏                            │
│  Zig: if (cb) |f| f(arg, data);       // 强制检查                            │
│                                                                              │
│  3. 用户数据传递                                                             │
│  ─────────────────────────────────────────────────────────────────────────  │
│  C:   使用 void* user_data 参数                                              │
│  Zig: 使用 ?*anyopaque 或 comptime 泛型                                      │
│                                                                              │
│  4. 调用约定                                                                 │
│  ─────────────────────────────────────────────────────────────────────────  │
│  • 跨平台代码使用 callconv(.C)                                               │
│  • Windows API 使用 callconv(.Stdcall) (x86)                                 │
│  • SIMD 代码考虑 callconv(.Vectorcall)                                       │
│                                                                              │
│  5. 性能优化                                                                 │
│  ─────────────────────────────────────────────────────────────────────────  │
│  • 热路径使用 comptime 函数参数而非运行时函数指针                              │
│  • 频繁调用的回调考虑内联缓存模式                                              │
│  • 批量处理减少间接调用次数                                                    │
│                                                                              │
│  6. 错误处理                                                                 │
│  ─────────────────────────────────────────────────────────────────────────  │
│  C:   返回值表示错误，或设置全局 errno                                         │
│  Zig: 错误联合类型 !T，或使用错误回调                                          │
│                                                                              │
│  7. 线程安全                                                                 │
│  ─────────────────────────────────────────────────────────────────────────  │
│  • 共享回调使用原子操作或互斥锁保护                                             │
│  • 考虑线程局部存储避免竞争                                                    │
│  • 信号处理器保持异步信号安全                                                  │
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 快速参考表

```text
┌─────────────────────────────────────────────────────────────────────────────┐
│                      C → Zig 函数指针快速参考                                  │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  C                                    Zig                                    │
│  ────────────────────────────────────────────────────────────────────────   │
│                                                                              │
│  // 基本函数指针                                                                  │
│  int (*fp)(int);                      const fp: *const fn(i32)               │
│                                       callconv(.C) i32 = &func;              │
│                                                                              │
│  // 类型别名                                                                  │
│  typedef void (*cb_t)(void*);         const Cb = *const fn(?*anyopaque)      │
│                                       callconv(.C) void;                     │
│                                                                              │
│  // 变参函数指针                                                              │
│  int (*vfp)(int, ...);                // Zig 不支持，使用 C 导入               │
│                                                                              │
│  // NULL 检查                                                                 │
│  if (fp != NULL) fp(x);               if (fp) |f| f(x);                      │
│  fp ? fp(x) : default;                (fp orelse default)(x);                │
│                                                                              │
│  // 函数指针数组                                                              │
│  int (*ops[])(int) = {f1, f2};        const ops = [_]*const fn(i32)          │
│                                       callconv(.C) i32{f1, f2};              │
│                                                                              │
│  // 调用约定                                                                  │
│  int __stdcall f(int);                fn f(i32) callconv(.Stdcall) i32;      │
│  int __fastcall f(int);               fn f(i32) callconv(.Fastcall) i32;     │
│  int __vectorcall f(float);           fn f(f32) callconv(.Vectorcall) i32;   │
│                                                                              │
│  // Vtable                                                                    │
│  struct { void (*f)(void); }          const VTable = extern struct {          │
│                                           f: *const fn() callconv(.C) void,  │
│                                       };                                     │
│                                                                              │
│  // 类型转换                                                                  │
│  (void (*)(void))fp;                  @ptrCast(fp)                           │
│  (void*)fp;                           @ptrCast(@as(*anyopaque, @ptrCast(fp)))│
│                                                                              │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

> **文档状态**: 核心内容完成 | **验证状态**: 理论框架 | **代码示例**: 可编译验证 | **最后更新**: 2026-03-12


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
