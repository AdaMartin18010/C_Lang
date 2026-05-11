# x86-64 架构系统编程指南

> **目标读者**: 已掌握 C 语言，希望深入理解底层执行模型的开发者
> **对比基准**: 本指南与项目现有 RISC-V 内容形成互补
> **参考规范**: Intel SDM Vol. 1-3, AMD64 APM, System V AMD64 ABI

---

## 1. x86-64 vs RISC-V：设计哲学差异

### 1.1 架构对比

| 特性 | x86-64 (CISC) | RISC-V (RISC) | 对 C 程序员的影响 |
|:-----|:--------------|:--------------|:------------------|
| **指令长度** | 变长 (1-15 字节) | 定长 (4 字节) | x86 解码复杂，影响性能预测 |
| **寄存器** | 16 通用 (GPR) | 32 通用 | x86 寄存器压力更大 |
| **历史包袱** | x86, x87, MMX, SSE, AVX... | 简洁模块化扩展 | x86 需要理解多种执行模式 |
| **内存模型** | TSO (强一致性) | 宽松一致性 + fence | x86 并发编程更直觉 |
| **市场份额** | ~95% 服务器/桌面 | 嵌入式/教育/实验 | 工业场景几乎必须掌握 x86-64 |

### 1.2 为什么 C 程序员必须理解 x86-64

```
┌────────────────────────────────────────────────────────────┐
│                    C 代码 → 可执行文件                       │
├────────────────────────────────────────────────────────────┤
│  1. 编译器优化: 理解 x86 特性才能预测优化行为                   │
│  2. 调试: 80% 的调试需要阅读汇编 (gdb disas)                 │
│  3. 性能: perf 事件、cache miss、分支预测与架构强相关          │
│  4. 安全: 缓冲区溢出、ROP、Spectre/Meltdown 是 x86 特性       │
│  5. 系统调用: ABI 约定直接影响 C 函数调用约定                  │
└────────────────────────────────────────────────────────────┘
```

---

## 2. x86-64 寄存器架构

### 2.1 通用寄存器 (GPR)

```
┌─────────────────────────────────────────────────────────────┐
│                    x86-64 通用寄存器                          │
├─────────────────────────────────────────────────────────────┤
│  64-bit    │  32-bit   │  16-bit  │  8-bit   │ System V ABI │
├─────────────────────────────────────────────────────────────┤
│   %rax     │   %eax    │   %ax    │  %al/%ah │ 返回值        │
│   %rbx     │   %ebx    │   %bx    │  %bl/%bh │ 被调用者保存   │
│   %rcx     │   %ecx    │   %cx    │  %cl/%ch │ 第 4 参数     │
│   %rdx     │   %edx    │   %dx    │  %dl/%dh │ 第 3 参数     │
│   %rsi     │   %esi    │   %si    │  %sil    │ 第 2 参数     │
│   %rdi     │   %edi    │   %di    │  %dil    │ 第 1 参数     │
│   %rbp     │   %ebp    │   %bp    │  %bpl    │ 帧指针 (可选)  │
│   %rsp     │   %esp    │   %sp    │  %spl    │ 栈指针        │
│   %r8      │   %r8d    │   %r8w   │  %r8b    │ 第 5 参数     │
│   %r9      │   %r9d    │   %r9w   │  %r9b    │ 第 6 参数     │
│   %r10-%r11│           │          │          │ 调用者保存     │
│   %r12-%r15│           │          │          │ 被调用者保存   │
└─────────────────────────────────────────────────────────────┘
```

### 2.2 C 代码与寄存器映射

```c
// C 代码
int64_t add(int64_t a, int64_t b, int64_t c, 
            int64_t d, int64_t e, int64_t f) {
    return a + b + c + d + e + f;
}

// System V AMD64 ABI 寄存器分配:
// a → %rdi, b → %rsi, c → %rdx, d → %rcx, e → %r8, f → %r9
// 返回值 → %rax
```

```asm
; 生成的汇编 (AT&T 语法)
add:
    leaq    (%rdi,%rsi), %rax    ; rax = a + b
    addq    %rdx, %rax           ; rax += c
    addq    %rcx, %rax           ; rax += d
    addq    %r8, %rax            ; rax += e
    addq    %r9, %rax            ; rax += f
    ret
```

