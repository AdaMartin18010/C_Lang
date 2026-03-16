# 安全关键C语言开发快速参考指南

> MISRA C:2023 + ISO 26262 + DO-178C + IEC 61508 速查表

---

## 编译器选项速查

### GCC安全编译选项

```bash
# 基础选项
gcc -std=c17 -pedantic -Wall -Wextra -Werror

# MISRA相关警告
-Wshadow              # 变量隐藏
-Wconversion          # 隐式转换
-Wsign-conversion     # 符号转换
-Wuninitialized       # 未初始化
-Wmaybe-uninitialized # 可能未初始化
-Wstrict-prototypes   # 原型声明
-Wmissing-prototypes  # 缺少原型
-Wundef               # 未定义宏

# 额外检查
-Wfloat-equal         # 浮点相等比较
-Waggregate-return    # 返回结构体
-Wbad-function-cast   # 错误函数转换
-Wcast-qual           # 移除const/volatile

# 调试和Sanitizer
-fsanitize=address    # 地址检查
-fsanitize=undefined  # UB检查
-g3                   # 调试信息
-O0                   # 禁用优化（调试）
```

### Clang选项

```bash
# 更严格的检查
clang -Weverything -Wno-padded -Wno-covered-switch-default

# 静态分析
scan-build clang -c file.c
```

---

## MISRA C:2023 核心规则速查

### 类型安全

| 规则 | 检查项 | 代码示例 |
|:-----|:-------|:---------|
| 5.1 | char只用于字符 | `char c = 'A';` ✅ `char c = 200;` ❌ |
| 5.2 | 不移除const | `(int *)&const_var` ❌ |
| 5.4 | 避免隐式窄化 | `short s = large_int;` ❌ |
| 5.7 | 不使用VLA | `int arr[n];` ❌ |
| 11.5 | 位运算用无符号 | `uint32_t flags` ✅ `int flags` ❌ |

### 指针安全

| 规则 | 检查项 | 代码示例 |
|:-----|:-------|:---------|
| 7.1 | 解引用前检查NULL | `if (p) *p = 0;` ✅ |
| 7.2 | 释放后置NULL | `free(p); p = NULL;` ✅ |
| 7.3 | 指针运算在数组内 | `p < arr + SIZE` |
| 15.4 | 对象指针不混用 | `(float*)int_arr` ❌ |

### 表达式安全

| 规则 | 检查项 | 代码示例 |
|:-----|:-------|:---------|
| 6.1 | 不依赖求值顺序 | `i++ + i++` ❌ |
| 6.2 | 单表达式单副作用 | `x = y++` ✅ `x = y++ + z++` ❌ |
| 10.1 | 使用前初始化 | `int x = 0;` ✅ `int x; use(x);` ❌ |
| 12.1 | if条件无赋值 | `if (x = 0)` ❌ |

### 控制流

| 规则 | 检查项 | 代码示例 |
|:-----|:-------|:---------|
| 6.6 | switch有default | `default: break;` ✅ |
| 6.8 | goto不向后跳 | `goto cleanup;` ✅ |
| 12.3 | 循环计数器不修改 | `for(i=0; i<n; i++) i++;` ❌ |
| 15.1 | 无隐式转换 | `(int)float_var` ✅ |

---

## ASIL/DAL/SIL对比表

| 标准 | 等级 | 典型应用 | 代码覆盖 | 工具要求 |
|:-----|:----:|:---------|:---------|:---------|
| **ISO 26262** | QM | 信息娱乐 | 建议MISRA | 基础 |
| | ASIL A | 后视镜 | MISRA Required | 静态分析 |
| | ASIL B | 大灯 | MISRA Required | 静态+单元测试 |
| | ASIL C | 巡航控制 | MISRA Req+Man | 全套工具 |
| | **ASIL D** | **转向/制动** | **MISRA全部** | **全套+形式化** |
| **DO-178C** | Level E | 乘客信息 | - | - |
| | Level D | 娱乐 | 语句覆盖 | 基础 |
| | Level C | 燃油管理 | 语句+分支 | 静态分析 |
| | Level B | 自动驾驶 | 语句+分支+MC/DC | 全套工具 |
| | **Level A** | **飞控/FADEC** | **MC/DC** | **全套+工具鉴定** |
| **IEC 61508** | SIL 1 | 监控 | 建议编码规范 | 基础 |
| | SIL 2 | PLC安全 | MISRA Required | 静态分析 |
| | **SIL 3** | **ESD系统** | **MISRA Req+Man** | **全套工具** |
| | **SIL 4** | **铁路信号** | **MISRA全部** | **全套+形式化** |

---

## 安全机制实现模板

### 看门狗实现

```c
#include <stdint.h>

static volatile uint32_t watchdog_counter = 0;
static const uint32_t WATCHDOG_THRESHOLD = 1000;

/* 主程序调用 */
void feed_watchdog(void)
{
    watchdog_counter++;
}

/* 独立任务/中断 */
void watchdog_monitor(void)
{
    static uint32_t last_count = 0;

    if (watchdog_counter == last_count) {
        /* 未喂狗，系统无响应 */
        system_reset();
    }

    if (watchdog_counter - last_count > WATCHDOG_THRESHOLD) {
        /* 喂狗过于频繁或异常 */
        log_warning("Watchdog irregularity");
    }

    last_count = watchdog_counter;
}
```

