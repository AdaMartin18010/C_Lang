# C语言编译与构建

> **层级定位**: 01_Core_Knowledge_System > 05_Engineering_Layer

---

## 编译过程

### 四阶段编译

```
源代码(.c) → 预处理(.i) → 编译(.s) → 汇编(.o) → 链接(可执行)
    gcc -E       gcc -S       gcc -c      gcc -o
```

### 编译命令

```bash
# 完整编译
gcc program.c -o program

# 分步编译
gcc -E program.c > program.i  # 预处理
gcc -S program.i              # 编译为汇编
gcc -c program.s              # 汇编为目标文件
gcc program.o -o program      # 链接

# 包含调试信息
gcc -g program.c -o program

# 优化级别
gcc -O0 program.c -o program  # 无优化
gcc -O2 program.c -o program  # 标准优化
gcc -O3 program.c -o program  # 激进优化
```

---

## 构建系统

### Makefile基础

```makefile
CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGET = program
SOURCES = main.c utils.c
OBJECTS = $(SOURCES:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
 $(CC) $(OBJECTS) -o $@

%.o: %.c
 $(CC) $(CFLAGS) -c $< -o $@

clean:
 rm -f $(OBJECTS) $(TARGET)
```

### CMake基础

```cmake
# CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(MyProject)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "-Wall -Wextra")

add_executable(program main.c utils.c)
```

```bash
mkdir build && cd build
cmake ..
make
```

---

> **最后更新**: 2026-03-21


---

## 扩展内容

### 核心概念

本节详细介绍核心概念和原理。

### 实践应用

实际应用场景和代码示例。

### 深入学习

- 相关主题1
- 相关主题2
- 相关主题3

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
