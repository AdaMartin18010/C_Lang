# 逆向工程基础

> **层级定位**: 03 System Technology Domains / 10 Security Engineering
> **参考课程**: CMU 18-330, Berkeley CS161
> **难度级别**: L4-L5
> **预估学习时间**: 12-15 小时

---

## 目录

- [逆向工程基础](#逆向工程基础)
  - [目录](#目录)
  - [1. 逆向工程概述](#1-逆向工程概述)
    - [1.1 什么是逆向工程](#11-什么是逆向工程)
    - [1.2 分析流程](#12-分析流程)
  - [2. 文件格式分析](#2-文件格式分析)
    - [2.1 ELF文件结构](#21-elf文件结构)
    - [2.2 PE文件结构](#22-pe文件结构)
  - [3. 反汇编与反编译](#3-反汇编与反编译)
    - [3.1 x86-64汇编基础](#31-x86-64汇编基础)
    - [3.2 反汇编工具](#32-反汇编工具)
    - [3.3 反编译原理](#33-反编译原理)
  - [4. 动态分析](#4-动态分析)
    - [4.1 GDB调试](#41-gdb调试)
    - [4.2 动态插桩](#42-动态插桩)
  - [5. 混淆与反混淆](#5-混淆与反混淆)
    - [5.1 常见混淆技术](#51-常见混淆技术)
    - [5.2 反混淆策略](#52-反混淆策略)
  - [关联导航](#关联导航)
    - [前置知识](#前置知识)
    - [后续延伸](#后续延伸)
    - [参考](#参考)

## 1. 逆向工程概述

### 1.1 什么是逆向工程

```
逆向工程 (Reverse Engineering):
从可执行代码还原设计、架构或源代码的过程

应用场景：
┌─────────────────────────────────────────────────────────────┐
│  安全研究              恶意软件分析          互操作性       │
│  • 漏洞挖掘            • 行为分析            • 文件格式解析  │
│  • 补丁分析            • 家族归属             • 协议实现    │
│  • 利用开发            • 清除技术              • 驱动开发   │
├─────────────────────────────────────────────────────────────┤
│  软件破解              数字取证              教学研究       │
│  • 许可证验证          • 证据恢复             • 编译器学习  │
│  • 算法提取            • 行为重建              • 病毒分析   │
└─────────────────────────────────────────────────────────────┘

法律边界：
• ✓ 自己拥有的软件
• ✓ 安全研究（负责任披露）
• ✓ 互操作性目的
• ✗ 商业盗版
• ✗ 恶意攻击
```

### 1.2 分析流程

```
┌─────────────────────────────────────────────────────────────┐
│                    逆向分析流程                              │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌─────────────┐                                            │
│  │  静态分析    │                                            │
│  │  • 文件格式  │  ┌─────────────┐  ┌─────────────┐        │
│  │  • 字符串   │  │  动态分析    │  │  符号执行    │        │
│  │  • 反汇编   │──│  • 调试     │──│  • 路径探索  │        │
│  │  • 反编译   │  │  • 跟踪     │  │  • 约束求解  │        │
│  └─────────────┘  │  • 钩子     │  └─────────────┘        │
│         │         └─────────────┘           │              │
│         │                │                   │              │
│         └────────────────┴───────────────────┘              │
│                          │                                  │
│                          ▼                                  │
│                  ┌─────────────┐                           │
│                  │  行为分析    │                           │
│                  │  • 控制流   │                           │
│                  │  • 数据流   │                           │
│                  │  • 算法还原 │                           │
│                  └─────────────┘                           │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## 2. 文件格式分析

### 2.1 ELF文件结构

```
ELF可执行文件：

┌─────────────────────────────────────────────────────────────┐
│                      ELF Header                             │
│  e_ident[EI_MAG0-3]: 0x7F 'E' 'L' 'F'                      │
│  e_ident[EI_CLASS]: 32/64位                                │
│  e_ident[EI_DATA]: 小端/大端                               │
│  e_type: ET_REL/ET_EXEC/ET_DYN/ET_CORE                     │
│  e_machine: x86/ARM/x64...                                 │
│  e_entry: 入口点地址                                        │
│  e_phoff: 程序头表偏移                                      │
│  e_shoff: 节头表偏移                                        │
└─────────────────────────────────────────────────────────────┘
│                  Program Header Table                       │
│  (加载信息，运行时必需)                                      │
│  PT_LOAD: 可加载段                                          │
│  PT_DYNAMIC: 动态链接信息                                    │
│  PT_INTERP: 解释器路径（/lib64/ld-linux-x86-64.so.2）        │
└─────────────────────────────────────────────────────────────┘
│                      .text Section                          │
│  (代码段，可执行、只读)                                      │
└─────────────────────────────────────────────────────────────┘
│                      .rodata Section                        │
│  (只读数据：字符串、常量)                                    │
└─────────────────────────────────────────────────────────────┘
│                      .data Section                          │
│  (已初始化全局变量)                                          │
└─────────────────────────────────────────────────────────────┘
│                      .bss Section                           │
│  (未初始化全局变量，不占文件空间)                             │
└─────────────────────────────────────────────────────────────┘
│                      Section Header Table                   │
│  (链接信息，调试信息)                                        │
│  .symtab: 符号表                                             │
│  .strtab: 字符串表                                           │
│  .rel.*: 重定位表                                            │
└─────────────────────────────────────────────────────────────┘

分析工具：
• readelf -a program      # 查看所有信息
• objdump -d program      # 反汇编
• objdump -h program      # 节头信息
• nm program              # 符号表
• strings program         # 提取字符串
```

### 2.2 PE文件结构

```
Windows PE文件：

┌─────────────────────────────────────────────────────────────┐
│                      DOS Header                             │
│  e_magic: "MZ"                                              │
│  e_lfanew: PE头偏移                                         │
└─────────────────────────────────────────────────────────────┘
│                      DOS Stub                               │
│  "This program cannot be run in DOS mode"                   │
└─────────────────────────────────────────────────────────────┘
│                      PE Signature                           │
│  "PE\0\0"                                                   │
└─────────────────────────────────────────────────────────────┘
│                      COFF Header                            │
│  Machine: x86/x64/ARM                                       │
│  NumberOfSections                                           │
│  TimeDateStamp                                              │
│  Characteristics                                            │
└─────────────────────────────────────────────────────────────┘
│                      Optional Header                        │
│  Magic: PE32/PE32+                                          │
│  AddressOfEntryPoint                                        │
│  ImageBase                                                  │
│  SectionAlignment/FileAlignment                             │
│  DataDirectories[16]                                        │
└─────────────────────────────────────────────────────────────┘
│                      Section Table                          │
│  .text: 代码                                                │
│  .data: 数据                                                │
│  .rsrc: 资源                                                │
│  .reloc: 重定位                                             │
└─────────────────────────────────────────────────────────────┘

分析工具：
• dumpbin /all program.exe
• CFF Explorer
• PE-bear
```

---

## 3. 反汇编与反编译

### 3.1 x86-64汇编基础

```asm
; 调用约定 (System V AMD64 ABI)
; 参数传递：RDI, RSI, RDX, RCX, R8, R9, 栈
; 返回值：RAX
; 调用者保存：RAX, RCX, RDX, RSI, RDI, R8-R11
; 被调者保存：RBX, RBP, R12-R15

; 函数序言 (Function Prologue)
push rbp
mov rbp, rsp
sub rsp, 0x20       ; 分配栈空间

; 函数尾声 (Function Epilogue)
leave               ; mov rsp, rbp; pop rbp
ret

; 常见指令模式
; 1. 函数调用
call <target>       ; 调用函数
                    ; push rip; jmp target

; 2. 条件跳转
cmp rax, rbx
je equal_label      ; jump if equal
jne not_equal       ; jump if not equal
jg greater          ; jump if greater (signed)
ja above            ; jump if above (unsigned)

; 3. 循环
mov ecx, 10
loop_start:
    ; 循环体
    dec ecx
    jnz loop_start

; 4. 数组访问
; arr[i] = *(base + i * sizeof(element))
mov rax, [rbx + rdi*8]  ; 64位数组访问

; 5. 结构体访问
; ptr->field = *(ptr + offset)
mov eax, [rdi + 0x10]   ; 访问偏移0x10的字段
```

### 3.2 反汇编工具

```bash
# ========== objdump ==========
objdump -d program              # 反汇编代码段
objdump -D program              # 反汇编所有段
objdump -M intel -d program     # Intel语法
objdump -t program              # 符号表

# ========== radare2 ==========
r2 -A program                   # 分析并打开
[0x00000000]> aaa               # 完整分析
[0x00000000]> afl               # 列出函数
[0x00000000]> s main            # 跳转到main
[0x00000000]> pdf               # 显示函数反汇编
[0x00000000]> VV                # 图形化控制流
[0x00000000]> iz                # 查看字符串

# ========== Ghidra ==========
# 图形界面，自动反编译为C伪代码

# ========== IDA Pro ==========
# 商业标准工具，功能最全面
```

### 3.3 反编译原理

```c
// 反汇编代码
push    rbp
mov     rbp, rsp
mov     DWORD PTR [rbp-4], 5
mov     DWORD PTR [rbp-8], 10
mov     edx, DWORD PTR [rbp-4]
mov     eax, DWORD PTR [rbp-8]
add     eax, edx
pop     rbp
ret

// 反编译后的C代码
int func() {
    int a = 5;
    int b = 10;
    return a + b;
}

// 模式识别：
// 1. [rbp-4], [rbp-8] 是局部变量
// 2. mov + add 对应 + 操作
// 3. eax 是返回值
```

---

## 4. 动态分析

### 4.1 GDB调试

```bash
# ========== GDB基础 ==========
gdb ./program                   # 启动
gdb -p <pid>                    # 附加到进程

# 断点
(gdb) break main                # 函数断点
(gdb) break *0x401000           # 地址断点
(gdb) break 42                  # 行号断点
(gdb) info breakpoints          # 列出断点
(gdb) delete 1                  # 删除断点1

# 执行
(gdb) run [args]                # 运行
(gdb) continue                  # 继续
(gdb) step                      # 单步进入
(gdb) next                      # 单步跳过
(gdb) finish                    # 运行到函数返回
(gdb) until                     # 运行到下一行

# 检查
(gdb) print variable            # 打印变量
(gdb) print/x $rax              # 十六进制打印寄存器
(gdb) x/10gx $rsp               # 检查栈
(gdb) info registers            # 所有寄存器
(gdb) disassemble               # 反汇编当前函数

# 内存
(gdb) x/16wx 0x401000           # 检查内存
(gdb) find &system,+9999999,"/bin/sh"  # 搜索字符串
```

### 4.2 动态插桩

```c
// ========== LD_PRELOAD钩子 ==========
// hook.c
#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>

typedef int (*original_malloc_t)(size_t);

void *malloc(size_t size) {
    original_malloc_t orig = dlsym(RTLD_NEXT, "malloc");
    void *ptr = orig(size);
    printf("[HOOK] malloc(%zu) = %p\n", size, ptr);
    return ptr;
}

// 编译：gcc -shared -fPIC hook.c -o hook.so -ldl
// 使用：LD_PRELOAD=./hook.so ./program

// ========== ptrace系统调用 ==========
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <unistd.h>

void trace_process(pid_t pid) {
    ptrace(PTRACE_ATTACH, pid, NULL, NULL);
    waitpid(pid, NULL, 0);

    // 单步执行
    ptrace(PTRACE_SINGLESTEP, pid, NULL, NULL);
    waitpid(pid, NULL, 0);

    // 读取寄存器
    struct user_regs_struct regs;
    ptrace(PTRACE_GETREGS, pid, NULL, &regs);
    printf("RIP: %llx\n", regs.rip);

    ptrace(PTRACE_DETACH, pid, NULL, NULL);
}
```

---

## 5. 混淆与反混淆

### 5.1 常见混淆技术

```
┌─────────────────────────────────────────────────────────────┐
│                    代码混淆技术                              │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  1. 控制流混淆                                              │
│     • 扁平化控制流：所有基本块放入大switch                 │
│     • 不透明谓词：永真/永假条件，但难以静态分析              │
│     • 间接跳转：通过计算得到的跳转目标                       │
│                                                             │
│  2. 数据混淆                                                │
│     • 常量编码：x = encoded_value; x = decode(x)            │
│     • 数组变换：分散存储、索引变换                           │
│     • 字符串加密：运行时解密                                 │
│                                                             │
│  3. 布局混淆                                                │
│     • 函数内联/外联                                         │
│     • 虚假控制流：插入死代码                                 │
│     • 指令重排：打乱指令顺序                                 │
│                                                             │
│  4. 反调试/反分析                                           │
│     • 检测调试器 (ptrace, /proc/self/status)               │
│     • 代码校验：检查代码段哈希                               │
│     • 时间检查：检测执行暂停                                 │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### 5.2 反混淆策略

```
┌─────────────────────────────────────────────────────────────┐
│                    反混淆策略                                │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  1. 动态分析为主                                            │
│     • 在运行时观察真实行为                                   │
│     • 记录代码覆盖路径                                       │
│     • 使用Taint分析追踪数据流                                │
│                                                             │
│  2. 符号执行                                                │
│     • 用符号值代替具体值执行                                 │
│     • 求解约束，发现可行路径                                  │
│     • Angr, Triton, Manticore                               │
│                                                             │
│  3. 机器学习辅助                                            │
│     • 识别混淆模式                                           │
│     • 预测原始控制流                                         │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## 关联导航

### 前置知识

- [汇编语言基础](../../02_Formal_Semantics_and_Physics/10_ISA_Machine_Code/readme.md)
- [链接与加载](../../02_Formal_Semantics_and_Physics/08_Linking_Loading_Topology/readme.md)

### 后续延伸

- [漏洞分析与利用](./09_Exploit_Development.md)
- [恶意软件分析](./10_Malware_Analysis.md)

### 参考

- "Reversing: Secrets of Reverse Engineering", Eldad Eilam
- "Practical Reverse Engineering", Dang et al.
- "The IDA Pro Book", Chris Eagle
