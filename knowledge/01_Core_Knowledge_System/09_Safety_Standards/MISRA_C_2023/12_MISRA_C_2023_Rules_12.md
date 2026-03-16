# MISRA C:2023 Rules 12.1-12.6 - 控制流语句

> if/switch/for/while/do控制流的安全使用

---

## 规则分类概述

| 规则编号 | 规则名称 | 强制性 | 核心要求 |
|:--------:|:---------|:------:|:---------|
| Rule 12.1 | if语句 | Required | 条件明确，避免赋值 |
| Rule 12.2 | switch语句 | Required | 完整性和default |
| Rule 12.3 | for循环 | Required | 循环控制安全 |
| Rule 12.4 | while循环 | Required | 终止保证 |
| Rule 12.5 | do-while | Advisory | 避免使用 |
| Rule 12.6 | 跳转语句 | Required | goto/break/continue限制 |

---

## Rule 12.1 - if语句安全

### 规则原文

> **Rule 12.1 (Required)**: The condition in an if or switch statement shall not be an assignment expression.
>
> **中文**: if或switch语句中的条件不得是赋值表达式。

### 违反示例

```c
/* ❌ 严重违反 - 赋值误用为相等 */
if (x = 0) {  /* 赋值，不是比较！ */
    /* 永远不会执行（x=0为假） */
}

/* ❌ 违反 - 复杂赋值 */
if (x = get_value() + offset) {
    /* 意图不明确 */
}

/* ❌ 违反 - 函数内赋值 */
if (result = process_data()) {
    /* 副作用隐藏在条件中 */
}

/* ❌ 违反 - switch中的赋值 */
switch (status = get_status()) {  /* 不清晰 */
    /* ... */
}
```

### 合规示例

```c
/* ✅ 合规 - 明确比较 */
if (x == 0) {
    /* ... */
}

/* ✅ 合规 - 分离赋值和比较 */
x = get_value() + offset;
if (x != 0) {
    /* ... */
}

/* ✅ 合规 - 明确处理结果 */
result = process_data();
if (result != 0) {
    /* ... */
}

/* ✅ 合规 - 先赋值再switch */
status = get_status();
switch (status) {
    /* ... */
}

/* ✅ 合规 - 如果必须检查非零 */
if ((x = get_value()) != 0) {  /* 允许，但需显式比较 */
    use(x);
}
```

---

## Rule 12.2 - switch语句完整性

### 规则原文

> **Rule 12.2 (Required)**: The final clause of a switch statement shall be the default clause.
>
> **中文**: switch语句的最后一个子句应是default子句。

### switch语句最佳实践

```c
/* ✅ 合规 - 完整的switch结构 */
switch (command) {
    case CMD_START:
        handle_start();
        break;

    case CMD_STOP:
        handle_stop();
        break;

    case CMD_PAUSE:
        handle_pause();
        break;

    default:  /* 总是最后 */
        handle_unknown_command(command);
        break;
}

/* ✅ 合规 - 枚举类型的穷尽检查 */
enum State { STATE_IDLE, STATE_RUNNING, STATE_ERROR };

void handle_state(enum State s)
{
    switch (s) {
        case STATE_IDLE:
            /* 处理 */
            break;
        case STATE_RUNNING:
            /* 处理 */
            break;
        case STATE_ERROR:
            /* 处理 */
            break;

        default:
            /* 防御性：不应到达 */
            log_error("Invalid state: %d", s);
            break;
    }
}

/* ✅ 合规 - 故意fall-through */
switch (packet.type) {
    case TYPE_CONTROL:
        process_control_header();
        /* fall-through - 控制包也需要处理数据 */

    case TYPE_DATA:
        process_data_payload();
        break;

    default:
        drop_packet();
        break;
}
```

### 违反示例

```c
/* ❌ 违反 - 缺少default */
switch (value) {
    case 1: do_something(); break;
    case 2: do_other(); break;
    /* 缺少default */
}

/* ❌ 违反 - default不在最后 */
switch (value) {
    case 1: break;
    default: break;
    case 2: break;  /* default后还有case */
}

/* ❌ 违反 - 隐式fall-through */
switch (cmd) {
    case CMD_A:
        init();  /* 缺少break！ */
    case CMD_B:
        process();
        break;
}
```

---

## Rule 12.3 - for循环安全

### 规则原文

> **Rule 12.3 (Required)**: Loop counters shall not be modified in the loop body.
>
> **中文**: 循环计数器不得在循环体中修改。

### 违反示例

```c
/* ❌ 违反 - 修改循环变量 */
for (int i = 0; i < 10; i++) {
    if (skip_condition) {
        i++;  /* 修改循环计数器！ */
    }
}

/* ❌ 违反 - 条件中修改 */
for (int i = 0; i < n; i++) {
    if (remove_item(i)) {
        n--;  /* 修改边界 */
        i--;  /* 回退计数器 */
    }
}

/* ❌ 违反 - 复杂控制 */
for (int i = 0; i < 100; i += step) {
    if (condition) {
        step = 2;  /* 修改步长 */
    }
}
```

### 合规示例

