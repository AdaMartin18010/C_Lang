# ISO/IEC TS 17961: C Secure Coding Rules

## 标准概述

ISO/IEC TS 17961 是由国际标准化组织 (ISO) 和国际电工委员会 (IEC) 联合制定的C语言安全编码规则国际标准。
该标准专门设计为可由静态代码分析器强制执行，确保安全漏洞检测的一致性。

**标准全名**: Information technology — Programming languages, their environments and system software interfaces — C secure coding rules

**版本**: 2013 版 + 2016 技术勘误表 (COR 1)
**状态**: 现行有效标准 (2016年更新)

---

## 标准目的

### 核心目标

1. **建立统一的静态分析基线**: 在 ISO/IEC TS 17961 之前，静态分析的使用是即兴的、非系统性的，导致安全问题的覆盖不均匀。

2. **减少误报**: 标准要求静态代码分析器以一致的方式识别安全编码规则违规，减少误报数量。

3. **促进工具互操作性**: 确保不同厂商的静态分析工具产生一致的检测结果。

### 与相关标准的关系

```
标准层级关系:
┌─────────────────────────────────────────────────────────────┐
│  ISO/IEC 9899:2024 (C23) - C语言基础标准                     │
└──────────────────────────┬──────────────────────────────────┘
                           │
┌──────────────────────────▼──────────────────────────────────┐
│  ISO/IEC TS 17961 - C Secure Coding Rules                   │
│  (安全编码规则 - 可静态分析强制执行)                           │
└──────────────────────────┬──────────────────────────────────┘
                           │
        ┌──────────────────┼──────────────────┐
        ▼                  ▼                  ▼
┌───────────────┐  ┌───────────────┐  ┌───────────────┐
│ MISRA C:2025  │  │  CERT C 2024  │  │  CWE Top 25   │
│ (汽车安全)     │  │  (通用安全)    │  │  (漏洞分类)    │
└───────────────┘  └───────────────┘  └───────────────┘
```

---

## 技术规范详解

### 规则分类结构

ISO/IEC TS 17961 包含 **46 条核心安全规则**，按安全类别组织：

| 类别ID | 类别名称 | 规则数量 | 说明 |
|--------|----------|----------|------|
| 5.1-5.3 | 环境安全 | 3 | system() 调用、环境变量处理 |
| 5.4-5.6 | 整数安全 | 3 | 整数溢出、除零错误 |
| 5.7-5.19 | 内存安全 | 13 | 内存分配、释放、越界访问 |
| 5.20-5.23 | 指针安全 | 4 | 无效指针、越界指针 |
| 5.24-5.28 | 库函数安全 | 5 | 字符串处理、格式化 I/O |
| 5.29-5.37 | 字符串安全 | 9 | 字符串操作、字符处理 |
| 5.38-5.40 | 污染数据处理 | 3 | 外部输入验证 |

### 关键规则详解

#### 规则 5.1: 调用 `system()` 函数

**要求**: 不得使用 `system()` 函数执行外部命令。

```c
// 不合规代码
void process_file(const char* filename) {
    char command[256];
    sprintf(command, "cat %s", filename);  // 命令注入漏洞
    system(command);
}

// 合规代码
#include <fcntl.h>
#include <unistd.h>

bool display_file_safe(const char* filename) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) return false;

    char buffer[4096];
    ssize_t n;
    while ((n = read(fd, buffer, sizeof(buffer))) > 0) {
        write(STDOUT_FILENO, buffer, n);
    }

    close(fd);
    return n == 0;
}
```

#### 规则 5.4: 整数溢出 (有符号)

**要求**: 必须检测有符号整数运算溢出。

```c
#include <stdbool.h>
#include <limits.h>
#include <stdint.h>

// 合规实现 - 加法溢出检测
bool safe_add_int(int a, int b, int* result) {
    if (b > 0) {
        if (a > INT_MAX - b) return false;  // 正溢出
    } else if (b < 0) {
        if (a < INT_MIN - b) return false;  // 负溢出
    }
    *result = a + b;
    return true;
}

// C23 标准解决方案
#if __STDC_VERSION__ >= 202311L
#include <stdckdint.h>

bool safe_add_int_c23(int a, int b, int* result) {
    return ckd_add(result, a, b);  // C23 标准函数
}
#endif
```

#### 规则 5.22: 重复释放内存

**要求**: 不得重复释放同一块内存。

```c
// 不合规代码 - 双重释放
void process_buffer(char** buf1, char** buf2) {
    free(*buf1);
    free(*buf2);  // 如果 buf1 == buf2，双重释放!
}

// 合规代码
void process_buffer_safe(char** buf1, char** buf2) {
    if (*buf1 == *buf2) {
        // 同一缓冲区，只释放一次
        free(*buf1);
        *buf1 = NULL;
        *buf2 = NULL;
    } else {
        free(*buf1);
        *buf1 = NULL;
        free(*buf2);
        *buf2 = NULL;
    }
}
```

#### 规则 5.36: 污染字符串传递给字符串复制函数

**要求**: 外部输入字符串必须经过验证后才能传递给 `strcpy`、`strcat` 等函数。

```c
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

// 输入验证函数
bool is_valid_filename(const char* filename) {
    if (filename == NULL) return false;

    // 只允许字母数字、点和下划线
    for (const char* p = filename; *p; p++) {
        if (!isalnum((unsigned char)*p) && *p != '.' && *p != '_') {
            return false;
        }
    }
    return true;
}

// 合规代码
bool copy_filename_safe(const char* user_input, char* dest, size_t dest_size) {
    if (!is_valid_filename(user_input)) {
        return false;
    }

    // 现在可以安全复制
    if (strlen(user_input) >= dest_size) {
        return false;
    }
    strcpy(dest, user_input);
    return true;
}
```

