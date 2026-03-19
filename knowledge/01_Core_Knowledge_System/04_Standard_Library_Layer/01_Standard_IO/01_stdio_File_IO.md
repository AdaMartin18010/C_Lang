# 标准I/O与文件操作

> **层级定位**: 01 Core Knowledge System / 04 Standard Library Layer / 01 Standard_IO
> **对应标准**: ISO C §7.21, POSIX §stdio.h
> **难度级别**: L2-L4
> **预估学习时间**: 6-8小时

---

## 📋 本节概要

| 属性 | 内容 |
|:-----|:-----|
| **核心概念** | 文件流、缓冲机制、格式化I/O、二进制I/O、错误处理 |
| **前置知识** | 基本语法、指针 |
| **后续延伸** | POSIX I/O、网络编程 |
| **权威来源** | ISO C标准, K&R C §7 |

---

## 📖 1. 文件流基础

### 1.1 打开与关闭文件

```c
#include <stdio.h>

int main() {
    // 打开文件进行读取
    FILE *fp = fopen("data.txt", "r");
    if (fp == NULL) {
        perror("Failed to open file");
        return 1;
    }

    // 使用文件...

    // 关闭文件
    if (fclose(fp) != 0) {
        perror("Failed to close file");
        return 1;
    }

    return 0;
}
```

### 1.2 文件打开模式

| 模式 | 含义 | 文件不存在 | 文件存在 |
|:-----|:-----|:----------:|:--------:|
| `"r"` | 只读 | 失败 | 从头读取 |
| `"w"` | 只写 | 创建 | 截断 |
| `"a"` | 追加 | 创建 | 末尾追加 |
| `"r+"` | 读写 | 失败 | 从头读写 |
| `"w+"` | 读写 | 创建 | 截断 |
| `"a+"` | 读写 | 创建 | 读从头，写追加 |
| `"rb"` | 二进制只读 | 同上 | 同上 |
| `"wb"` | 二进制只写 | 同上 | 同上 |

```c
#include <stdio.h>

void demonstrate_modes() {
    // 文本模式
    FILE *fp1 = fopen("text.txt", "w");    // 文本写
    FILE *fp2 = fopen("data.bin", "wb");   // 二进制写

    // Windows: 文本模式下'\n'会被转换为"\r\n"
    // 二进制模式: 原始字节

    fclose(fp1);
    fclose(fp2);
}
```

---

## 📖 2. 格式化I/O

### 2.1 printf/scanf家族

```c
#include <stdio.h>

void formatted_io_demo() {
    // printf - 格式化输出到stdout
    printf("Integer: %d\n", 42);
    printf("Float: %.2f\n", 3.14159);
    printf("String: %s\n", "hello");
    printf("Hex: %x, Oct: %o\n", 255, 8);
    printf("Pointer: %p\n", (void*)&formatted_io_demo);

    // sprintf - 输出到字符串（不安全，可能溢出）
    char buffer[100];
    sprintf(buffer, "Value: %d", 42);

    // snprintf - 安全版本
    snprintf(buffer, sizeof(buffer), "Value: %d", 42);

    // fprintf - 输出到文件流
    FILE *fp = fopen("output.txt", "w");
    fprintf(fp, "Error code: %d\n", 404);
    fclose(fp);

    // scanf - 从stdin读取
    int num;
    char str[50];
    printf("Enter number and string: ");
    scanf("%d %49s", &num, str);

    // fscanf - 从文件读取
    // sscanf - 从字符串读取
}
```

### 2.2 格式说明符详解

```c
#include <stdio.h>
#include <stdint.h>

void format_specifiers() {
    // 整数类型
    int8_t  i8 = -128;
    int16_t i16 = -32768;
    int32_t i32 = -2147483648;
    int64_t i64 = -9223372036854775807LL;

    printf("int8:  %" PRId8 "\n",  i8);   // 或 %hhd
    printf("int16: %" PRId16 "\n", i16);  // 或 %hd
    printf("int32: %" PRId32 "\n", i32);  // 或 %d
    printf("int64: %" PRId64 "\n", i64);  // 或 %lld

    // 宽度与精度
    printf("[%10d]\n", 42);     // [        42] 右对齐
    printf("[%-10d]\n", 42);    // [42        ] 左对齐
    printf("[%010d]\n", 42);    // [0000000042] 前导零
    printf("[%.3f]\n", 3.14);   // [3.140] 小数位
    printf("[%*d]\n", 10, 42);  // 宽度从参数获取
}
```

---

## 📖 3. 字符与行I/O

### 3.1 字符级操作

```c
#include <stdio.h>

void char_io_demo() {
    // putc/getc - 带缓冲
    FILE *fp = fopen("test.txt", "r+");

    int c;
    while ((c = getc(fp)) != EOF) {
        putchar(c);
    }

    // putchar/getchar - 操作stdout/stdin
    putchar('A');
    c = getchar();

    // ungetc - 将字符放回输入流
    ungetc('X', fp);

    fclose(fp);
}
```

