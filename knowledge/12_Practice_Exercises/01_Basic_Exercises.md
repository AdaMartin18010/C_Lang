# 基础编程练习 (Basic Exercises)

> **难度**: L1-L2 | **预计时间**: 4-6小时

---

## 练习1: 温度转换器

### 描述

编写一个程序，在摄氏度和华氏度之间进行转换。

### 要求

- 支持摄氏度转华氏度: `F = C * 9/5 + 32`
- 支持华氏度转摄氏度: `C = (F - 32) * 5/9`
- 处理无效输入（负数温度在科学上有效，但超出合理范围时警告）
- 使用命令行参数或交互式输入

### 示例

```text
$ ./temp_converter -c 100
100°C = 212°F

$ ./temp_converter -f 32
32°F = 0°C
```

### 参考答案框架

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

double c_to_f(double c) {
    return c * 9.0 / 5.0 + 32.0;
}

double f_to_c(double f) {
    return (f - 32.0) * 5.0 / 9.0;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s [-c|-f] temperature\n", argv[0]);
        return 1;
    }

    char *endptr;
    double temp = strtod(argv[2], &endptr);

    if (*endptr != '\0' || errno == ERANGE) {
        fprintf(stderr, "Error: Invalid temperature value\n");
        return 1;
    }

    if (strcmp(argv[1], "-c") == 0) {
        printf("%.1f°C = %.1f°F\n", temp, c_to_f(temp));
    } else if (strcmp(argv[1], "-f") == 0) {
        printf("%.1f°F = %.1f°C\n", temp, f_to_c(temp));
    } else {
        fprintf(stderr, "Error: Invalid flag. Use -c or -f\n");
        return 1;
    }

    return 0;
}
```

---

## 练习2: 素数生成器

### 描述

使用埃拉托斯特尼筛法生成素数。

### 要求

- 生成小于N的所有素数
- N通过命令行参数指定
- 优化内存使用（使用位图而非字节数组）
- 计算并显示程序运行时间

### 进阶要求

- 使用多线程并行筛选
- 支持超大范围（使用分段筛法）

### 示例

```text
$ ./primes 100
2 3 5 7 11 13 17 19 23 29 31 37 41 43 47 53 59 61 67 71 73 79 83 89 97
Found 25 primes in 0.001s
```

---

## 练习3: 字符串反转与回文检测

### 描述

实现字符串操作函数。

### 要求

1. 实现 `void reverse(char *str)` 原地反转字符串
2. 实现 `int is_palindrome(const char *str)` 检测回文
3. 实现 `void reverse_words(char *str)` 反转单词顺序

### 示例

```text
输入: "Hello, World!"
反转: "!dlroW ,olleH"
回文检测: false
单词反转: "World! Hello,"
```

### 关键考点

- 原地操作（不使用额外内存）
- 处理Unicode/多字节字符
- 边界情况（空字符串、单字符）

---

## 练习4: 简易计算器

### 描述

实现一个支持基本运算的表达式计算器。

### 要求

- 支持 `+`, `-`, `*`, `/`, `%` 运算符
- 支持括号优先级
- 处理错误输入
- 使用递归下降解析

### 示例

```text
$ ./calc "3 + 4 * 2 / (1 - 5)"
Result: 1.000000

$ ./calc "10 / 0"
Error: Division by zero
```

### 语法定义

```text
expr    := term { ('+' | '-') term }
term    := factor { ('*' | '/' | '%') factor }
factor  := number | '(' expr ')'
number  := [0-9]+ ('.' [0-9]+)?
```

---

## 练习5: 文件加密工具

### 描述

实现一个简单的XOR加密工具。

### 要求

- 从文件读取数据，使用密码加密/解密
- 支持命令行指定操作模式（加密/解密）
- 密钥派生（使用简单方法或PBKDF2）
- 处理大文件（流式处理，不一次性加载）

### 示例

```text
$ ./crypt -e -i input.txt -o output.enc -k "mypassword"
Encryption complete: input.txt -> output.enc

$ ./crypt -d -i output.enc -o decrypted.txt -k "mypassword"
Decryption complete: output.enc -> decrypted.txt
```

### 安全提示

```text
⚠️ 注意：XOR加密仅供学习使用，不安全！
实际应用应使用AES-GCM或ChaCha20-Poly1305
```

---

## 练习6: 统计文本文件

### 描述

实现 `wc` (word count) 工具的简化版。

### 要求

统计文件的：

- 字节数
- 行数
- 单词数
- 字符数（多字节字符正确计数）

### 示例

```text
$ ./mywc test.txt
  12   45  389 test.txt
(行数 单词数 字节数 文件名)
```

### 进阶

- 支持多个文件
- 支持通配符
- 输出格式与系统wc一致

---

## 练习7: 链表基础操作

### 描述

实现单向链表的基本操作。

### 要求

```c
typedef struct Node {
    int data;
    struct Node *next;
} Node;

// 需要实现:
Node* list_create(void);           // 创建空链表
void list_append(Node **head, int value);     // 尾部添加
void list_prepend(Node **head, int value);    // 头部添加
int list_remove(Node **head, int value);      // 删除节点
Node* list_find(Node *head, int value);       // 查找节点
void list_reverse(Node **head);               // 反转链表
void list_free(Node **head);                  // 释放链表
size_t list_length(Node *head);               // 获取长度
void list_print(Node *head);                  // 打印链表
```

### 测试用例

```c
int main() {
    Node *list = list_create();

    list_append(&list, 1);
    list_append(&list, 2);
    list_append(&list, 3);
    list_print(list);  // 1 -> 2 -> 3 -> NULL

    list_reverse(&list);
    list_print(list);  // 3 -> 2 -> 1 -> NULL

    list_free(&list);
    return 0;
}
```

---

## 练习8: 学生成绩管理系统

### 描述

实现一个简单的学生成绩管理系统。

### 要求

- 结构体定义学生信息（姓名、学号、多科成绩）
- 支持添加、删除、查询学生
- 计算平均分、最高分、最低分
- 按成绩排序
- 数据持久化（保存到文件/从文件加载）

### 数据结构

```c
typedef struct {
    char name[50];
    int id;
    float scores[5];  // 5门课程
    float average;
} Student;

typedef struct {
    Student *students;
    size_t count;
    size_t capacity;
} StudentDB;
```

### 功能菜单

```text
1. Add student
2. Remove student
3. Query student
4. List all students
5. Sort by average score
6. Save to file
7. Load from file
0. Exit
```

---

## 评估检查清单

完成练习后，检查以下项目：

- [ ] 代码正确编译，无警告
- [ ] 所有边界情况已处理
- [ ] 内存无泄漏（使用valgrind检查）
- [ ] 代码有适当注释
- [ ] 使用版本控制（git）
- [ ] 编写单元测试（可选）

---

> **返回导航**: [练习模块总览](./README.md) | [知识库总览](../README.md)
