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
