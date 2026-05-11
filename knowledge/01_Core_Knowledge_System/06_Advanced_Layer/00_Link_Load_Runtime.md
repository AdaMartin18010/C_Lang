# 链接、加载与运行时数据结构

> **标准**: System V ABI, ELF Format, DWARF Debugging Format  
> **前置知识**: [抽象状态机](../01_Basic_Layer/00_Abstract_State_Machine.md), [内存管理](../02_Core_Layer/02_Memory_Management.md)  
> **后续延伸**: [C到汇编映射](../../02_Formal_Semantics_and_Physics/06_C_Assembly_Mapping/README.md), [链接器脚本](../../02_Formal_Semantics_and_Physics/08_Linking_Loading_Topology/05_Linker_Scripts_Guide.md)  
> **对标资源**: Expert C Programming (Ch.5, Ch.6), Stanford CS107, CMU 15-213  
> **难度**: 4/5 | **预计学习时间**: 4-5 小时  

---

## 目录

1. [程序生命周期概览](#一程序生命周期)
2. [编译流程回顾](#二编译流程)
3. [ELF格式详解](#三elf格式)
4. [链接器工作原理](#四链接器)
5. [加载器与程序启动](#五加载器)
6. [运行时数据结构](#六运行时结构)
7. [调用约定与栈帧](#七调用约定)
8. [位置无关代码（PIC）](#八pic)
9. [动态链接](#九动态链接)
10. [调试信息（DWARF）](#十调试信息)

---

## 一、程序生命周期

```
C源代码
    |
    v
[编译器] 预处理 -> 编译 -> 汇编
    |
    v
目标文件 (.o/.obj)
    |
    v
[链接器] 符号解析 -> 重定位 -> 合并段
    |
    v
可执行文件 / 共享库
    |
    v
[加载器] 创建进程 -> 映射段 -> 重定位（动态）
    |
    v
程序运行 (main开始)
    |
    v
[运行时] 栈帧创建/销毁 -> 堆分配/释放 -> 动态链接
    |
    v
程序终止
```

---

## 二、编译流程

### 2.1 四阶段编译

```bash
# 完整流程
gcc -E hello.c -o hello.i      # 预处理：宏展开、头文件包含
gcc -S hello.i -o hello.s      # 编译：C -> 汇编
gcc -c hello.s -o hello.o      # 汇编：汇编 -> 目标文件
gcc hello.o -o hello           # 链接：目标文件 -> 可执行文件

# 一步完成
gcc hello.c -o hello
```

### 2.2 目标文件内容

目标文件（.o）包含：

| 段（Section） | 内容 |
|:-------------|:-----|
| .text | 机器码（代码段） |
| .data | 已初始化的全局/静态变量 |
| .bss | 未初始化的全局/静态变量（仅大小信息） |
| .rodata | 只读数据（字符串常量、const变量） |
| .symtab | 符号表（函数名、变量名、地址） |
| .rel.* | 重定位表（需要链接器修补的位置） |

```bash
# 查看目标文件段
objdump -h hello.o
readelf -S hello.o

# 查看符号表
nm hello.o
readelf -s hello.o
```

---

## 三、ELF格式

### 3.1 ELF文件结构

ELF（Executable and Linkable Format）是Linux/Unix的标准可执行文件格式。

```
ELF文件
├── ELF Header（文件元数据）
├── Program Header Table（运行时段信息）
├── Sections（数据段）
│   ├── .text      代码
│   ├── .data      已初始化数据
│   ├── .bss       未初始化数据
│   ├── .rodata    只读数据
│   ├── .symtab    符号表
│   ├── .strtab    字符串表
│   ├── .rel.text  代码重定位表
│   ├── .rel.data  数据重定位表
│   └── ...
└── Section Header Table（段描述信息）
```

### 3.2 ELF Header关键字段

```c
// /usr/include/elf.h 中的定义（简化）
typedef struct {
    unsigned char e_ident[16];  // 魔数、字长、字节序等
    uint16_t      e_type;        // 文件类型（可重定位/可执行/共享库）
    uint16_t      e_machine;     // 目标架构（x86-64=62, ARM=40, RISC-V=243）
    uint32_t      e_version;     // 版本
    ElfN_Addr     e_entry;       // 程序入口点地址
    ElfN_Off      e_phoff;       // Program Header Table偏移
    ElfN_Off      e_shoff;       // Section Header Table偏移
    uint32_t      e_flags;       // 处理器特定标志
    uint16_t      e_ehsize;      // ELF Header大小
    uint16_t      e_phentsize;   // Program Header条目大小
    uint16_t      e_phnum;       // Program Header条目数
    uint16_t      e_shentsize;   // Section Header条目大小
    uint16_t      e_shnum;       // Section Header条目数
    uint16_t      e_shstrndx;    // 段名字符串表索引
} ElfN_Ehdr;
```

### 3.3 查看ELF信息

```bash
# 文件类型
file ./hello
# hello: ELF 64-bit LSB executable, x86-64, version 1 (SYSV), dynamically linked

# ELF头
readelf -h ./hello

# 程序头（运行时段）
readelf -l ./hello

# 段表
readelf -S ./hello

# 动态段
readelf -d ./hello

# 符号表
readelf -s ./hello
```

---

## 四、链接器

### 4.1 链接器的两个任务

1. **符号解析（Symbol Resolution）**
   - 将符号引用与符号定义关联
   - 处理全局变量和函数的引用

2. **重定位（Relocation）**
   - 为符号分配最终运行时地址
   - 修改代码和数据中的引用，指向正确地址

### 4.2 符号解析规则

```c
// main.c
extern int global_var;      // 引用（未定义）
void foo();                 // 引用（未定义）

int main() {
    foo();
    return global_var;
}

// utils.c
int global_var = 42;        // 定义（强符号）
void foo() {}               // 定义（强符号）
```

**符号解析规则**：
1. 不允许多个强符号（函数和已初始化全局变量）同名
2. 一个强符号 + 多个弱符号（未初始化全局变量）：选择强符号
3. 多个弱符号：任选其一（通常是第一个遇到的）

### 4.3 重定位类型

| 类型 | 说明 | 示例 |
|:-----|:-----|:-----|
| R_X86_64_32 | 绝对地址（32位） | 静态数据引用 |
| R_X86_64_PC32 | PC相对地址（32位） | 函数调用 |
| R_X86_64_PLT32 | PLT条目相对地址 | 外部函数调用 |
| R_X86_64_GOTPCREL | GOT条目相对地址 | 全局变量（PIC） |

```bash
# 查看重定位表
objdump -r hello.o
readelf -r hello.o
```

### 4.4 链接脚本（Linker Script）

链接器脚本控制段的布局和地址分配：

```ld
/* simple.ld */
ENTRY(_start)           /* 入口点 */

SECTIONS {
    . = 0x400000;       /* 代码段起始地址 */
    
    .text : {
        *(.text)        /* 合并所有输入文件的.text段 */
    }
    
    . = ALIGN(0x1000);  /* 页对齐 */
    .data : {
        *(.data)
    }
    
    .bss : {
        *(.bss)
        *(COMMON)       /* 未初始化全局变量 */
    }
}
```

```bash
# 使用自定义链接脚本
gcc -T simple.ld main.c -o hello
```

---

## 五、加载器

### 5.1 程序启动流程

```
命令行: ./hello arg1 arg2
    |
    v
shell调用execve()
    |
    v
操作系统内核:
    1. 创建进程（fork/clone）
    2. 分配虚拟地址空间
    3. 读取ELF文件
    4. 映射段到内存:
       - .text -> 代码段（只读、可执行）
       - .data -> 数据段（读写）
       - .bss  -> 零初始化页
    5. 设置栈
    6. 加载动态链接器（若需要）
    7. 跳转到入口点（_start）
    |
    v
_start (crt1.o):
    1. 设置环境
    2. 初始化C运行时（全局构造器等）
    3. 调用 __libc_start_main
    4. __libc_start_main调用main()
    |
    v
main(argc, argv, envp)
```

### 5.2 进程内存布局

```
高地址
+------------------+
|  内核空间        |  (用户不可访问)
+------------------+
|  栈（向下增长）  |  局部变量、函数参数、返回地址
|        |         |  
|        v         |
|                  |
|        ^         |
|        |         |
|  堆（向上增长）  |  malloc分配区域
+------------------+
|  BSS段           |  未初始化全局变量
+------------------+
|  数据段          |  已初始化全局变量
+------------------+
|  代码段          |  机器码、只读数据
+------------------+
低地址
```

```bash
# 查看进程内存映射
cat /proc/self/maps

# 查看程序段地址
size ./hello
```

---

## 六、运行时数据结构

### 6.1 栈帧（Stack Frame）

每次函数调用时，都会在栈上创建一个栈帧：

```
高地址
+------------------+
|  参数N           |  调用者压入
|  ...             |
|  参数1           |
|  返回地址        |  call指令压入
+------------------+  <- 被调用者栈帧开始
|  保存的旧RBP     |  push rbp
+------------------+  <- RBP指向此处
|  局部变量1       |
|  局部变量2       |
|  ...             |
|  保存的寄存器    |
+------------------+  <- RSP指向此处
低地址
```

**x86-64 System V ABI栈帧**：

```asm
; 函数序言（Prologue）
push    rbp          ; 保存旧基址指针
mov     rbp, rsp     ; 设置新基址指针
sub     rsp, 32      ; 为局部变量分配空间（对齐到16字节）

; 函数体
...

; 函数尾声（Epilogue）
mov     rsp, rbp     ; 恢复栈指针
pop     rbp          ; 恢复基址指针
ret                  ; 返回（弹出返回地址并跳转）
```

### 6.2 激活记录（Activation Record）

激活记录是栈帧的另一种称呼，包含：

| 内容 | 说明 |
|:-----|:-----|
| 返回地址 | 调用者中下一条指令的地址 |
| 保存的寄存器 | 被调用者需要保存的寄存器（RBX, R12-R15等）|
| 局部变量 | 函数内定义的自动变量 |
| 临时空间 | 编译器生成的临时变量 |

### 6.3 setjmp/longjmp的实现

```c
#include <setjmp.h>

jmp_buf env;

void func() {
    longjmp(env, 1);  // "跳转"回setjmp位置
}

int main() {
    if (setjmp(env) == 0) {
        // 首次执行
        func();
    } else {
        // 从longjmp返回
    }
}
```

**实现原理**：
- `setjmp(env)`：保存当前寄存器状态（RSP, RBP, PC等）到env
- `longjmp(env, val)`：从env恢复寄存器状态，返回val

```asm
; setjmp伪代码
setjmp:
    mov [env + 0],  rbp     ; 保存RBP
    mov [env + 8],  rsp     ; 保存RSP
    mov [env + 16], rcx     ; 保存返回地址（调用者PC）
    xor eax, eax            ; 返回0
    ret

; longjmp伪代码
longjmp:
    mov rbp, [env + 0]      ; 恢复RBP
    mov rsp, [env + 8]      ; 恢复RSP
    mov eax, edi            ; 返回值 = val
    mov rcx, [env + 16]     ; 恢复PC
    jmp rcx                 ; 跳转（而非call）
```

---

## 七、调用约定

### 7.1 x86-64 System V ABI（Linux/macOS/Unix）

**参数传递**：
- 整数/指针参数：RDI, RSI, RDX, RCX, R8, R9
- 浮点参数：XMM0-XMM7
- 额外参数：通过栈传递

**返回值**：
- 整数/指针：RAX
- 浮点：XMM0

**调用者保存（Caller-saved）**：RAX, RCX, RDX, RSI, RDI, R8-R11
**被调用者保存（Callee-saved）**：RBX, RBP, R12-R15

```c
// C代码
int add(int a, int b) {
    return a + b;
}

int main() {
    int r = add(10, 20);
    return r;
}
```

```asm
; 汇编（x86-64 System V ABI）
add:
    lea     eax, [rdi + rsi]   ; a在rdi, b在rsi，结果返回到rax
    ret

main:
    mov     edi, 10            ; 第一个参数
    mov     esi, 20            ; 第二个参数
    call    add                ; 调用
    ret                        ; 返回值已在rax
```

### 7.2 Windows x64调用约定

与System V ABI不同：
- 参数传递：RCX, RDX, R8, R9（前四个整数参数）
- 调用者必须为所有参数（包括寄存器传递的）分配32字节"影子空间"

---

## 八、位置无关代码（PIC）

### 8.1 为什么需要PIC

共享库（.so/.dll）被加载到不同进程的**不同地址**，因此不能包含绝对地址。

### 8.2 PIC的工作原理

**GOT（Global Offset Table）**：
- 数据段中的一个数组
- 每个条目存储全局变量/函数的实际地址
- 代码通过GOT间接访问全局数据

**PLT（Procedure Linkage Table）**：
- 代码段中的跳转表
- 用于延迟绑定外部函数

```
代码段（PIC）
    |
    v
访问global_var:
    mov rax, [rip + GOT_offset]   ; 获取GOT条目地址
    mov eax, [rax]                 ; 通过GOT读取实际值
    
调用外部函数:
    call [rip + PLT_offset]       ; 跳转到PLT条目
    PLT -> 首次调用 -> 解析地址 -> 更新GOT -> 跳转
         -> 后续调用 -> 直接通过GOT跳转
```

```bash
# 编译位置无关代码
gcc -fPIC -c lib.c -o lib.o
gcc -shared lib.o -o lib.so

# 查看GOT/PLT
objdump -R ./program
readelf -r ./program
```

---

## 九、动态链接

### 9.1 动态链接流程

```
程序启动
    |
    v
加载器发现需要动态链接器（/lib64/ld-linux-x86-64.so.2）
    |
    v
动态链接器（ld.so）接管:
    1. 加载所需的共享库
    2. 符号解析（可能延迟）
    3. 重定位（GOT/PLT修补）
    4. 执行初始化代码
    5. 跳转到程序入口
```

### 9.2 延迟绑定（Lazy Binding）

外部函数的地址只在**首次调用时**解析：

```
首次调用 printf:
    call printf@PLT
    PLT -> 跳转到解析桩 -> 调用动态链接器
         -> 解析printf地址 -> 更新GOT[printf]
         -> 跳转到printf

后续调用 printf:
    call printf@PLT
    PLT -> GOT[printf]已包含实际地址 -> 直接跳转
```

```bash
# 查看动态依赖
ldd ./hello

# 查看动态符号
readelf --dyn-syms ./hello

# 禁用延迟绑定（用于调试）
LD_BIND_NOW=1 ./hello
```

### 9.3 dlsym与运行时动态加载

```c
#include <dlfcn.h>

void* handle = dlopen("./libmath.so", RTLD_LAZY);
if (!handle) { /* error */ }

int (*add)(int, int) = dlsym(handle, "add");
if (!add) { /* error */ }

int r = add(10, 20);
dlclose(handle);
```

---

## 十、调试信息

### 10.1 DWARF格式

DWARF是ELF文件中存储调试信息的标准格式：

| 段 | 内容 |
|:---|:-----|
| .debug_info | 编译单元信息（函数、变量、类型） |
| .debug_abbrev | 属性缩写表 |
| .debug_line | 行号映射（地址 <-> 源代码行） |
| .debug_str | 调试字符串表 |
| .debug_frame | 调用帧信息（栈展开） |

```bash
# 查看DWARF信息
readelf --debug-dump=info ./hello
readelf --debug-dump=line ./hello

# 使用dwarfdump
dwarfdump ./hello
```

### 10.2 剥离调试信息

```bash
# 编译时包含调试信息
gcc -g hello.c -o hello

# 剥离调试信息（发布版本）
strip ./hello
# 或
objcopy --strip-debug ./hello ./hello_stripped
```

---

## 参考资源

1. **Expert C Programming** (Peter van der Linden) - Chapter 5: Thinking in Linkages; Chapter 6: Poetry in Motion - Runtime Data Structures
2. **Linkers and Loaders** (John R. Levine) - 链接器权威参考书
3. **Stanford CS107** - Lecture: Compilation, Linking, Loading
4. **CMU 15-213** - Chapter 7: Linking
5. **System V AMD64 ABI** - 调用约定标准
6. **ELF Format Specification** - TIS Committee
7. **DWARF Debugging Information Format** - dwarfstd.org

---

> 核心总结：从源代码到运行中的进程，C程序经历了编译、汇编、链接、加载四个阶段。理解ELF格式、链接器的符号解析和重定位、加载器的内存映射、以及运行时的栈帧结构，是调试复杂问题（崩溃、链接错误、动态库问题）的基础。

---

*文档状态: 初版完成 | 对齐: Expert C Programming Ch.5,6, CS107, CMU 15-213 | 最后更新: 2026-05-11*
