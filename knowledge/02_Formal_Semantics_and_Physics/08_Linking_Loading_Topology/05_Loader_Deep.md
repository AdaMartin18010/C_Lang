# 程序加载器深度解析

> **层级定位**: 02 Formal Semantics and Physics / 08 Linking_Loading_Topology
> **难度级别**: L5 专家
> **预估学习时间**: 6-8 小时
> **前置知识**: 动态链接、虚拟内存、系统调用

---

## 1. 加载器概述

### 1.1 加载器的作用

程序加载器负责将可执行文件从磁盘加载到内存，并准备执行环境。

```
可执行文件(ELF)          内存中的进程
      │                       │
      │ 1. 读取ELF头           │
      │ 2. 创建地址空间        │
      │ 3. 映射PT_LOAD段       │
      │ 4. 设置权限(rwx)       │
      │ 5. 分配栈和堆          │
      │ 6. 设置入口点          │
      │ 7. 传递参数(env/argv)  │
      ▼                       ▼
┌───────────┐           ┌───────────┐
│ 磁盘文件   │  加载器    │ 进程映像   │
└───────────┘  ─────→   └───────────┘
                              │
                              ▼
                          执行入口
```

### 1.2 两种加载方式

**内核加载器** (execve):

- 内核空间执行
- 处理解释器脚本(#!/bin/sh)
- 处理动态链接程序(PT_INTERP)

**用户态加载器** (ld.so):

- 动态链接器本身
- 加载依赖的共享库
- 执行重定位和初始化

---

## 2. execve系统调用

### 2.1 execve执行流程

```c
int execve(const char *pathname, char *const argv[], char *const envp[]);
```

**内核执行流程**:

```
sys_execve()
    │
    ├── 1. 打开可执行文件
    │   └── do_open_execat()
    │
    ├── 2. 检查并加载二进制格式
    │   ├── search_binary_handler()  // 查找合适的加载器
    │   │   ├── 尝试ELF: load_elf_binary()
    │   │   ├── 尝试脚本: load_script()
    │   │   └── 尝试其他: a.out, etc.
    │   │
    │   └── load_elf_binary()        // ELF加载入口
    │       ├── 读取并验证ELF头
    │       ├── 检查架构匹配
    │       └── 处理PT_INTERP
    │
    ├── 3. 设置新地址空间
    │   ├── flush_old_exec()         // 清理旧地址空间
    │   └── setup_new_exec()         // 创建新地址空间
    │
    ├── 4. 映射程序段
    │   ├── elf_map()                // 映射PT_LOAD段
    │   ├── 设置内存权限
    │   └── 处理BSS段
    │
    ├── 5. 设置栈
    │   └── setup_arg_pages()
    │       ├── 分配栈内存
    │       ├── 复制argv/envp
    │       └── 设置辅助向量(auxv)
    │
    ├── 6. 处理动态链接
    │   └── 如果有PT_INTERP:
    │       └── 加载并执行ld.so
    │
    ├── 7. 设置入口点
    │   └── start_thread()
    │       ├── 设置PC = e_entry
    │       └── 设置SP = 栈顶
    │
    └── 8. 返回到用户态
        └── 程序开始执行
```

### 2.2 辅助向量 (Auxiliary Vector)

辅助向量是内核传递给动态链接器的关键信息：

```c
// 辅助向量条目
struct auxv {
    unsigned long a_type;
    unsigned long a_val;
};

// 关键类型
#define AT_NULL     0    // 结束标记
#define AT_IGNORE   1    // 忽略
#define AT_EXECFD   2    // 可执行文件描述符
#define AT_PHDR     3    // 程序头表地址
#define AT_PHENT    4    // 程序头表项大小
#define AT_PHNUM    5    // 程序头表项数量
#define AT_PAGESZ   6    // 页面大小
#define AT_BASE     7    // 解释器基地址
#define AT_FLAGS    8    // 标志
#define AT_ENTRY    9    // 程序入口点
#define AT_NOTELF   10   // 非ELF程序
#define AT_UID      11   // 真实UID
#define AT_EUID     12   // 有效UID
#define AT_GID      13   // 真实GID
#define AT_EGID     14   // 有效GID
#define AT_PLATFORM 15   // 平台字符串
#define AT_HWCAP    16   // 硬件能力
#define AT_CLKTCK   17   // 时钟频率
#define AT_SECURE   23   // 安全模式标志
#define AT_RANDOM   25   // 16字节随机数据
#define AT_HWCAP2   26   // 硬件能力2
#define AT_EXECFN   31   // 可执行文件名
```

**栈布局**:

```
高地址
┌─────────────────────┐
│  环境变量字符串      │
├─────────────────────┤
│  参数字符串          │
├─────────────────────┤
│  辅助向量            │
├─────────────────────┤
│  NULL               │
├─────────────────────┤
│  环境变量指针数组    │
├─────────────────────┤
│  NULL               │
├─────────────────────┤
│  参数指针数组(argv)  │
├─────────────────────┤
│  argc (参数个数)     │ ← 栈顶 (rsp指向)
├─────────────────────┤
│  可用栈空间          │
│       ...           │
├─────────────────────┤
│  栈底               │
└─────────────────────┘
低地址
```

---

## 3. 内存映射详解

### 3.1 ELF段映射

```c
// 程序头表项
typedef struct {
    uint32_t p_type;     // 段类型
    uint32_t p_flags;    // 权限标志
    uint64_t p_offset;   // 文件偏移
    uint64_t p_vaddr;    // 虚拟地址
    uint64_t p_paddr;    // 物理地址（未使用）
    uint64_t p_filesz;   // 文件大小
    uint64_t p_memsz;    // 内存大小
    uint64_t p_align;    // 对齐要求
} Elf64_Phdr;
```

**段类型**:

| 类型 | 值 | 说明 |
|:-----|:---|:-----|
| PT_NULL | 0 | 未使用 |
| PT_LOAD | 1 | 可加载段 |
| PT_DYNAMIC | 2 | 动态链接信息 |
| PT_INTERP | 3 | 解释器路径 |
| PT_NOTE | 4 | 辅助信息 |
| PT_SHLIB | 5 | 保留 |
| PT_PHDR | 6 | 程序头表 |
| PT_TLS | 7 | 线程本地存储 |

### 3.2 典型的内存布局

```
高地址 0x7fff_ffff_ffff
┌──────────────────────┐
│      栈 (Stack)       │ ← 向下增长
│  (参数、环境、局部变量) │
├──────────────────────┤
│       ...            │
├──────────────────────┤
│      共享库映射        │ ← mmap区域
│  (.so文件、内存映射)   │
├──────────────────────┤
│       ...            │
├──────────────────────┤
│      堆 (Heap)        │ ← 向上增长
│   (malloc分配)        │
├──────────────────────┤
│      BSS段            │
│  (未初始化全局变量)    │
├──────────────────────┤
│      数据段 (.data)    │
│  (已初始化全局变量)    │
├──────────────────────┤
│      代码段 (.text)    │
│  (程序代码、只读数据)  │
├──────────────────────┤
│      保留区域          │
└──────────────────────┘
低地址 0x400000
```

**权限映射**:

```
.text:  r-x (只读可执行)
.rodata: r-- (只读)
.data:   rw- (读写)
.bss:    rw- (读写)
.stack:  rw- (读写)
.heap:   rw- (读写)
```

### 3.3 mmap映射过程

```c
// 内核中ELF段映射
static unsigned long elf_map(struct file *filep, unsigned long addr,
                             struct elf_phdr *eppnt, int prot, int type) {
    unsigned long map_addr;

    map_addr = vm_mmap(filep, ELF_PAGESTART(addr),
                       ELF_PAGEALIGN(eppnt->p_filesz + ELF_PAGEOFFSET(eppnt->p_vaddr)),
                       prot, MAP_FIXED|MAP_PRIVATE|type,
                       eppnt->p_offset - ELF_PAGEOFFSET(eppnt->p_vaddr));

    return map_addr;
}
```

---

## 4. 动态链接器自举

### 4.1 ld.so的特殊性

动态链接器本身是一个共享库，但加载它时**不能依赖任何其他库**。

**自举问题**:

```
普通共享库加载:
    ld.so加载libfoo.so → 解析libfoo.so的重定位 → 执行

ld.so自举:
    内核加载ld.so → ??? 谁来解析ld.so的重定位?

    答案: ld.so自己解析自己!
    - 必须使用PIC代码
    - 不能使用全局变量（或只能使用相对寻址）
    - 手动初始化GOT
```

### 4.2 自举代码分析

```asm
# ld.so入口 (_start)
_start:
    # 获取初始栈指针（内核设置）
    movq %rsp, %rdi

    # 调用_dl_start，这是第一个C函数
    # 它返回用户程序的入口点
    call _dl_start

    # 跳转到用户程序入口
    jmp *%rax
```

```c
// _dl_start: 自举入口
ElfW(Addr) _dl_start(void *arg) {
    // 1. 找到动态段(.dynamic)
    // 通过&_DYNAMIC符号，它在GOT中

    // 2. 自举重定位
    // 解析ld.so自身的重定位项
    // 不能使用外部函数，只能使用本地代码

    // 3. 初始化GOT
    // 设置GOT[1]和GOT[2]

    // 4. 进入主入口
    return _dl_start_final(arg);
}
```

### 4.3 动态链接器主循环

```c
void dl_main(const struct auxv *av, ElfW(Addr) user_entry) {
    // 1. 从辅助向量获取信息
    ElfW(Addr) program_base = av[AT_BASE].a_val;
    ElfW(Phdr) *phdr = av[AT_PHDR].a_ptr;

    // 2. 解析可执行文件的依赖
    struct link_map *main_map = map_object("./program", program_base);

    // 3. 广度优先加载依赖库
    load_dependencies(main_map);

    // 4. 重定位
    for (each loaded object) {
        relocate_object(obj);
    }

    // 5. 执行初始化
    for (each loaded object in reverse order) {
        run_initializers(obj);
    }

    // 6. 跳转到程序入口
    start_thread(user_entry, ...);
}
```

---

## 5. ASLR与安全加载

### 5.1 地址空间布局随机化

**ASLR的作用**: 随机化内存布局，防止攻击者预测地址。

```
无ASLR (固定地址):
    .text: 0x400000
    stack: 0x7fff_ffff_xxxx

有ASLR (随机地址):
    .text: 0x55xx_xxxx_xxxx
    stack: 0x7ffc_xxxx_xxxx
    mmap:  0x7fxx_xxxx_xxxx
```

**Linux实现**:

```bash
# 查看ASLR设置
cat /proc/sys/kernel/randomize_va_space
# 0 = 关闭
# 1 = 保守（栈、库随机）
# 2 = 完全（堆、可执行文件也随机）

# 临时禁用ASLR（调试时）
echo 0 | sudo tee /proc/sys/kernel/randomize_va_space
setarch $(uname -m) -R ./program  # 单程序禁用

# 查看具体随机化范围
cat /proc/self/maps | head -20
```

### 5.2 PIE (Position Independent Executable)

PIE使可执行文件可以加载到任意地址，支持ASLR。

```bash
# 编译PIE程序
gcc -fPIE -pie -o program program.c

# 检查是否为PIE
readelf -h program | grep Type
# Type: DYN (Shared object file)  ← PIE
# Type: EXEC (Executable file)     ← 非PIE

# 检查ASLR效果
ldd ./program
./program &
cat /proc/$!/maps | grep program
```

### 5.3 安全机制总结

| 机制 | 作用 | 配置 |
|:-----|:-----|:-----|
| **ASLR** | 地址随机化 | /proc/sys/kernel/randomize_va_space |
| **PIE** | 可执行文件PIC | gcc -fPIE -pie |
| **NX** | 栈不可执行 | 默认启用 |
| **RELRO** | GOT保护 | gcc -Wl,-z,relro,-z,now |
| **Stack Canary** | 栈溢出检测 | gcc -fstack-protector |

---

## 6. 调试与分析

### 6.1 查看进程内存布局

```bash
# 查看进程内存映射
cat /proc/$$/maps

# 示例输出:
# 55c3d8a2c000-55c3d8a2d000 r--p 00000000 08:01 1310734   /bin/bash
# 55c3d8a2d000-55c3d8b1c000 r-xp 00001000 08:01 1310734   /bin/bash
# 55c3d8b1c000-55c3d8b6c000 r--p 000f0000 08:01 1310734   /bin/bash
# 55c3d8b6c000-55c3d8b70000 r--p 0013f000 08:01 1310734   /bin/bash
# 55c3d8b70000-55c3d8b79000 rw-p 00143000 08:01 1310734   /bin/bash
# ...
# 7ffc3d7ff000-7ffc3d820000 rw-p 00000000 00:00 0        [stack]
# 7ffc3d9fe000-7ffc3da00000 r--p 00000000 00:00 0        [vvar]
# 7ffc3da00000-7ffc3da01000 r-xp 00000000 00:00 0        [vdso]

# 查看特定地址信息
cat /proc/$$/smaps | less

# 使用pmap
pmap $$
```

### 6.2 strace跟踪加载过程

```bash
# 跟踪execve和内存映射
strace -e execve,mmap,mprotect ./program 2>&1

# 跟踪文件打开（库加载）
strace -e openat,open ./program 2>&1 | grep \\.so

# 完整跟踪加载过程
strace -f -o trace.log ./program
grep -E "(execve|mmap|open)" trace.log | head -100
```

### 6.3 GDB调试加载过程

```bash
# 在动态链接器入口设置断点
gdb ./program
(gdb) set disable-randomization off  # 保持ASLR
(gdb) break _start
(gdb) run

# 查看加载的共享库
(gdb) info sharedlibrary

# 查看动态段
(gdb) x/20gx &_DYNAMIC

# 在动态链接器函数设置断点
(gdb) break dlopen
(gdb) break dlsym
```

---

## 7. 自定义加载器

### 7.1 简单的ELF加载器

```c
// mini_loader.c - 简化版ELF加载器
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <elf.h>
#include <sys/auxv.h>

int load_elf(const char *path, char **argv, char **envp) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    // 读取ELF头
    Elf64_Ehdr ehdr;
    read(fd, &ehdr, sizeof(ehdr));

    // 验证ELF
    if (memcmp(ehdr.e_ident, ELFMAG, SELFMAG) != 0) {
        fprintf(stderr, "Not an ELF file\n");
        return 1;
    }

    // 读取程序头表
    Elf64_Phdr *phdr = malloc(ehdr.e_phnum * sizeof(Elf64_Phdr));
    lseek(fd, ehdr.e_phoff, SEEK_SET);
    read(fd, phdr, ehdr.e_phnum * sizeof(Elf64_Phdr));

    // 映射PT_LOAD段
    for (int i = 0; i < ehdr.e_phnum; i++) {
        if (phdr[i].p_type == PT_LOAD) {
            int prot = 0;
            if (phdr[i].p_flags & PF_R) prot |= PROT_READ;
            if (phdr[i].p_flags & PF_W) prot |= PROT_WRITE;
            if (phdr[i].p_flags & PF_X) prot |= PROT_EXEC;

            void *addr = mmap(
                (void *)phdr[i].p_vaddr,
                phdr[i].p_memsz,
                prot,
                MAP_FIXED | MAP_PRIVATE,
                fd,
                phdr[i].p_offset
            );

            if (addr == MAP_FAILED) {
                perror("mmap");
                return 1;
            }

            // 清零BSS
            if (phdr[i].p_memsz > phdr[i].p_filesz) {
                memset((char *)addr + phdr[i].p_filesz, 0,
                       phdr[i].p_memsz - phdr[i].p_filesz);
            }
        }
    }

    close(fd);

    // 注意：实际实现需要处理更多细节
    // 包括栈设置、动态链接、参数传递等

    printf("ELF loaded at entry point: %p\n", (void *)ehdr.e_entry);

    // 这里应该跳转到入口点执行
    // 但完整实现需要汇编代码设置栈和跳转

    return 0;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <elf-file>\n", argv[0]);
        return 1;
    }

    return load_elf(argv[1], argv + 1, environ);
}
```

---

## 关联导航

### 前置知识

- [动态链接深度解析](./04_Dynamic_Linking_Deep.md)
- [系统调用](../../03_System_Technology_Domains/01_System_Programming/04_System_Call_Interface.md)
- [虚拟内存](../../03_System_Technology_Domains/01_System_Programming/05_Memory_Mapped_Files.md)

### 后续延伸

- [Linux内核](../../04_Industrial_Scenarios/13_Linux_Kernel/README.md)
- [内存管理](../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md)
- [安全机制](../../01_Core_Knowledge_System/09_Safety_Standards/04_Secure_Coding_Guide.md)

### 实践关联

- [逆向工程](../../03_System_Technology_Domains/01_System_Programming/08_Binary_Reverse_Engineering.md)
- [漏洞利用](../../01_Core_Knowledge_System/09_Safety_Standards/Vulnerability_Cases/README.md)