### 3.2 行级操作

```c
#include <stdio.h>
#include <stdlib.h>

void line_io_demo() {
    // fgets - 安全读取一行
    char buffer[256];
    FILE *fp = fopen("lines.txt", "r");

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        // 移除末尾的换行符
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
        printf("Line: %s\n", buffer);
    }

    fclose(fp);

    // fputs - 写入字符串
    FILE *out = fopen("output.txt", "w");
    fputs("Hello, World!\n", out);
    fclose(out);
}

// 使用getline (POSIX扩展)
#ifdef __linux__
#include <stdlib.h>
void posix_line_io() {
    FILE *fp = fopen("lines.txt", "r");
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, fp)) != -1) {
        printf("Retrieved line of length %zu: %s", read, line);
    }

    free(line);
    fclose(fp);
}
#endif
```

---

## 📖 4. 二进制I/O

```c
#include <stdio.h>
#include <stdlib.h>

// 写入二进制数据
void write_binary() {
    typedef struct {
        int id;
        double value;
        char name[32];
    } Record;

    Record records[] = {
        {1, 3.14, "Record 1"},
        {2, 2.71, "Record 2"},
        {3, 1.41, "Record 3"}
    };

    FILE *fp = fopen("data.bin", "wb");
    if (!fp) {
        perror("fopen");
        return;
    }

    // 写入记录数量
    size_t count = sizeof(records) / sizeof(records[0]);
    fwrite(&count, sizeof(count), 1, fp);

    // 写入所有记录
    fwrite(records, sizeof(Record), count, fp);

    fclose(fp);
}

// 读取二进制数据
void read_binary() {
    typedef struct {
        int id;
        double value;
        char name[32];
    } Record;

    FILE *fp = fopen("data.bin", "rb");
    if (!fp) {
        perror("fopen");
        return;
    }

    // 读取记录数量
    size_t count;
    fread(&count, sizeof(count), 1, fp);

    // 分配内存并读取
    Record *records = malloc(count * sizeof(Record));
    fread(records, sizeof(Record), count, fp);

    for (size_t i = 0; i < count; i++) {
        printf("ID: %d, Value: %.2f, Name: %s\n",
               records[i].id, records[i].value, records[i].name);
    }

    free(records);
    fclose(fp);
}
```

---

## 📖 5. 缓冲与刷新

```c
#include <stdio.h>
#include <unistd.h>

void buffering_demo() {
    // 三种缓冲模式
    // 1. 全缓冲 - 块设备（文件）
    // 2. 行缓冲 - 终端（stdout）
    // 3. 无缓冲 - stderr

    // 设置缓冲模式
    FILE *fp = fopen("buffered.txt", "w");
    setvbuf(fp, NULL, _IOFBF, 4096);  // 全缓冲，4KB

    // 手动刷新
    fprintf(fp, "Important data");
    fflush(fp);  // 立即写入

    // 禁用缓冲
    setvbuf(fp, NULL, _IONBF, 0);

    fclose(fp);
}
```

---

## ⚠️ 常见陷阱

### 陷阱 FILE01: 未检查fopen返回值

```c
// 错误
FILE *fp = fopen("file.txt", "r");
fprintf(fp, "data");  // 如果文件不存在，fp为NULL，崩溃！

// 正确
FILE *fp = fopen("file.txt", "r");
if (!fp) {
    perror("fopen failed");
    return 1;
}
```

### 陷阱 FILE02: 混淆文本与二进制模式

```c
// Windows上，文本模式会将\r\n转换为\n
// 错误：用文本模式读写二进制数据
FILE *fp = fopen("image.jpg", "r");  // 应该用"rb"!
```

### 陷阱 FILE03: 缓冲区溢出

```c
char buffer[10];
fscanf(fp, "%s", buffer);  // 危险！可能溢出

// 正确做法
fscanf(fp, "%9s", buffer);  // 限制读取长度
```

---

## ✅ 质量验收清单

- [x] 文件打开/关闭操作
- [x] 打开模式详解
- [x] 格式化I/O函数
- [x] 字符级I/O
- [x] 行级I/O
- [x] 二进制I/O
- [x] 缓冲机制
- [x] 常见陷阱分析

---

> **更新记录**
>
> - 2025-03-09: 创建，补充核心缺失主题


---

## 深入理解

### 技术原理

深入探讨相关技术原理和实现细节。

### 实践指南

- 步骤1：理解基础概念
- 步骤2：掌握核心原理
- 步骤3：应用实践

### 相关资源

- 文档链接
- 代码示例
- 参考文章

---

> **最后更新**: 2026-03-21  
> **维护者**: AI Code Review