---

## 3. System V AMD64 ABI：C 函数调用约定

### 3.1 参数传递规则

```
整数/指针参数: %rdi, %rsi, %rdx, %rcx, %r8, %r9
浮点参数:      %xmm0-%xmm7
超出寄存器:    压入栈 (右到左)
返回值整数:    %rax (+ %rdx for 128-bit)
返回值浮点:    %xmm0
栈对齐要求:    16 字节边界
```

### 3.2 栈帧布局

```
高地址
┌─────────────────────┐ ← %rbp (旧帧指针)
│  返回地址            │    8 bytes
├─────────────────────┤ ← %rsp (进入函数时)
│  被调用者保存寄存器   │    %rbx, %r12-%r15 (按需)
├─────────────────────┤
│  局部变量            │
├─────────────────────┤
│  对齐填充            │    保证 16 字节对齐
├─────────────────────┤ ← %rsp (函数执行中)
│  参数构建区          │    为被调函数准备参数
├─────────────────────┤
│  被调函数参数        │    第 7+ 参数
└─────────────────────┘
低地址
```

### 3.3 C 代码的栈帧示例

```c
// C 代码
void caller(void) {
    int x = 10;
    int y = 20;
    int z = callee(x, y);
    (void)z;
}

int callee(int a, int b) {
    int local = a + b;
    return local * 2;
}
```

```asm
; caller 的汇编
caller:
    pushq   %rbp
    movq    %rsp, %rbp
    
    ; 分配局部变量空间 (16 字节对齐)
    subq    $16, %rsp
    
    ; x = 10, y = 20
    movl    $10, -4(%rbp)    ; x at rbp-4
    movl    $20, -8(%rbp)    ; y at rbp-8
    
    ; 准备参数 (整数前 6 个用寄存器)
    movl    -4(%rbp), %edi   ; a = x
    movl    -8(%rbp), %esi   ; b = y
    
    call    callee
    
    ; 返回值在 %eax
    movl    %eax, -12(%rbp)  ; z = result
    
    leave                     ; movq %rbp, %rsp; popq %rbp
    ret
```

---

## 4. 内存模型与缓存层次

### 4.1 x86-64 缓存层次

```
CPU 核心
┌────────────────────────────────────────┐
│  L1 指令缓存: 32-64 KB                 │
│  L1 数据缓存: 32-64 KB                 │
├────────────────────────────────────────┤
│  L2 缓存: 256-512 KB (每核心)          │
├────────────────────────────────────────┤
│  L3 缓存: 8-64 MB (共享)               │
└────────────────────────────────────────┘
           │
           ▼
        主内存 (DDR4/DDR5)
```

### 4.2 缓存行与伪共享

```c
// ❌ 伪共享 (False Sharing): 两个线程频繁修改同一缓存行
struct Counter {
    int64_t count_a;  // 线程 A 修改
    int64_t count_b;  // 线程 B 修改
    // 两者在同一 64 字节缓存行 → 缓存行在核心间弹跳
};

// ✅ 缓存行对齐避免伪共享
#define CACHE_LINE_SIZE 64

struct alignas(CACHE_LINE_SIZE) PaddedCounter {
    int64_t count;
    char padding[CACHE_LINE_SIZE - sizeof(int64_t)];
};

// 或 C23 方式
struct alignas(64) PaddedCounter {
    int64_t count;
};
```

### 4.3 TSO 内存模型 vs C11 内存序

x86-64 实现**全存储排序 (TSO)**，天然保证：
- Store → Store 有序
- Load → Load 有序  
- Load → Store 有序
- **Store → Load 可能重排序**（这是唯一允许的重排序）

```c
// x86-64 上，以下代码实际是有序的（不需要 barrier）
// 因为 TSO 禁止 Load-Load 重排序

int data = 0;
atomic_int flag = 0;

// Thread 1
void write_data(void) {
    data = 42;                    // Store
    atomic_store_explicit(&flag, 1, memory_order_release);
                                  // → 在 x86 上实际发出 mfence 或 lock
}

// Thread 2
void read_data(void) {
    while (atomic_load_explicit(&flag, memory_order_acquire) != 1)
        ;
    // data 一定是 42（在 x86 上）
    int x = data;                 // Load
}
```

