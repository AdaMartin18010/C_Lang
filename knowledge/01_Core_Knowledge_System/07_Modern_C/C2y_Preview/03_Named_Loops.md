# C2y Named Loops (命名循环)

> **提案**: N3658 - Named Loops
> **状态**: 讨论中
> **作者**: Celeste
> **预计支持**: Clang 22+ (实验性) / GCC (计划中)

---

## 1. 概述

Named Loops允许为循环指定标识符，从而可以从嵌套循环内部直接`break`或`continue`到指定的外层循环，消除对`goto`的需求。

```c
// C2y: 命名循环简化嵌套循环控制
outer: for (int i = 0; i < 10; i++) {
    inner: for (int j = 0; j < 10; j++) {
        if (found(i, j)) {
            break outer;  // 直接跳出外层循环
        }
    }
}
// 执行流直接到达这里

// C23及之前: 需要goto或标志变量
int found = 0;
for (int i = 0; i < 10 && !found; i++) {
    for (int j = 0; j < 10; j++) {
        if (found_target(i, j)) {
            found = 1;
            break;  // 只跳出内层
        }
    }
}
// 或
for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 10; j++) {
        if (found_target(i, j)) {
            goto outer_done;
        }
    }
}
outer_done:
```

---

## 2. 语法规范

### 2.1 基本语法

```c
identifier: for (init; cond; incr) statement
identifier: while (cond) statement
identifier: do statement while (cond);

break identifier;      // 跳出指定循环
continue identifier;   // 继续指定循环
```

### 2.2 命名规则

- 遵循C标识符命名规则
- 在作用域内唯一
- 不与其他变量名冲突 (独立的命名空间)

```c
loop1: for (int i = 0; i < 10; i++) {      // OK
loop2: while (condition) {                  // OK
loop1: for (int j = 0; j < 10; j++) {       // 错误: loop1重复
```

---

## 3. 使用场景

### 3.1 矩阵搜索

```c
// C2y: 矩阵搜索找到目标后立即退出所有循环
int search_matrix(int rows, int cols, int target, int matrix[rows][cols]) {
    int found_row = -1, found_col = -1;

    search: for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (matrix[i][j] == target) {
                found_row = i;
                found_col = j;
                break search;  // 跳出search循环
            }
        }
    }

    if (found_row >= 0) {
        printf("Found at (%d, %d)\n", found_row, found_col);
        return 1;
    }
    return 0;
}
```

### 3.2 多层嵌套continue

```c
// C2y: 跳到指定外层循环的下一次迭代
outer: for (int i = 0; i < 10; i++) {
    middle: for (int j = 0; j < 10; j++) {
        inner: for (int k = 0; k < 10; k++) {
            if (skip_condition(i, j, k)) {
                continue middle;  // 跳到middle的下一次迭代
            }
            // 处理...
        }
    }
}
```

### 3.3 解析器实现

```c
// C2y: 解析嵌套结构
typedef enum { TOKEN_EOF, TOKEN_BEGIN, TOKEN_END, TOKEN_OTHER } Token;

int parse_nested_blocks(Token* tokens) {
    int depth = 0;
    Token* p = tokens;

    parse: while (*p != TOKEN_EOF) {
        switch (*p) {
            case TOKEN_BEGIN:
                depth++;
                // 解析嵌套内容
                nested: while (*p != TOKEN_EOF) {
                    if (*p == TOKEN_BEGIN) {
                        depth++;
                    } else if (*p == TOKEN_END) {
                        depth--;
                        if (depth == 0) {
                            break nested;  // 完成当前块
                        }
                    }
                    p++;
                }
                break;
            case TOKEN_END:
                return -1;  // 语法错误: 意外的END
            default:
                // 处理其他token
                break;
        }
        p++;
    }

    return depth == 0 ? 0 : -1;
}
```

---

## 4. 与现有方案对比

### 4.1 与 `goto` 对比

