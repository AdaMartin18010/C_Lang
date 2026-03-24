# 调试信息格式深度解析

> **层级定位**: 02 Formal Semantics and Physics / 08 Linking Loading Topology
> **参考标准**: DWARF 5, PDB, STABS
> **难度级别**: L4-L5
> **预估学习时间**: 10-12 小时

---

## 📋 目录

- [调试信息格式深度解析](#调试信息格式深度解析)
  - [📋 目录](#-目录)
  - [1. 调试信息概述](#1-调试信息概述)
    - [1.1 为什么需要调试信息](#11-为什么需要调试信息)
    - [1.2 调试信息格式对比](#12-调试信息格式对比)
  - [2. DWARF格式详解](#2-dwarf格式详解)
    - [2.1 DWARF基本概念](#21-dwarf基本概念)
    - [2.2 DWARF节结构](#22-dwarf节结构)
    - [2.3 使用DWARF信息](#23-使用dwarf信息)
    - [2.4 编译生成调试信息](#24-编译生成调试信息)
  - [3. 调用帧信息 (CFI)](#3-调用帧信息-cfi)
    - [3.1 栈回溯原理](#31-栈回溯原理)
    - [3.2 CFI指令](#32-cfi指令)
  - [4. 调试器工作原理](#4-调试器工作原理)
    - [4.1 ptrace系统调用](#41-ptrace系统调用)
    - [4.2 断点实现](#42-断点实现)
  - [关联导航](#关联导航)
    - [前置知识](#前置知识)
    - [后续延伸](#后续延伸)
    - [参考](#参考)

## 1. 调试信息概述

### 1.1 为什么需要调试信息

```
源代码                      编译后                       运行时
┌─────────┐                ┌─────────┐                  ┌─────────┐
│ int x   │    编译        │ movl    │                  │ 0x7fff  │
│ = 42;   │ ────────────>  │ $42,    │    加载运行      │ = 42    │
│         │                │ -4(%rbp)│ ──────────────>  │         │
└─────────┘                └─────────┘                  └─────────┘
     │                          │                            │
     │                          │ 调试信息                    │
     └──────────────────────────┴────────────────────────────┘
                        DWARF/PDB

调试信息的作用：
• 源代码行号映射到机器指令地址
• 变量名映射到内存地址/寄存器
• 类型信息（结构体、数组等）
• 函数参数和局部变量
• 宏定义和内联展开
```

### 1.2 调试信息格式对比

| 格式 | 平台 | 特点 | 工具支持 |
|:-----|:-----|:-----|:---------|
| **DWARF** | Unix/Linux | 标准格式，灵活 | GDB, LLDB |
| **PDB** | Windows | 微软专有 | Visual Studio, WinDbg |
| **STABS** | 老旧Unix | 简单，已过时 | 旧版GDB |
| **COFF** | 嵌入式 | 紧凑 | 嵌入式调试器 |

---

## 2. DWARF格式详解

### 2.1 DWARF基本概念

```
DWARF结构层次：

Compilation Unit (CU)
├── DIE (Debugging Information Entry)
│   ├── TAG: DW_TAG_subprogram (函数)
│   │   ├── DW_AT_name: "main"
│   │   ├── DW_AT_low_pc: 0x1149 (函数起始地址)
│   │   ├── DW_AT_high_pc: 0x11a3 (函数结束地址)
│   │   └── DIE (子条目)
│   │       ├── TAG: DW_TAG_variable
│   │       ├── DW_AT_name: "x"
│   │       └── DW_AT_location: DW_OP_fbreg -4
│   │
│   └── TAG: DW_TAG_base_type (基本类型)
│       ├── DW_AT_name: "int"
│       └── DW_AT_byte_size: 4
│
├── Line Number Table
│   └── 地址到源代码行号的映射
│
├── Macro Information
│   └── 宏定义展开信息
│
└── Call Frame Information (CFI)
    └── 栈帧展开信息（用于回溯）
```

### 2.2 DWARF节结构

```
ELF文件中的DWARF节：

.debug_info      # 主要的调试信息条目(DIE)
.debug_abbrev    # DIE的缩写描述（压缩）
.debug_line      # 行号表
.debug_str       # 字符串表
.debug_loc       # 位置列表
.debug_ranges    # 地址范围
.debug_frame     # 调用帧信息（旧）
.eh_frame        # 异常处理帧信息（也用于调试）
.debug_aranges   # 快速地址查找表
.debug_pubnames  # 全局名称索引
.debug_pubtypes  # 全局类型索引
.debug_macro     # 宏信息（DWARF 5）
```

### 2.3 使用DWARF信息

```c
/*
 * 使用libdw读取DWARF信息
 */

#include <dwarf.h>
#include <elfutils/libdw.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

void print_dwarf_info(const char *filename) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0) return;

    Dwarf *dwarf = dwarf_begin(fd, DWARF_C_READ);
    if (!dwarf) {
        close(fd);
        return;
    }

    // 遍历所有编译单元
    Dwarf_Off cu_offset = 0;
    Dwarf_Off next_cu_offset;

    while (dwarf_nextcu(dwarf, cu_offset, &next_cu_offset,
                        NULL, NULL, NULL, NULL) == 0) {
        Dwarf_Die cu_die;
        if (dwarf_offdie(dwarf, cu_offset, &cu_die) == NULL) {
            cu_offset = next_cu_offset;
            continue;
        }

        // 获取编译单元名称
        Dwarf_Attribute attr;
        const char *name = NULL;
        if (dwarf_attr(&cu_die, DW_AT_name, &attr)) {
            name = dwarf_formstring(&attr);
        }
        printf("Compilation Unit: %s\n", name ? name : "<unknown>");

        // 遍历所有DIE
        Dwarf_Die child;
        if (dwarf_child(&cu_die, &child) == 0) {
            do {
                int tag = dwarf_tag(&child);
                const char *tag_name = dwarf_tag_name(tag);

                // 获取名称属性
                const char *die_name = dwarf_diename(&child);

                if (tag == DW_TAG_subprogram && die_name) {
                    printf("  Function: %s\n", die_name);

                    // 获取地址范围
                    Dwarf_Addr low_pc, high_pc;
                    if (dwarf_lowpc(&child, &low_pc) == 0 &&
                        dwarf_highpc(&child, &high_pc) == 0) {
                        printf("    Range: 0x%lx - 0x%lx\n", low_pc, high_pc);
                    }
                }
                else if (tag == DW_TAG_variable && die_name) {
                    printf("  Variable: %s\n", die_name);
                }
            } while (dwarf_siblingof(&child, &child) == 0);
        }

        cu_offset = next_cu_offset;
    }

    dwarf_end(dwarf);
    close(fd);
}

// 地址到行号映射
void addr2line(const char *filename, Dwarf_Addr addr) {
    int fd = open(filename, O_RDONLY);
    if (fd < 0) return;

    Dwarf *dwarf = dwarf_begin(fd, DWARF_C_READ);
    if (!dwarf) {
        close(fd);
        return;
    }

    Dwarf_Die cudie;
    if (dwarf_addrdie(dwarf, addr, &cudie)) {
        Dwarf_Line *line;
        if (dwarf_getsrc_die(&cudie, addr, &line) == 0) {
            const char *filename = dwarf_linesrc(line, NULL, NULL);
            int lineno = dwarf_lineno(line);
            printf("%s:%d\n", filename, lineno);
        }
    }

    dwarf_end(dwarf);
    close(fd);
}
```

### 2.4 编译生成调试信息

```bash
# GCC生成DWARF调试信息

# 基本调试信息
gcc -g program.c -o program

# 指定DWARF版本
gcc -gdwarf-4 program.c -o program
gcc -gdwarf-5 program.c -o program

# 仅生成最小调试信息（用于生产环境回溯）
gcc -gline-tables-only program.c -o program

# 分离调试信息（常用于发布版本）
gcc -g program.c -o program
objcopy --only-keep-debug program program.debug
strip --strip-debug --strip-unneeded program
objcopy --add-gnu-debuglink=program.debug program

# 查看调试信息大小
cd knowledge/03_System_Technology_Domains/16_OS_Kernel
readelf --debug-dump=info program | head -50
dwarfdump program | head -100
```

---

## 3. 调用帧信息 (CFI)

### 3.1 栈回溯原理

```
栈帧结构（x86-64）：

高地址
┌──────────────────┐
│   参数n          │
│   ...            │
│   参数1          │
│   返回地址       │ ← RSP进入函数时指向这里
├──────────────────┤
│   旧RBP          │ ← 保存的基址指针
├──────────────────┤ ← RBP指向这里
│   局部变量       │
│   ...            │
│   保存的寄存器   │
├──────────────────┤
│   参数构建区     │
└──────────────────┘
低地址  ← RSP当前位置

CFI信息让调试器知道：
• 每个地址处RBP保存在哪里
• 返回地址相对于RBP的位置
• 哪些寄存器被保存及位置
```

### 3.2 CFI指令

```asm
# 汇编中的CFI伪指令

.macro push_registers
    pushq   %rbp
    .cfi_def_cfa_offset 16      # CFA = RSP + 16
    .cfi_offset 6, -16          # RBP保存在CFA-16处

    movq    %rsp, %rbp
    .cfi_def_cfa_register 6     # CFA = RBP + 16

    pushq   %rbx
    .cfi_offset 3, -24          # RBX保存在CFA-24处

    subq    $24, %rsp           # 分配局部变量空间
.endm

# CFI指令说明：
# .cfi_startproc            - 开始过程
# .cfi_endproc              - 结束过程
# .cfi_def_cfa reg, off     - 定义CFA = reg + off
# .cfi_def_cfa_offset off   - 定义CFA = SP + off
# .cfi_def_cfa_register reg - CFA = reg + off（当前off）
# .cfi_offset reg, off      - reg保存在CFA+off处
# .cfi_remember_state       - 记住当前状态
# .cfi_restore_state        - 恢复状态
# .cfi_restore reg          - reg恢复为原始值
```

---

## 4. 调试器工作原理

### 4.1 ptrace系统调用

```c
/*
 * ptrace - 进程跟踪
 * 调试器通过ptrace控制被调试进程
 */

#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <unistd.h>
#include <stdio.h>

void simple_debugger(pid_t child_pid) {
    int status;

    // 等待子进程停止
    waitpid(child_pid, &status, 0);

    if (WIFSTOPPED(status)) {
        printf("Child stopped at start\n");
    }

    // 单步执行
    while (1) {
        // 设置单步标志（x86 TF标志）
        ptrace(PTRACE_SINGLESTEP, child_pid, NULL, NULL);

        waitpid(child_pid, &status, 0);

        if (WIFEXITED(status)) {
            printf("Child exited\n");
            break;
        }

        // 读取寄存器
        struct user_regs_struct regs;
        ptrace(PTRACE_GETREGS, child_pid, NULL, &regs);

        printf("RIP: 0x%llx\n", regs.rip);

        // 读取内存
        long data = ptrace(PTRACE_PEEKDATA, child_pid, regs.rip, NULL);
        printf("Instruction: 0x%lx\n", data);
    }
}

// 设置断点
void set_breakpoint(pid_t pid, void *addr) {
    // 保存原指令
    long original = ptrace(PTRACE_PEEKDATA, pid, addr, NULL);

    // 写入int3 (0xCC) - x86断点指令
    long trap = (original & ~0xFF) | 0xCC;
    ptrace(PTRACE_POKEDATA, pid, addr, trap);

    // 保存原始值以便恢复
    breakpoint_table[addr] = original;
}

void remove_breakpoint(pid_t pid, void *addr) {
    // 恢复原始指令
    ptrace(PTRACE_POKEDATA, pid, addr, breakpoint_table[addr]);
}
```

### 4.2 断点实现

```
软件断点：
1. 调试器读取断点地址的原始指令（1字节）
2. 用中断指令(0xCC)替换
3. 被调试进程执行到该地址时触发SIGTRAP
4. 调试器捕获信号，恢复原始指令
5. 回退指令指针，单步执行原始指令
6. 再次设置断点，继续执行

硬件断点：
• 使用CPU调试寄存器（x86 DR0-DR3）
• 最多4个硬件断点
• 可以设置数据断点（读写监控）
• 无需修改代码

内存断点（页面保护）：
• mprotect设置页面为PROT_NONE
• 访问时触发SIGSEGV
• 用于实现"数据观察点"
```

---

## 关联导航

### 前置知识

- [静态链接深度](./03_Static_Linking_Deep.md)
- [ELF文件格式](../08_Linking_Loading_Topology/readme.md)
- [x86-64汇编](../10_ISA_Machine_Code/01_x86_64_Assembly.md)

### 后续延伸

- [GDB调试器实现](../16_OS_Kernel/05_GDB_Debugger_Implementation.md)
- [性能分析工具](../16_Performance_Analysis_Hardware/readme.md)

### 参考

- DWARF 5 Standard (<http://www.dwarfstd.org/>)
- System V ABI x86-64
- GDB Internals Manual