> **重要**: 虽然 x86-64 的 TSO 让并发代码"看起来"工作正常，但 C 代码中**仍必须使用正确的 `memory_order`**。编译器优化可能重排指令，不考虑硬件内存模型。

---

## 5. 系统调用机制

### 5.1 Linux x86-64 系统调用 ABI

```
系统调用号: %rax
参数 1:     %rdi
参数 2:     %rsi
参数 3:     %rdx
参数 4:     %r10
参数 5:     %r8
参数 6:     %r9

指令: syscall (快速系统调用)
返回值: %rax (负数为错误码)
```

### 5.2 C 内联汇编系统调用

```c
// 直接调用 write(1, msg, len) 而不经过 glibc
#include <unistd.h>

static inline long syscall3(long n, long a1, long a2, long a3) {
    long ret;
    __asm__ volatile (
        "syscall"
        : "=a"(ret)
        : "a"(n), "D"(a1), "S"(a2), "d"(a3)
        : "rcx", "r11", "memory"
    );
    return ret;
}

void direct_write(const char *msg, size_t len) {
    syscall3(__NR_write, 1, (long)msg, len);
}
```

### 5.3 vDSO 优化

x86-64 Linux 通过 **vDSO (Virtual Dynamic Shared Object)** 加速常用系统调用：

| 系统调用 | vDSO 提供 | 绕过内核？ |
|:---------|:----------|:-----------|
| `gettimeofday` | ✅ | 是（用户态读取） |
| `clock_gettime` | ✅ | 是 |
| `getcpu` | ✅ | 是 |
| `rt_sigreturn` | ✅ | 否（ trampoline） |

```c
// glibc 自动使用 vDSO，无需特殊处理
// 但理解它有助于解释为什么某些调用"不进入内核"
```

---

## 6. SIMD 与向量化

### 6.1 x86-64 SIMD 演进

```
MMX (1997)      → 64-bit 整数，8 寄存器 (mm0-mm7)
SSE (1999)      → 128-bit，16 寄存器 (xmm0-xmm15)
SSE2 (2001)     → 128-bit 整数 + 双精度浮点
AVX (2011)      → 256-bit (ymm0-ymm15)
AVX-512 (2017)  → 512-bit (zmm0-zmm31) + mask 寄存器
```

### 6.2 C 代码向量化示例

```c
#include <immintrin.h>  // AVX2

// 标量版本
void add_scalar(const float *a, const float *b, float *c, size_t n) {
    for (size_t i = 0; i < n; i++) {
        c[i] = a[i] + b[i];
    }
}

// AVX2 手动向量化 (8 floats = 256 bits)
void add_avx2(const float *a, const float *b, float *c, size_t n) {
    size_t i = 0;
    // 每次处理 8 个 float
    for (; i + 8 <= n; i += 8) {
        __m256 va = _mm256_loadu_ps(&a[i]);
        __m256 vb = _mm256_loadu_ps(&b[i]);
        __m256 vc = _mm256_add_ps(va, vb);
        _mm256_storeu_ps(&c[i], vc);
    }
    // 处理剩余元素
    for (; i < n; i++) {
        c[i] = a[i] + b[i];
    }
}
```

### 6.3 编译器自动向量化

```bash
# 让编译器自动向量化（推荐首选）
gcc -O3 -march=native -ftree-vectorize -fopt-info-vec main.c

# 检查向量化报告
# note: loop vectorized
# note: vectorized 4 loops in function
```

```c
// 帮助编译器向量化的 C 编码模式

// ✅ 连续内存访问
for (int i = 0; i < n; i++) {
    c[i] = a[i] + b[i];  // 向量化友好
}

// ❌ 不规则访问（无法向量化）
for (int i = 0; i < n; i++) {
    c[idx[i]] = a[i] + b[i];  //  gather/scatter，效率低
}

// ✅ 使用 restrict 关键字
void add_restrict(float *restrict c, 
                   const float *restrict a,
                   const float *restrict b, size_t n);
// 告诉编译器指针不重叠，可向量化
```

---

