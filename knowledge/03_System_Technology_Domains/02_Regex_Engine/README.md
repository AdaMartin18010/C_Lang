# 正则表达式引擎

> **层级定位**: 03_System_Technology_Domains > 02_Regex_Engine

---

## 内容

- [Thompson NFA](./01_Thompson_NFA.md)
- [Pike VM](./02_Pike_VM.md)

---

## 实现方法

### NFA方法

将正则表达式转换为NFA，进行模拟。

```c
// 状态结构
typedef struct State {
    int c;
    struct State *out;
    struct State *out1;
    int lastlist;
} State;

// Thompson构造
State* re_to_nfa(const char *pattern);
int match(State *start, const char *text);
```

---

> **最后更新**: 2026-03-21


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
