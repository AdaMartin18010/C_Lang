# 错误处理 (Error Handling)

> **层级**: L2-L4
> **对应标准**: ISO C §7.5 (errno), POSIX

---

## 内容导航

| 主题 | 说明 | 文档 |
|:-----|:-----|:-----|
| **错误处理完全指南** | 返回值模式、errno、goto清理、RAII模拟、setjmp/longjmp | [错误处理指南](00_Error_Handling_Complete_Guide.md) ⭐ |

---

## 核心技能

- 函数返回值检查与错误传播
- 自定义错误码设计
- 资源清理（goto cleanup、__attribute__((cleanup))）
- errno 线程安全使用
- C23 `ckd_add` 溢出检测

---

> **最后更新**: 2026-05-11
