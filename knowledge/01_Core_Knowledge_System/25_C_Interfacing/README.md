# C 语言互操作 (C Interfacing)

> **层级**: L4 (高级)
> **目标**: 掌握 C 与其他语言、系统的互操作技术

---

## 内容导航

| 主题 | 说明 | 文档 |
|:-----|:-----|:-----|
| **调用约定与 ABI** | cdecl、stdcall、fastcall、System V AMD64 ABI、Windows x64 | 调用约定 |
| **FFI 概览** | Python/ctypes、Java/JNI、Go/CGO、Rust FFI 简介 | FFI 概览 |
| **C ↔ Rust FFI** | 完整指南：所有权、生命周期、panic安全、bindgen/cbindgen | [C↔Rust FFI 完全指南](C_Rust_FFI_Guide.md) ⭐ |

---

## 学习路径

```
1. 调用约定基础
   └── 理解 ABI、寄存器分配、栈帧布局

2. 多语言 FFI 概览
   └── Python ctypes、Java JNI、Go CGO 基础

3. C ↔ Rust 深度实践
   └── 所有权转移、内存安全、自动化绑定
```

---

> **最后更新**: 2026-05-11