```c
/* ✅ 合规 - 简单for循环 */
for (int i = 0; i < count; i++) {
    process(i);  /* 不修改i */
}

/* ✅ 合规 - 使用while处理复杂逻辑 */
int i = 0;
while (i < count) {
    if (should_skip(i)) {
        i += 2;  /* while允许复杂控制 */
    } else {
        process(i);
        i++;
    }
}

/* ✅ 合规 - 使用临时变量 */
for (int i = 0; i < n; i++) {
    int idx = i;
    if (condition) {
        idx = alternative_index(i);  /* 使用临时变量 */
    }
    use(idx);
}

/* ✅ 合规 - 逆向遍历 */
for (int i = count - 1; i >= 0; i--) {
    /* 安全删除元素 */
    remove(i);
}
```

---

## Rule 12.4 - while循环终止

### 规则原文

> **Rule 12.4 (Required)**: A loop shall have a single exit point.
>
> **中文**: 循环应有单一出口点。

### 违反示例

```c
/* ❌ 违反 - 多个出口 */
while (condition1) {
    if (condition2) {
        break;  /* 出口1 */
    }
    if (condition3) {
        return;  /* 出口2 */
    }
    /* ... */
    if (done) {
        break;  /* 出口3 */
    }
}

/* ❌ 违反 - 复杂退出条件 */
while (1) {
    /* ... */
    if (a && b || c && d) {
        break;
    }
}
```

### 合规示例

```c
/* ✅ 合规 - 单一出口 */
bool done = false;
while (!done) {
    if (error) {
        done = true;  /* 设置标志而非break */
    } else {
        process();
        done = is_complete();
    }
}

/* ✅ 合规 - 清晰的退出条件 */
while (has_more_data() && !timeout_occurred()) {
    process_next();
}

/* ✅ 合规 - do-while替代（如果适用）*/
do {
    process();
} while (should_continue());

/* ✅ 合规 - 无限循环（明确意图）*/
for (;;) {  /* 或 while (1) */
    process_messages();
}
```

---

## Rule 12.5 - do-while限制

### 规则原文

> **Rule 12.5 (Advisory)**: The do...while loop should be avoided.
>
> **中文**: 应避免使用do...while循环。

### 原因

do-while循环的终止条件在循环体之后检查，容易忽略边界情况。

### 替代方案

```c
/* ❌ 建议避免 */
do {
    process();
} while (condition);

/* ✅ 替代 - for循环（已知次数）*/
for (int i = 0; i < n; i++) {
    process();
}

/* ✅ 替代 - while循环（先检查）*/
while (condition) {
    process();
}

/* ✅ 如果必须使用do-while */
/* 使用场景：至少执行一次，且条件依赖执行结果 */
do {
    status = try_operation();
    if (status != OK) {
        retry_delay();
    }
} while (status != OK && retries++ < MAX_RETRIES);
```

---

## Rule 12.6 - 跳转语句

### 规则原文

> **Rule 12.6 (Required)**: goto shall not be used to create backward jumps.
>
> **中文**: goto不得用于创建向后跳转。

### goto使用规范

```c
/* ✅ 合规 - 向前跳转（资源清理）*/
int process_file(const char *path)
{
    FILE *fp = NULL;
    char *buffer = NULL;
    int result = -1;

    fp = fopen(path, "r");
    if (fp == NULL) {
        goto cleanup;  /* 向前跳转 */
    }

    buffer = malloc(BUF_SIZE);
    if (buffer == NULL) {
        goto cleanup;
    }

    /* 处理文件 */

    result = 0;

cleanup:
    free(buffer);
    if (fp != NULL) {
        fclose(fp);
    }
    return result;
}

/* ❌ 严重违反 - 向后跳转 */
retry:
    if (try_operation() != OK) {
        goto retry;  /* 向后跳转 - 像goto一样！ */
    }
```

### break和continue使用

```c
/* ✅ 合规 - 正常循环控制 */
for (int i = 0; i < n; i++) {
    if (skip_condition(i)) {
        continue;  /* 跳到下一次迭代 */
    }

    if (done_condition(i)) {
        break;  /* 退出循环 */
    }

    process(i);
}

/* ✅ 合规 - 标签化break（MISRA允许）*/
search:
for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
        if (found(i, j)) {
            break search;  /* 跳出外层循环 */
        }
    }
}
```

---

## 控制流安全检查清单

```markdown
□ if条件中无赋值
□ switch有default且在最后
□ 枚举类型穷尽处理
□ fall-through有注释
□ for循环不修改计数器
□ while循环有明确终止条件
□ 循环单一出口（或明确标记）
□ goto只向前跳转
□ 无 spaghetti code
```

---

## 检测工具配置

### GCC

```bash
gcc \
  -Wparentheses \
  -Wswitch-default \      /* switch缺少default */
  -Wswitch-enum \         /* switch未覆盖所有枚举值 */
  -Wjump-misses-init \    /* 跳过初始化 */
  -c file.c
```

### PC-lint

```bash
pc-lint \
    -e616 \   /* switch无default */
    -e646 \   /* case未break */
    -e616 \   /* switch无default */
    -e717 \   /* do-while使用 */
    file.c
```

---

## 总结

| 规则 | 强制 | 核心要求 |
|:----:|:----:|:---------|
| 12.1 | Required | if条件无赋值 |
| 12.2 | Required | switch完整，default最后 |
| 12.3 | Required | 循环计数器不修改 |
| 12.4 | Required | 单一出口 |
| 12.5 | Advisory | 避免do-while |
| 12.6 | Required | goto不向后跳 |

---

**参考**: MISRA C:2023 Guidelines for the use of the C language in critical systems, Chapter 12

---

> **下一章节**: [Rules 13.1-13.x - 函数安全](./13_MISRA_C_2023_Rules_13.md)
