# C2y (C27) 提案预览

> **标准**: C2y (预计2027-2028年发布)
> **来源**: WG14文档日志
> **状态**: 审议中/讨论中

---

## 提案文档

| 提案 | 特性 | 状态 | 文档 |
|:-----|:-----|:-----|:-----|
| N3734 | `defer` 关键字 | 审议中 | [详解](01_Defer_Keyword.md) |
| N3753 | Elvis操作符 `?:` | 审议中 | [详解](02_Elvis_Operator.md) |
| N3658 | Named Loops | 讨论中 | [详解](03_Named_Loops.md) |
| N3636 | `__COUNTER__` 宏 | 讨论中 | [详解](04_Counter_Macro.md) |
| N3454 | `typeof(return)` | 审议中 | [详解](05_Typeof_Return.md) |

---

## C2y提案索引更新

完整提案文档详见各文件，所有提案均包含：

- 提案背景与动机
- 语法规范与语义
- 使用场景与示例
- 编译器支持状态
- 与现有方案对比

---

## 快速参考

### defer - 自动资源管理

```c
FILE* f = fopen("data.txt", "r");
if (!f) return ERROR;
defer fclose(f);
// 使用f...
// 函数退出时自动关闭
```

### Elvis操作符 - 默认值

```c
int port = config.port ?: 8080;
// 等价于: config.port ? config.port : 8080
// 但只评估一次
```

### Named Loops - 循环控制

```c
outer: for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 10; j++) {
        if (found) break outer;  // 跳出外层
    }
}
```

### **COUNTER** - 编译时计数器

```c
#define UNIQUE_NAME(prefix) prefix##__COUNTER__
int UNIQUE_NAME(var_);  // var_0
int UNIQUE_NAME(var_);  // var_1
```

### typeof(return) - 返回类型推导

```c
int func(void) {
    typeof(return) result;  // int类型
    // ...
}
```

---

## 编译器支持状态

| 提案 | GCC | Clang | 预计版本 |
|:-----|:----|:------|:---------|
| defer | 计划中 | 计划中 | 17+ / 25+ |
| Elvis | 扩展已支持 | 扩展已支持 | 标准支持17+ |
| Named Loops | - | 22+实验性 | 24+ |
| **COUNTER** | 扩展已支持 | 扩展已支持 | 标准支持17+ |
| typeof(return) | - | - | 17+ / 25+ |

---

## 参考

- **WG14文档日志**: <https://www.open-std.org/jtc1/sc22/wg14/www/wg14_document_log>
- **C23标准**: ISO/IEC 9899:2024
- **Modern C**: <https://gustedt.gitlabpages.inria.fr/modern-c/>

---

**最后更新**: 2026-03-24
