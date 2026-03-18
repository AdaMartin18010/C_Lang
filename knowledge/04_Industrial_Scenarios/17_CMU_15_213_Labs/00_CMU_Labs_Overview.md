# CMU 15-213 Introduction to Computer Systems Labs

## 1. 课程概述

### 1.1 CMU 15-213简介

CMU 15-213是卡内基梅隆大学的计算机系统入门课程，使用CSAPP教材。

| 项目 | 内容 |
|------|------|
| **课程名称** | Introduction to Computer Systems |
| **教材** | Computer Systems: A Programmer's Perspective |
| **目标** | 理解计算机系统如何影响程序性能、正确性和安全 |

### 1.2 Labs列表

- **Lab 1**: Data Lab - 位操作、整数、浮点
- **Lab 2**: Bomb Lab - 汇编、逆向工程
- **Lab 3**: Attack Lab - 缓冲区溢出、安全
- **Lab 4**: Cache Lab - 缓存模拟与优化
- **Lab 5**: Shell Lab - 进程、信号
- **Lab 6**: Malloc Lab - 动态内存分配
- **Lab 7**: Proxy Lab - 网络编程

---

## 2. 与知识库对应

| CMU Lab | 知识库章节 |
|---------|-----------|
| Data Lab | 数据表示、浮点数 |
| Bomb Lab | x86-64汇编 |
| Attack Lab | 安全编程 |
| Cache Lab | 微架构、缓存 |
| Shell Lab | 系统编程 |
| Malloc Lab | 内存管理 |
| Proxy Lab | 网络编程 |

---

## 3. Lab 1: Data Lab 要点

```c
// 位操作示例
int bitAnd(int x, int y) {
    return ~(~x | ~y);  // 德摩根定律
}

int getByte(int x, int n) {
    return (x >> (n << 3)) & 0xFF;
}
```

---

**创建日期**: 2026-03-19
