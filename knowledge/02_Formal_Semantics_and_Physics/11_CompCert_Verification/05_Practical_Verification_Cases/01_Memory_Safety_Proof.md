# 内存安全形式化验证案例

> **定位**: CompCert Verification / 实践案例 | **难度**: ⭐⭐⭐⭐⭐ | **目标**: 使用 VST/Frama-C 验证内存安全属性

---

## 目录

- [内存安全形式化验证案例](#内存安全形式化验证案例)
  - [目录](#目录)
  - [一、案例概述](#一案例概述)
  - [二、Frama-C/WP 内存安全验证](#二frama-cwp-内存安全验证)
    - [2.1 数组边界检查](#21-数组边界检查)
    - [2.2 空指针检查](#22-空指针检查)
    - [2.3 内存泄漏检测](#23-内存泄漏检测)
  - [三、VST 内存安全验证](#三vst-内存安全验证)
    - [3.1 分离逻辑基础](#31-分离逻辑基础)
    - [3.2 链表操作验证](#32-链表操作验证)
  - [四、Zig 对比实现](#四zig-对比实现)
  - [五、验证方法论总结](#五验证方法论总结)

---

## 一、案例概述

内存安全验证目标是确保程序不包含：缓冲区溢出、使用已释放内存、空指针解引用、内存泄漏、重复释放。

验证工具:

- **Frama-C/WP**: 自动演绎验证，ACSL 规格
- **VST**: Coq 交互式证明，适用于复杂数据结构
- **Zig**: 编译时检查 + 运行时安全模式

---

## 二、Frama-C/WP 内存安全验证

### 2.1 数组边界检查

```c
/*@
  requires \valid(arr + (0..n-1));
  requires n > 0;
  assigns \nothing;
*/
int array_max(const int* arr, size_t n) {
    int max = arr[0];
    /*@
      loop invariant 0 <= i <= n;
      loop invariant \forall integer j; 0 <= j < i ==> max >= arr[j];
      loop assigns i, max;
      loop variant n - i;
    */
    for (size_t i = 1; i < n; i++) {
        if (arr[i] > max) max = arr[i];
    }
    return max;
}
```

### 2.2 空指针检查

```c
/*@
  requires p != \null ==> \valid(p);
  behavior null_case:
    assumes p == \null;
    ensures \result == 0;
  behavior valid_case:
    assumes p != \null;
    ensures \result == *p + 1;
*/
int safe_increment(const int* p) {
    return (p == NULL) ? 0 : (*p + 1);
}
```

### 2.3 内存泄漏检测

```c
/*@
  allocates \result;
  ensures \result != \null ==> \valid(\result);
  ensures \result != \null ==> \fresh(\result, sizeof(int) * n);
*/
int* create_array(size_t n);

/*@
  requires \valid(ptr);
  frees ptr;
*/
void destroy_array(int* ptr);
```

---

## 三、VST 内存安全验证

### 3.1 分离逻辑基础

```coq
(* 分离逻辑断言 *)
(* data_at: 指向特定类型数据的内存块 *)
(* malloc_token: malloc 分配的标记 *)
(* emp: 空堆 *)
(* star (*): 分离合取 *)

Fixpoint listrep (sigma: list val) (p: val) : mpred :=
  match sigma with
  | nil => !!(p = nullval) && emp
  | x::xs => EX q:val, data_at Tsh t_list (x, q) p * listrep xs q
  end.
```

### 3.2 链表操作验证

```c
typedef struct Node {
    int data;
    struct Node* next;
} Node;

Node* list_prepend(Node* head, int value);
void list_free(Node* head);
```

```coq
Lemma body_list_prepend:
  semax_body Vprog Gprog f_list_prepend list_prepend_spec.
Proof.
  start_function.
  forward_call (sizeof t_node).
  Intros p.
  forward_if.
  - (* p = null *) forward.
  - (* p <> null *) forward; forward; forward.
    Exists p. entailer!.
Qed.
```

---

## 四、Zig 对比实现

```zig
// 自动边界检查
fn arrayMax(arr: []const i32) ?i32 {
    if (arr.len == 0) return null;
    var max = arr[0];
    for (arr[1..]) |val| {
        if (val > max) max = val;
    }
    return max;
}

// 强制空检查
fn safeIncrement(p: ?*const i32) i32 {
    const ptr = p orelse return 0;
    return ptr.* + 1;
}

// 通用分配器模式
fn managedLifecycle(allocator: std.mem.Allocator) !void {
    const arr = try allocator.alloc(i32, 10);
    defer allocator.free(arr);  // 确保释放
    arr[0] = 42;
}
```

---

## 五、验证方法论总结

| 方法 | 优势 | 局限 |
|:-----|:-----|:-----|
| Frama-C/WP | 自动化程度高 | 复杂数据结构需辅助 |
| VST | 表达能力最强 | 需 Coq 专业知识 |
| Zig | 零成本抽象 | 不保证完全正确性 |

---

> **文档状态**: 核心案例完成 | **最后更新**: 2026-03-13