### 双通道比较 (1oo2)

```c
typedef struct {
    int32_t value_a;
    int32_t value_b;
    bool valid;
} DualChannelResult;

int32_t vote_dual_channel(const DualChannelResult *dc, int32_t *output)
{
    const int32_t TOLERANCE = 10;

    if (!dc->valid) {
        return ERROR_INVALID;
    }

    int32_t diff = (dc->value_a > dc->value_b) ?
                   (dc->value_a - dc->value_b) :
                   (dc->value_b - dc->value_a);

    if (diff > TOLERANCE) {
        /* 通道不一致 */
        return ERROR_MISMATCH;
    }

    /* 输出较保守值 */
    *output = (dc->value_a < dc->value_b) ? dc->value_a : dc->value_b;
    return SUCCESS;
}
```

### CRC校验

```c
#include <stdint.h>
#include <stddef.h>

uint16_t crc16_ccitt(const uint8_t *data, size_t length)
{
    uint16_t crc = 0xFFFF;

    for (size_t i = 0; i < length; i++) {
        crc ^= (uint16_t)data[i] << 8;
        for (int j = 0; j < 8; j++) {
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x1021;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}

typedef struct {
    uint8_t data[64];
    uint16_t crc;
} ProtectedPacket;

bool verify_packet(const ProtectedPacket *pkt)
{
    uint16_t calc_crc = crc16_ccitt(pkt->data, sizeof(pkt->data));
    return (calc_crc == pkt->crc);
}
```

### 范围检查模板

```c
#include <stdint.h>
#include <stdbool.h>

/* 安全类型转换 */
bool safe_int32_to_int16(int32_t input, int16_t *output)
{
    if (input > INT16_MAX || input < INT16_MAX) {
        return false;
    }
    *output = (int16_t)input;
    return true;
}

/* 加法溢出检查 */
bool safe_add_int32(int32_t a, int32_t b, int32_t *result)
{
    if (b > 0 && a > INT32_MAX - b) {
        return false;  /* 正溢出 */
    }
    if (b < 0 && a < INT32_MIN - b) {
        return false;  /* 负溢出 */
    }
    *result = a + b;
    return true;
}

/* 除零检查 */
int32_t safe_divide(int32_t a, int32_t b, int32_t *result)
{
    if (b == 0) {
        return ERROR_DIV_ZERO;
    }
    *result = a / b;
    return SUCCESS;
}
```

---

## 常见错误与修复

### 错误1: 未定义行为

```c
/* ❌ 错误 */
int x = i++ + i++;  /* 未定义行为 */

/* ✅ 修复 */
int temp1 = i++;
int temp2 = i++;
int x = temp1 + temp2;
```

### 错误2: 整数溢出

```c
/* ❌ 错误 */
int sum = a + b;  /* 可能溢出 */

/* ✅ 修复 */
int sum;
if (b > 0 && a > INT_MAX - b) {
    /* 溢出处理 */
} else if (b < 0 && a < INT_MIN - b) {
    /* 下溢处理 */
} else {
    sum = a + b;
}
```

### 错误3: 空指针解引用

```c
/* ❌ 错误 */
void process(Data *p)
{
    p->value = 0;  /* 可能空指针 */
}

/* ✅ 修复 */
void process(Data *p)
{
    if (p == NULL) {
        return;  /* 或错误处理 */
    }
    p->value = 0;
}
```

### 错误4: 缓冲区溢出

```c
/* ❌ 错误 */
char buf[10];
strcpy(buf, long_string);  /* 溢出 */

/* ✅ 修复 */
char buf[10];
strncpy(buf, long_string, sizeof(buf) - 1);
buf[sizeof(buf) - 1] = '\0';
```

### 错误5: 内存泄漏

```c
/* ❌ 错误 */
void leak(void)
{
    int *p = malloc(100);
    /* 使用p */
    /* 忘记free */
}

/* ✅ 修复 */
void no_leak(void)
{
    int *p = malloc(100);
    if (p == NULL) {
        return;
    }
    /* 使用p */
    free(p);
}
```

---

## 工具推荐

### 静态分析

| 工具 | 级别 | 价格 | MISRA支持 |
|:-----|:-----|:-----|:----------|
| PC-lint/Flexelint | 专业 | $$$ | 完整 |
| Coverity | 企业 | $$$$ | 完整 |
| Polyspace | 企业 | $$$$ | 完整+形式化 |
| Cppcheck | 开源 | 免费 | 基础 |
| Clang Static Analyzer | 开源 | 免费 | 部分 |

### 测试覆盖

| 工具 | 功能 | 价格 |
|:-----|:-----|:-----|
| BullseyeCoverage | 语句/分支/MC/DC | $$$ |
| gcov/lcov | 语句/分支 | 免费 |
| VectorCAST | 全套测试 | $$$$ |

### 调试工具

| 工具 | 功能 |
|:-----|:-----|
| GDB | 基础调试 |
| Valgrind | 内存检查 |
| AddressSanitizer | 内存错误 |
| UBSan | 未定义行为 |

---

## 参考资源

- [MISRA C:2023官方](https://misra.org.uk/)
- [ISO 26262-2018](https://www.iso.org/standard/68383.html)
- [DO-178C](https://my.rtca.org/)
- [IEC 61508](https://www.iec.ch/functionalsafety)

---

**最后更新**: 2026-03