---

## 静态分析器合规性要求

### 分析器分类

| 类型 | 描述 | 合规要求 |
|------|------|----------|
| **Type 1** | 检测到所有违规模式 | 必须 |
| **Type 2** | 需开发者审核的潜在违规 | 辅助 |
| **Type 3** | 需规范文档辅助分析 | 建议 |

### 可诊断性要求

标准要求静态分析器对以下模式必须产生诊断：

```c
// 示例: 规则 5.21 - 越界指针或数组下标
void array_access(void) {
    int arr[10];
    int* ptr = &arr[10];      // 分析器必须诊断: 越界指针
    int x = arr[10];          // 分析器必须诊断: 越界访问
}

// 示例: 规则 5.37 - 对指针使用 sizeof 而非所指类型
void buffer_alloc(void) {
    int* p = malloc(10 * sizeof(p));  // 错误: sizeof(int*) 而非 sizeof(int)
    // 分析器必须诊断: 可能分配不足
}
```

---

## 规则与 CWE 映射

ISO/IEC TS 17961 规则与常见弱点枚举 (CWE) 的映射关系：

| ISO 规则 | CWE ID | CWE 名称 |
|----------|--------|----------|
| 5.1 | CWE-78 | OS Command Injection |
| 5.4 | CWE-190 | Integer Overflow |
| 5.22 | CWE-415 | Double Free |
| 5.23 | CWE-134 | Use of Externally-Controlled Format String |
| 5.27 | CWE-758 | Reliance on Undefined Behavior |
| 5.36 | CWE-120 | Buffer Copy without Checking Size |
| 5.37 | CWE-467 | Use of sizeof() on Pointer Type |

---

## 合规验证方法

### 自动化验证工具

#### 1. GrammaTech CodeSonar

```bash
# ISO 17961 规则集分析
codesonar analyze -preset ISO17961 -project MyProject .
```

#### 2. Klocwork

```bash
# Klocwork ISO/IEC TS 17961 检查
kwcheck run -e "ISO.IEC.TS.17961" --project-dir .kwlp
```

#### 3. LDRA

```bash
# LDRA 规则集配置
tbrun -tbcfg=iso17961.tcf myproject
```

### 合规性报告模板

```markdown
## ISO/IEC TS 17961 合规性报告

项目: [项目名称]
分析日期: [日期]
工具: [工具名称及版本]

### 摘要
- 规则总数: 46
- 合规规则: [X] 条
- 不合规规则: [Y] 条
- 不适用规则: [Z] 条

### 不合规项详情
| 规则ID | 严重性 | 位置 | 描述 | 修复建议 |
|--------|--------|------|------|----------|
| 5.22 | 高 | file.c:45 | 双重释放 | 添加空指针检查 |

### 偏差批准
| 规则ID | 偏差原因 | 批准人 | 有效期 |
|--------|----------|--------|--------|
| 5.1 | 遗留代码，已隔离 | 安全团队 | 2026-12-31 |
```

---

## 与 MISRA C:2025 协同使用

### 联合合规策略

```c
/* 同时满足 MISRA C:2025 和 ISO/IEC TS 17961 的代码示例 */

/* MISRA Rule 21.3: 不得使用 malloc/free (若适用)
 * ISO 17961 5.22: 避免重复释放
 */

#include <stddef.h>
#include <stdint.h>

/* 合规的安全内存管理封装 */
typedef struct {
    void* data;
    size_t size;
    uint32_t magic;  /* 完整性校验 */
} safe_buffer_t;

#define BUFFER_MAGIC 0x42464642U

bool buffer_init(safe_buffer_t* buf, size_t size) {
    if (buf == NULL || size == 0U) {
        return false;
    }

    buf->data = NULL;
    buf->size = 0U;
    buf->magic = 0U;

    /* 使用静态分析器验证的分配 */
    void* ptr = /* 分配实现 */ NULL;
    if (ptr == NULL) {
        return false;
    }

    buf->data = ptr;
    buf->size = size;
    buf->magic = BUFFER_MAGIC;
    return true;
}

void buffer_free(safe_buffer_t* buf) {
    if (buf == NULL || buf->magic != BUFFER_MAGIC) {
        return;
    }

    /* 单次释放保证 */
    if (buf->data != NULL) {
        /* 释放实现 */
        buf->data = NULL;
    }

    buf->size = 0U;
    buf->magic = 0U;  /* 清除魔数 */
}
```

---

## 未来演进

### ISO/IEC TS 17961 Part 2 (开发中)

根据 WG14 工作文档，Part 2 预计包含：

- 扩展安全规则集
- 多线程安全规则
- 与 C23 标准的深度集成
- AI/ML 相关安全指南

### 与新兴标准的关系

| 标准 | 关系 | 状态 |
|------|------|------|
| ISO/IEC TS 6010 | 来源感知内存对象模型 | 草案 |
| ISO/IEC TR 24772-3 | C语言漏洞描述 | 已发布 |
| MISRA C:2025 | 功能安全补充 | 已发布 |

---

## 参考文档

- [ISO/IEC TS 17961:2013](https://www.iso.org/standard/61134.html)
- [ISO/IEC TS 17961:2013/COR 1:2016](https://www.iso.org/standard/66911.html)
- [GrammaTech ISO 17961 指南](https://www.grammatech.com/iso-iec-ts-17961)
- [CERT C Secure Coding Standard](https://wiki.sei.cmu.edu/confluence/display/c)
- [MISRA C:2025 Guidelines](https://www.misra.org.uk/)

---

*文档版本: 2026-03-18 | 标准版本: ISO/IEC TS 17961:2013/COR 1:2016*