## 7. 安全：x86-64 特有攻击面

### 7.1 缓冲区溢出与栈保护

```c
// x86-64 栈上的缓冲区溢出可以覆盖返回地址
void vulnerable(void) {
    char buf[16];
    gets(buf);  // ❌ 极度危险
    // 攻击者可以覆盖返回地址 → 控制程序流
}

// 编译器保护：Stack Canary
// gcc -fstack-protector-strong
void protected(void) {
    char buf[16];
    // 函数序言插入 canary:
    // movq %fs:0x28, %rax      ; 从 TLS 读取随机 canary
    // movq %rax, -8(%rbp)       ; 存到栈上
    // ...
    // 函数尾声检查:
    // movq -8(%rbp), %rdx
    // xorq %fs:0x28, %rdx       ; 比较 canary
    // jne __stack_chk_fail      ; 不匹配则 abort
}
```

### 7.2 Spectre / Meltdown

x86-64 的**推测执行 (Speculative Execution)** 是性能关键，但也引入安全漏洞：

| 漏洞 | 机制 | 缓解 |
|:-----|:-----|:-----|
| **Meltdown** | 用户态读取内核内存 | KPTI (内核页表隔离) |
| **Spectre V1** | 边界检查绕过 | lfence, __builtin_speculation_safe_value |
| **Spectre V2** | 分支目标注入 | Retpoline, IBPB/IBRS |
| **Spectre V4** | 存储绕过读取 | SSBD (Speculative Store Bypass Disable) |

```c
// 边界检查后的推测安全访问
typedef struct { size_t len; int data[]; } FlexArray;

int safe_access(FlexArray *arr, size_t idx) {
    if (idx < arr->len) {
        // 即使推测执行绕过分支，也不泄露信息
        return arr->data[idx];
    }
    return -1;
}

// 更安全的模式（使用 speculation barrier）
#include <immintrin.h>

int spectre_safe_access(int *arr, size_t len, size_t idx) {
    if (idx < len) {
        _mm_lfence();  // 阻止推测执行越过此点
        return arr[idx];
    }
    return 0;
}
```

---

## 8. 从 RISC-V 到 x86-64 的快速对照

### 8.1 指令对照

| 操作 | RISC-V | x86-64 |
|:-----|:-------|:-------|
| 寄存器加载 | `lw t0, 0(sp)` | `movl (%rsp), %eax` |
| 立即数加 | `addi t0, t1, 42` | `leaq 42(%rsi), %rax` |
| 函数调用 | `jal ra, func` | `call func` |
| 函数返回 | `ret` | `ret` |
| 系统调用 | `ecall` | `syscall` |
| 内存屏障 | `fence` | `mfence` / `lock` |

### 8.2 ABI 对照

| 特性 | RISC-V (LP64) | x86-64 (System V) |
|:-----|:--------------|:------------------|
| 参数寄存器 | a0-a7 | rdi, rsi, rdx, rcx, r8, r9 |
| 返回值 | a0, a1 | rax, rdx |
| 被调用者保存 | s0-s11 | rbx, rbp, r12-r15 |
| 栈对齐 | 16 字节 | 16 字节 |
| 红区 | 无 | 128 字节 (rsp 下方) |

> **x86-64 红区**: 函数可以使用 %rsp 下方的 128 字节作为临时存储，**无需调整栈指针**。这简化了叶函数，但也意味着信号处理程序不能假设红区内容有效。

---

## 9. 检查清单

### x86-64 编程必知

- [ ] 掌握 16 个通用寄存器的 ABI 角色
- [ ] 理解 System V AMD64 调用约定
- [ ] 知道 `syscall` 指令和参数传递
- [ ] 理解 TSO 内存模型（与 C11 memory_order 的关系）
- [ ] 了解缓存行大小（64 字节）和对齐要求
- [ ] 知道 SSE/AVX 寄存器用于浮点和 SIMD
- [ ] 理解栈 canary、PIE、ASLR 等安全机制
- [ ] 了解 Spectre/Meltdown 缓解措施对性能的影响

---

> **最后更新**: 2026-05-11
> **参考**: Intel SDM Vol. 1-3, AMD64 Architecture Programmer's Manual, System V AMD64 ABI