```c
// C23: 使用goto
void search_with_goto(int n, int m, int target, int arr[n][m]) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (arr[i][j] == target) {
                printf("Found!\n");
                goto found;
            }
        }
    }
    printf("Not found\n");
    return;

found:
    // 处理找到的情况
}

// C2y: 使用Named Loops
void search_with_named_loops(int n, int m, int target, int arr[n][m]) {
    search: for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            if (arr[i][j] == target) {
                printf("Found!\n");
                break search;  // 更清晰，无需标签
            }
        }
    }
    // 统一退出点
}
```

### 4.2 与标志变量对比

```c
// C23: 使用标志变量 (容易出错)
int found = 0;
for (int i = 0; i < n && !found; i++) {
    for (int j = 0; j < m; j++) {
        if (arr[i][j] == target) {
            found = 1;
            break;  // 只跳出内层!
        }
    }
}

// C2y: 直接break外层
search: for (int i = 0; i < n; i++) {
    for (int j = 0; j < m; j++) {
        if (arr[i][j] == target) {
            break search;  // 清晰跳出外层
        }
    }
}
```

---

## 5. 限制与规则

### 5.1 作用域规则

```c
void scope_rules(void) {
    outer: for (int i = 0; i < 10; i++) {
        if (condition) {
            break outer;  // OK: 在outer作用域内
        }
    }

    // break outer;  // 错误: outer已不在作用域
}
```

### 5.2 命名冲突

```c
void naming(void) {
    int loop = 0;  // 变量

    loop: for (int i = 0; i < 10; i++) {  // OK: 不同命名空间
        break loop;  // 指循环名，不是变量
    }
}
```

### 5.3 必须与循环结合

```c
void invalid(void) {
    label:  // 普通标签
        printf("test");

    break label;  // 错误: label不是循环
}
```

---

## 6. 编译器支持

| 编译器 | 版本 | 支持状态 |
|:-------|:-----|:---------|
| Clang | 22+ | 实验性支持 (-fexperimental-named-loops) |
| GCC | - | 计划中 |
| MSVC | - | 未公布 |

### 6.1 Clang实验性使用

```bash
clang -std=c2y -fexperimental-named-loops test.c
```

### 6.2 兼容性代码

```c
#if __STDC_VERSION__ >= 202700L && __has_feature(named_loops)
    #define NAMED_LOOP(name) name:
    #define BREAK_LOOP(name) break name
    #define CONTINUE_LOOP(name) continue name
#else
    // Fallback: 使用goto模拟
    #define NAMED_LOOP(name) name##_loop_start:
    #define BREAK_LOOP(name) goto name##_loop_end
    #define CONTINUE_LOOP(name) goto name##_loop_continue

    // 需要在循环后添加标签
    // 注意: 这个宏无法完美模拟，建议手动处理
#endif
```

---

## 7. 最佳实践

### 7.1 有意义的命名

```c
// 推荐: 描述性命名
matrix_search: for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
        if (found) break matrix_search;
    }
}

// 不推荐: 无意义命名
a: for (int i = 0; i < 10; i++) {  // 'a' 没有描述性
```

### 7.2 适度嵌套

```c
// 警告: 过度嵌套难以理解，即使使用Named Loops
outer: for (...) {
    middle: for (...) {
        inner: for (...) {
            if (...) break outer;
            if (...) continue middle;
            if (...) break inner;
        }
    }
}

// 建议: 超过3层嵌套考虑重构
```

### 7.3 文档说明

```c
// 推荐: 复杂跳出逻辑添加注释
file_processing: for (int i = 0; i < file_count; i++) {
    for (int j = 0; j < record_count; j++) {
        if (fatal_error) {
            // 致命错误: 停止处理所有文件
            break file_processing;
        }
        // ...
    }
}
```

---

## 8. 参考

- **提案文档**: N3658 - Named Loops (2025-07-24)
- **作者**: Celeste
- **WG14日志**: <https://www.open-std.org/jtc1/sc22/wg14/www/wg14_document_log>
- **Clang跟踪**: <https://clang.llvm.org/docs/AttributeReference.html>

---

**状态**: 讨论中
**最后更新**: 2026-03-24
**预计标准化**: C2y (2027-2028)
