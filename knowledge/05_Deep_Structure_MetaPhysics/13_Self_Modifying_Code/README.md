# 自修改代码

## 概述

自修改代码（Self-Modifying Code, SMC）是指在运行时修改自身指令的程序。虽然 SMC 常被与恶意软件联系在一起，但它在现代计算中有许多合法且重要的应用，包括 JIT 编译、动态优化、虚拟机实现等。本文档深入探讨 SMC 的技术原理、实现方法和安全考虑。

## 目录

1. [运行时代码生成](#运行时代码生成)
2. [JIT 编译原理](#jit-编译原理)
3. [安全考虑](#安全考虑)
4. [合法用途与应用](#合法用途与应用)
5. [高级技术](#高级技术)

---

## 运行时代码生成

### 内存映射与权限管理

```c
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

// 现代操作系统通过内存保护阻止代码自修改
// 需要显式设置内存页为可写且可执行

// 分配可执行内存
void* alloc_executable_memory(size_t size) {
    // MAP_ANONYMOUS: 不映射文件
    // PROT_READ | PROT_WRITE | PROT_EXEC: 读写执行权限
    void* ptr = mmap(
        NULL,                    // 让内核选择地址
        size,
        PROT_READ | PROT_WRITE | PROT_EXEC,  // RWX 权限
        MAP_PRIVATE | MAP_ANONYMOUS,
        -1,                      // 文件描述符（未使用）
        0                        // 偏移量
    );

    if (ptr == MAP_FAILED) {
        perror("mmap");
        return NULL;
    }

    return ptr;
}

// 更安全的做法：W^X (Write XOR Execute) 原则
// 写和执行权限互斥

typedef struct {
    uint8_t* code_buffer;
    size_t size;
    int is_writable;
} JitCode;

JitCode* jit_code_create(size_t size) {
    JitCode* jit = malloc(sizeof(JitCode));
    jit->size = size;
    jit->is_writable = 1;

    // 分配时只有读写权限
    jit->code_buffer = mmap(
        NULL, size,
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS,
        -1, 0
    );

    if (jit->code_buffer == MAP_FAILED) {
        free(jit);
        return NULL;
    }

    return jit;
}

// 完成代码生成后，移除写权限，添加执行权限
int jit_code_finalize(JitCode* jit) {
    if (!jit->is_writable) return 0;  // 已经 finalized

    // mprotect: 修改内存保护
    if (mprotect(jit->code_buffer, jit->size, PROT_READ | PROT_EXEC) != 0) {
        perror("mprotect");
        return -1;
    }

    jit->is_writable = 0;
    return 0;
}

// 如果需要再次修改，必须重新添加写权限（但会失去执行权限）
int jit_code_make_writable(JitCode* jit) {
    if (jit->is_writable) return 0;

    if (mprotect(jit->code_buffer, jit->size, PROT_READ | PROT_WRITE) != 0) {
        perror("mprotect");
        return -1;
    }

    jit->is_writable = 1;
    return 0;
}

// 清理
void jit_code_free(JitCode* jit) {
    if (jit) {
        munmap(jit->code_buffer, jit->size);
        free(jit);
    }
}

// 刷新指令缓存（重要！）
// x86 有自修改代码支持，但 ARM 需要显式缓存同步
void flush_instruction_cache(void* addr, size_t len) {
    #ifdef __arm__
    // ARM: 清理数据缓存，无效化指令缓存
    __builtin___clear_cache(addr, (char*)addr + len);
    #elif defined(__x86_64__)
    // x86_64: 通常不需要显式刷新，但建议执行序列化指令
    __asm__ volatile ("sfence" ::: "memory");
    #endif
}
```

### 基础代码生成示例

```c
#include <stdint.h>
#include <stdio.h>

// x86-64 机器码生成
// 生成一个简单的函数: int f(int x) { return x * 3 + 1; }

// x86-64 编码 (System V AMD64 ABI):
// 参数在 %rdi，返回值在 %rax

// 指令编码：
// lea (%rdi,%rdi,2), %rax  ; rax = rdi + rdi*2 = rdi*3
// 编码: 48 8d 04 7f
// add $1, %rax             ; rax = rax + 1
// 编码: 48 83 c0 01
// ret
// 编码: c3

uint8_t multiply_add_3_1[] = {
    0x48, 0x8d, 0x04, 0x7f,  // lea (%rdi,%rdi,2), %rax
    0x48, 0x83, 0xc0, 0x01,  // add $1, %rax
    0xc3                     // ret
};

// 动态生成乘法函数
typedef int (*unary_int_func)(int);

unary_int_func generate_multiply_by(int n) {
    JitCode* jit = jit_code_create(4096);
    if (!jit) return NULL;

    uint8_t* p = jit->code_buffer;

    // 方法1: 使用 imul
    // imul $n, %edi, %eax  ; 3 bytes + 4 bytes immediate
    if (n >= -128 && n <= 127) {
        // 8-bit immediate
        *p++ = 0x6b;  // imul r32, r/m32, imm8
        *p++ = 0xc7;  // ModR/M: EAX, EDI
        *p++ = (int8_t)n;
    } else {
        // 32-bit immediate
        *p++ = 0x69;  // imul r32, r/m32, imm32
        *p++ = 0xc7;  // ModR/M
        *(int32_t*)p = n;
        p += 4;
    }

    // ret
    *p++ = 0xc3;

    // 更新实际大小
    jit->size = p - jit->code_buffer;

    // 保护内存
    jit_code_finalize(jit);
    flush_instruction_cache(jit->code_buffer, jit->size);

    return (unary_int_func)jit->code_buffer;
}

// 测试
void test_dynamic_codegen() {
    unary_int_func times5 = generate_multiply_by(5);
    unary_int_func times100 = generate_multiply_by(100);

    printf("7 * 5 = %d\n", times5(7));
    printf("7 * 100 = %d\n", times100(7));
}
```

### 汇编器实现

```c
// 简单的 x86-64 汇编器

typedef struct {
    uint8_t* buffer;
    size_t capacity;
    size_t size;
} Assembler;

Assembler* assembler_create(size_t capacity) {
    Assembler* a = malloc(sizeof(Assembler));
    a->buffer = malloc(capacity);
    a->capacity = capacity;
    a->size = 0;
    return a;
}

void emit_u8(Assembler* a, uint8_t b) {
    if (a->size < a->capacity) {
        a->buffer[a->size++] = b;
    }
}

void emit_u32(Assembler* a, uint32_t v) {
    emit_u8(a, v & 0xff);
    emit_u8(a, (v >> 8) & 0xff);
    emit_u8(a, (v >> 16) & 0xff);
    emit_u8(a, (v >> 24) & 0xff);
}

void emit_u64(Assembler* a, uint64_t v) {
    for (int i = 0; i < 8; i++) {
        emit_u8(a, (v >> (i * 8)) & 0xff);
    }
}

// 寄存器编码
#define RAX 0
#define RCX 1
#define RDX 2
#define RBX 3
#define RSP 4
#define RBP 5
#define RSI 6
#define RDI 7
#define R8  8
#define R9  9

// 编码 REX 前缀
void emit_rex(Assembler* a, int w, int r, int x, int b) {
    emit_u8(a, 0x40 | (w << 3) | (r << 2) | (x << 1) | b);
}

// mov reg, imm64 (仅 64 位寄存器)
void emit_mov_reg_imm64(Assembler* a, int reg, uint64_t imm) {
    emit_rex(a, 1, 0, 0, reg >= 8);
    emit_u8(a, 0xb8 + (reg & 7));
    emit_u64(a, imm);
}

// mov r/m64, r64
void emit_mov_mem_reg(Assembler* a, int base_reg, int offset, int src_reg) {
    emit_rex(a, 1, src_reg >= 8, 0, base_reg >= 8);
    emit_u8(a, 0x89);

    // ModR/M + SIB 编码
    if (offset == 0 && base_reg != RBP) {
        // [base]
        emit_u8(a, 0x00 | ((src_reg & 7) << 3) | (base_reg & 7));
    } else if (offset >= -128 && offset <= 127) {
        // disp8[base]
        emit_u8(a, 0x40 | ((src_reg & 7) << 3) | (base_reg & 7));
        emit_u8(a, (int8_t)offset);
    } else {
        // disp32[base]
        emit_u8(a, 0x80 | ((src_reg & 7) << 3) | (base_reg & 7));
        emit_u32(a, (int32_t)offset);
    }
}

// add r64, imm32
void emit_add_reg_imm32(Assembler* a, int reg, int32_t imm) {
    emit_rex(a, 1, 0, 0, reg >= 8);
    if (imm >= -128 && imm <= 127) {
        emit_u8(a, 0x83);
        emit_u8(a, 0xc0 | (reg & 7));
        emit_u8(a, (int8_t)imm);
    } else {
        emit_u8(a, 0x81);
        emit_u8(a, 0xc0 | (reg & 7));
        emit_u32(a, (uint32_t)imm);
    }
}

// ret
void emit_ret(Assembler* a) {
    emit_u8(a, 0xc3);
}

// 生成函数：void set_value(int64_t* ptr, int64_t val) { *ptr = val + 42; }
void* generate_set_value_plus_42() {
    Assembler* a = assembler_create(256);

    // System V AMD64 ABI:
    // %rdi = ptr (第一个参数)
    // %rsi = val (第二个参数)

    // mov %rsi, %rax (保存 val)
    emit_rex(a, 1, 0, 0, 0);
    emit_u8(a, 0x89);
    emit_u8(a, 0xf0);  // ModR/M for mov rsi, rax

    // add $42, %rax
    emit_add_reg_imm32(a, RAX, 42);

    // mov %rax, (%rdi)
    emit_rex(a, 1, 0, 0, 0);
    emit_u8(a, 0x89);
    emit_u8(a, 0x07);  // ModR/M for mov rax, [rdi]

    // ret
    emit_ret(a);

    // 复制到可执行内存
    JitCode* jit = jit_code_create(a->size);
    memcpy(jit->code_buffer, a->buffer, a->size);
    jit_code_finalize(jit);
    flush_instruction_cache(jit->code_buffer, jit->size);

    free(a->buffer);
    free(a);

    return jit->code_buffer;
}
```

---

## JIT 编译原理

### 简单 JIT 编译器框架

```c
// 基于栈的虚拟机 JIT 编译器示例
// 将字节码编译为 x86-64 机器码

typedef enum {
    OP_PUSH,    // push immediate
    OP_ADD,     // pop two, push sum
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_LOAD,    // load from local variable
    OP_STORE,   // store to local variable
    OP_CALL,    // call function
    OP_RET,     // return
} OpCode;

typedef struct {
    OpCode opcode;
    int operand;
} Instruction;

typedef struct {
    Instruction* code;
    int code_size;
    int stack_depth;
    int max_stack;
} BytecodeFunction;

// JIT 编译状态
typedef struct {
    Assembler* asm;
    int stack_offset;  // 当前栈在临时内存中的偏移
    int locals_offset; // 局部变量在栈帧中的偏移
    void** const_pool; // 常量池（用于函数指针等）
} JitState;

// 将字节码编译为机器码
void* jit_compile(BytecodeFunction* func) {
    JitCode* jit = jit_code_create(4096);
    JitState state = {
        .asm = assembler_create(4096),
        .stack_offset = 0,
        .locals_offset = -8,  // 从 rbp-8 开始
        .const_pool = malloc(256 * sizeof(void*))
    };

    // 函数序言
    emit_push_rbp(&state);
    emit_mov_rbp_rsp(&state);
    emit_sub_rsp(&state, func->max_stack * 8);  // 分配栈空间

    for (int i = 0; i < func->code_size; i++) {
        Instruction* inst = &func->code[i];

        switch (inst->opcode) {
            case OP_PUSH:
                // push imm32
                emit_push_imm32(&state, inst->operand);
                state.stack_offset += 8;
                break;

            case OP_ADD: {
                // pop %rax
                emit_pop_rax(&state);
                // pop %rbx
                emit_pop_rbx(&state);
                // add %rbx, %rax
                emit_add_rax_rbx(&state);
                // push %rax
                emit_push_rax(&state);
                state.stack_offset -= 8;  // net -8 (pop 2, push 1)
                break;
            }

            case OP_LOAD: {
                // mov -offset(%rbp), %rax
                int var_offset = state.locals_offset - inst->operand * 8;
                emit_mov_rax_mem_rbp(&state, var_offset);
                emit_push_rax(&state);
                state.stack_offset += 8;
                break;
            }

            case OP_STORE: {
                // pop %rax
                emit_pop_rax(&state);
                // mov %rax, -offset(%rbp)
                int var_offset = state.locals_offset - inst->operand * 8;
                emit_mov_mem_rbp_rax(&state, var_offset);
                state.stack_offset -= 8;
                break;
            }

            case OP_CALL: {
                // 准备参数（从栈中弹出）
                int num_args = inst->operand;
                // System V AMD64: rdi, rsi, rdx, rcx, r8, r9
                const int arg_regs[] = {RDI, RSI, RDX, RCX, R8, R9};

                for (int j = num_args - 1; j >= 0; j--) {
                    emit_pop_reg(&state, arg_regs[j]);
                }

                // call function
                emit_call(&state, state.const_pool[inst->operand]);

                // 推送返回值
                emit_push_rax(&state);
                state.stack_offset += 8 - num_args * 8;
                break;
            }

            case OP_RET:
                // mov %rbp, %rsp
                emit_mov_rsp_rbp(&state);
                // pop %rbp
                emit_pop_rbp(&state);
                // ret
                emit_ret(&state);
                break;
        }
    }

    // 如果没有显式 ret，添加默认返回
    emit_mov_rsp_rbp(&state);
    emit_pop_rbp(&state);
    emit_ret(&state);

    // 复制到可执行内存
    memcpy(jit->code_buffer, state.asm->buffer, state.asm->size);
    jit_code_finalize(jit);
    flush_instruction_cache(jit->code_buffer, state.asm->size);

    free(state.asm->buffer);
    free(state.asm);
    free(state.const_pool);

    return jit->code_buffer;
}
```

### 追踪 JIT（Tracing JIT）

```c
// 追踪 JIT 编译器
// 识别热点执行路径（trace）并专门编译

typedef struct {
    BytecodeFunction* func;
    int pc;              // 程序计数器
    int* stack;
    int sp;              // 栈指针
} Interpreter;

// 执行计数器，用于识别热点
typedef struct {
    int* counters;       // 每个字节码位置的执行次数
    int threshold;       // 编译阈值
} HotSpotDetector;

// 记录执行路径
typedef struct {
    int* trace_pc;       // 记录的程序计数器序列
    int* trace_stack;    // 记录的栈状态
    int length;
    int capacity;
} TraceRecorder;

// 追踪 JIT 状态机
typedef enum {
    MODE_INTERPRET,      // 解释执行
    MODE_RECORD,         // 记录追踪
    MODE_COMPILE,        // 编译追踪
    MODE_EXECUTE_JIT     // 执行 JIT 代码
} ExecutionMode;

// 简单的追踪 JIT 实现
void tracing_jit_loop(Interpreter* interp) {
    HotSpotDetector hotspot = {
        .counters = calloc(interp->func->code_size, sizeof(int)),
        .threshold = 100
    };

    TraceRecorder recorder = {
        .trace_pc = malloc(1024 * sizeof(int)),
        .trace_stack = malloc(1024 * sizeof(int)),
        .length = 0,
        .capacity = 1024
    };

    ExecutionMode mode = MODE_INTERPRET;
    void* jited_trace = NULL;

    while (interp->pc < interp->func->code_size) {
        switch (mode) {
            case MODE_INTERPRET: {
                // 增加热点计数
                hotspot.counters[interp->pc]++;

                // 检查是否达到编译阈值
                if (hotspot.counters[interp->pc] >= hotspot.threshold) {
                    mode = MODE_RECORD;
                    recorder.length = 0;
                    printf("Start recording trace at PC=%d\n", interp->pc);
                }

                // 解释执行一条指令
                interpret_one_instruction(interp);
                break;
            }

            case MODE_RECORD: {
                // 记录当前状态
                recorder.trace_pc[recorder.length] = interp->pc;
                // 复制栈快照...
                recorder.length++;

                // 解释执行
                interpret_one_instruction(interp);

                // 检测循环闭合
                if (recorder.length > 10 && interp->pc == recorder.trace_pc[0]) {
                    mode = MODE_COMPILE;
                }

                if (recorder.length >= recorder.capacity - 1) {
                    // 追踪太长，放弃
                    mode = MODE_INTERPRET;
                }
                break;
            }

            case MODE_COMPILE: {
                // 将记录的执行路径编译为机器码
                jited_trace = compile_trace(&recorder, interp->func);
                mode = MODE_EXECUTE_JIT;
                printf("Compiled trace of %d instructions\n", recorder.length);
                break;
            }

            case MODE_EXECUTE_JIT: {
                // 检查是否可以进入追踪
                if (interp->pc == recorder.trace_pc[0]) {
                    // 调用 JIT 代码
                    typedef int (*TraceFunc)(int* stack);
                    interp->sp = ((TraceFunc)jited_trace)(
                        &interp->stack[interp->sp]
                    );
                    // 追踪执行后，继续在解释器执行
                } else {
                    interpret_one_instruction(interp);
                }
                break;
            }
        }
    }
}
```

---

## 安全考虑

### 安全威胁模型

```c
// 自修改代码的安全考虑

/*
 * 主要安全威胁：
 *
 * 1. 代码注入攻击
 *    - 攻击者通过缓冲区溢出等方式注入恶意代码
 *    - 防护: W^X 策略，ASLR，栈保护
 *
 * 2. 返回导向编程 (ROP)
 *    - 利用现有代码片段（gadgets）构造攻击
 *    - 防护: 栈金丝雀，ROP 检测
 *
 * 3. JIT 喷射 (JIT Spraying)
 *    - 通过控制 JIT 输入来生成恶意代码
 *    - 防护: 常量盲化 (constant blinding)
 *
 * 4. 代码重用攻击
 *    - 利用已有的代码进行攻击
 *    - 防护: CFI (Control Flow Integrity)
 */

// 安全实践：常量盲化
// 防止攻击者通过输入直接控制生成的常量

uint32_t blinding_key = 0;

void init_blinding() {
    // 初始化随机盲化密钥
    // 使用 /dev/urandom 或类似安全随机源
    int fd = open("/dev/urandom", O_RDONLY);
    read(fd, &blinding_key, sizeof(blinding_key));
    close(fd);
}

// 生成带盲化的常量加载
void emit_safe_load_const(Assembler* a, int reg, uint32_t value) {
    // 原始: mov $value, %reg
    // 盲化: mov $(value ^ key), %reg; xor $key, %reg

    uint32_t blinded = value ^ blinding_key;

    emit_mov_reg_imm32(a, reg, blinded);
    emit_xor_reg_imm32(a, reg, blinding_key);
}

// 代码签名验证
typedef struct {
    uint8_t signature[32];
    uint8_t hash[32];
} CodeSignature;

int verify_code_signature(void* code, size_t len, CodeSignature* sig) {
    // 计算代码哈希
    uint8_t computed_hash[32];
    sha256_hash(code, len, computed_hash);

    // 验证哈希匹配
    if (memcmp(computed_hash, sig->hash, 32) != 0) {
        return -1;  // 哈希不匹配
    }

    // 验证签名（使用公钥）
    if (!verify_rsa_signature(sig->hash, 32, sig->signature, public_key)) {
        return -1;  // 签名无效
    }

    return 0;
}

// 安全的 JIT 沙箱
typedef struct {
    JitCode* code;
    void* memory_base;
    size_t memory_size;
    // 资源限制
    size_t max_code_size;
    int max_execution_time_ms;
} JitSandbox;

JitSandbox* create_jit_sandbox(size_t code_size, size_t data_size) {
    JitSandbox* box = malloc(sizeof(JitSandbox));

    // 分配隔离的内存区域
    box->memory_base = mmap(
        NULL, data_size,
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS,
        -1, 0
    );
    box->memory_size = data_size;

    box->code = jit_code_create(code_size);
    box->max_code_size = code_size;

    return box;
}

// 限制 JIT 代码只能访问沙箱内存
void restrict_jit_memory_access(JitSandbox* box) {
    // 使用 seccomp-bpf 或类似机制
    // 限制系统调用
    // 或使用 Intel MPK / ARM MTE 等硬件特性
}
```

### 控制流完整性 (CFI)

```c
// 控制流完整性保护
// 确保间接调用只能跳转到合法的目标

// 影子栈（Shadow Stack）
__thread void* shadow_stack[1024];
__thread int shadow_sp = 0;

// 函数序言/尾声插入的 CFI 检查
void cfi_function_prologue(void* return_addr) {
    // 将返回地址保存到影子栈
    shadow_stack[shadow_sp++] = return_addr;
}

void* cfi_function_epilogue(void* actual_return_addr) {
    // 从影子栈弹出预期返回地址
    void* expected = shadow_stack[--shadow_sp];

    // 检查匹配
    if (expected != actual_return_addr) {
        // CFI 违规！
        abort();
    }

    return actual_return_addr;
}

// 间接调用目标验证表
void* valid_call_targets[1024];
int num_valid_targets = 0;

void register_valid_call_target(void* target) {
    valid_call_targets[num_valid_targets++] = target;
}

int is_valid_call_target(void* target) {
    for (int i = 0; i < num_valid_targets; i++) {
        if (valid_call_targets[i] == target) {
            return 1;
        }
    }
    return 0;
}

// 安全的间接调用
void* safe_indirect_call(void* target, void** args) {
    if (!is_valid_call_target(target)) {
        fprintf(stderr, "CFI violation: invalid call target %p\n", target);
        abort();
    }

    typedef void* (*func_t)(void**);
    return ((func_t)target)(args);
}
```

---

## 合法用途与应用

### 高性能计算

```c
// 领域特定语言 (DSL) 的 JIT 编译
// 示例：矩阵运算 DSL

typedef struct {
    int rows, cols;
    double* data;
} Matrix;

// 表达式模板 JIT 编译
// 将表达式如 C = A * B + D 编译为优化的机器码

typedef enum {
    EXPR_MATRIX,
    EXPR_ADD,
    EXPR_MUL,
    EXPR_SCALE
} ExprType;

typedef struct Expr {
    ExprType type;
    union {
        Matrix* matrix;
        struct { struct Expr* left; struct Expr* right; } binop;
        struct { double scalar; struct Expr* expr; } scale;
    };
} Expr;

// 编译矩阵表达式为优化的 x86-64 SIMD 代码
void* compile_matrix_expression(Expr* expr, int result_rows, int result_cols) {
    JitCode* jit = jit_code_create(8192);
    Assembler* a = assembler_create(8192);

    // 生成使用 AVX-512 的代码
    // 目标: void kernel(double* result, double* A, double* B, double* D)

    // 函数序言
    emit_push_rbp(a);
    emit_mov_rbp_rsp(a);

    // 加载参数
    // rdi = result, rsi = A, rdx = B, rcx = D

    // 生成表达式特定的代码
    switch (expr->type) {
        case EXPR_ADD: {
            // 生成 A + B 的向量化代码
            // vmovupd (%rsi), %zmm0
            // vaddpd (%rdx), %zmm0, %zmm0
            // vmovupd %zmm0, (%rdi)
            break;
        }
        case EXPR_MUL: {
            // 矩阵乘法的分块 SIMD 实现
            // 生成多层循环展开代码
            break;
        }
        // ... 其他操作
    }

    // 函数尾声
    emit_pop_rbp(a);
    emit_ret(a);

    // 复制到可执行内存
    memcpy(jit->code_buffer, a->buffer, a->size);
    jit_code_finalize(jit);
    flush_instruction_cache(jit->code_buffer, a->size);

    return jit->code_buffer;
}

// 使用示例
void matrix_expression_example() {
    Matrix A = {1024, 1024, malloc(1024*1024*sizeof(double))};
    Matrix B = {1024, 1024, malloc(1024*1024*sizeof(double))};
    Matrix D = {1024, 1024, malloc(1024*1024*sizeof(double))};
    Matrix C = {1024, 1024, malloc(1024*1024*sizeof(double))};

    // 构建表达式树: C = A * B + D
    Expr mul = {.type = EXPR_MUL, .binop = {&(Expr){.type=EXPR_MATRIX, .matrix=&A},
                                             &(Expr){.type=EXPR_MATRIX, .matrix=&B}}};
    Expr add = {.type = EXPR_ADD, .binop = {&mul,
                                             &(Expr){.type=EXPR_MATRIX, .matrix=&D}}};

    // JIT 编译
    typedef void (*MatKernel)(double*, double*, double*, double*);
    MatKernel kernel = compile_matrix_expression(&add, 1024, 1024);

    // 执行编译后的代码
    kernel(C.data, A.data, B.data, D.data);
}
```

### 解释器优化

```python
# Python 风格的小型 JIT 编译器示例
---

## 🔗 文档关联

### 前置知识
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [C语言基础](../01_Core_Knowledge_System/01_Basic_Layer/01_Syntax_Elements.md) | 基础依赖 | 语法基础 |
| [核心知识体系](../01_Core_Knowledge_System/readme.md) | 知识基础 | C语言核心概念 |

### 横向关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [形式语义](../02_Formal_Semantics_and_Physics/readme.md) | 理论支撑 | 形式化方法 |
| [系统技术](../03_System_Technology_Domains/readme.md) | 技术应用 | 系统级开发 |

### 后续延伸
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [工业场景](../04_Industrial_Scenarios/readme.md) | 实际应用 | 工业实践 |
| [安全标准](../01_Core_Knowledge_System/09_Safety_Standards/readme.md) | 安全规范 | 安全编码 |
# 演示了自修改代码在语言实现中的应用

import ctypes
import mmap
import os

class TinyJIT:
    """
    微型 JIT 编译器，将简单的算术表达式编译为 x86-64 代码
    """

    def __init__(self):
        # x86-64 操作码
        self.OPCODES = {
            'push_rax': bytes([0x50]),
            'pop_rax': bytes([0x58]),
            'add_rax_rbx': bytes([0x48, 0x01, 0xd8]),
            'mov_rax_imm32': lambda imm: bytes([0x48, 0xc7, 0xc0]) + imm.to_bytes(4, 'little', signed=True),
            'ret': bytes([0xc3]),
        }

    def compile_expr(self, expr):
        """
        编译简单表达式为机器码
        支持: 数字字面量、加法 (+)
        示例: "1 + 2 + 3"
        """
        code = bytearray()

        # 解析表达式（简化版）
        tokens = expr.replace('+', ' + ').split()

        # 生成代码
        # 第一个数加载到 rax
        first_num = int(tokens[0])
        code.extend(self.OPCODES['mov_rax_imm32'](first_num))

        # 处理加法
        i = 1
        while i < len(tokens):
            if tokens[i] == '+':
                next_num = int(tokens[i + 1])
                # push rax
                code.extend(self.OPCODES['push_rax'])
                # mov rax, next_num
                code.extend(self.OPCODES['mov_rax_imm32'](next_num))
                # pop rbx (rbx = 之前的值)
                code.extend(bytes([0x5b]))  # pop rbx
                # add rax, rbx
                code.extend(self.OPCODES['add_rax_rbx'])
                i += 2

        # ret
        code.extend(self.OPCODES['ret'])

        return bytes(code)

    def execute(self, machine_code):
        """
        在可执行内存中运行机器码
        """
        # 分配 RWX 内存
        size = len(machine_code)
        mem = mmap.mmap(-1, size,
                       prot=mmap.PROT_READ | mmap.PROT_WRITE | mmap.PROT_EXEC)

        # 写入代码
        mem.write(machine_code)

        # 转换为函数指针并调用
        func_type = ctypes.CFUNCTYPE(ctypes.c_int64)
        func = func_type(ctypes.addressof(ctypes.c_int.from_buffer(mem)))

        result = func()

        mem.close()
        return result

# 使用示例
jit = TinyJIT()

# 编译表达式
expr = "1 + 2 + 3 + 4 + 5"
machine_code = jit.compile_expr(expr)

print(f"表达式: {expr}")
print(f"机器码: {machine_code.hex()}")

result = jit.execute(machine_code)
print(f"结果: {result}")
```

---

## 高级技术

### 去优化（Deoptimization）

```c
// 推测优化失败时的去优化机制
// 从优化代码安全地回退到解释执行

typedef struct {
    void* exec_mode;      // 当前执行模式（解释/JIT）
    int* bytecode_pc;     // 对应字节码位置
    void** saved_state;   // 保存的执行状态
} DeoptInfo;

// 去优化点（当推测失败时跳转到这里）
void __attribute__((naked)) deoptimize_entry() {
    // 保存所有寄存器
    __asm__ volatile (
        "pushq %%rax\n\t"
        "pushq %%rbx\n\t"
        "pushq %%rcx\n\t"
        // ... 保存所有寄存器
        "call restore_interpreter_state\n\t"
        "jmp interpreter_loop\n\t"
    );
}

void restore_interpreter_state(DeoptInfo* info) {
    // 1. 恢复解释器栈
    // 2. 恢复局部变量
    // 3. 设置解释器 PC
    // 4. 切换到解释模式

    info->exec_mode = INTERPRETER_MODE;
}

// 在 JIT 代码中插入去优化检查
void emit_deoptimization_check(Assembler* a, int bytecode_offset) {
    // 检查条件（如类型检查失败）
    // cmp expected_type, actual_type
    // je continue
    // jmp deoptimize_entry

    // 保存去优化元数据
    DeoptInfo* info = malloc(sizeof(DeoptInfo));
    info->bytecode_pc = bytecode_offset;
    // 注册到去优化表中...
}
```

### 代码修补（On-Stack Replacement）

```c
// OSR: 在方法执行过程中切换实现
// 从解释执行切换到 JIT 编译版本

void on_stack_replacement(InterpreterFrame* frame, void* jited_code) {
    // 1. 暂停当前线程
    suspend_thread();

    // 2. 构建 JIT 函数的栈帧
    // 将解释器栈变量映射到 JIT 栈槽

    JITFrame jit_frame;
    jit_frame.local_vars = map_interpreter_locals(frame);
    jit_frame.operand_stack = map_interpreter_stack(frame);

    // 3. 计算 JIT 代码中的入口点
    // 不是函数开头，而是对应当前字节码位置的 OSR 入口
    void* osr_entry = find_osr_entry(jited_code, frame->current_pc);

    // 4. 修改返回地址，使其返回到 JIT 代码
    void** return_addr_slot = get_return_address_slot(frame);
    *return_addr_slot = osr_entry;

    // 5. 恢复执行
    resume_thread();
}
```

### 参考资料

1. "Engineering a Compiler" - Cooper & Torczon
2. "Modern Compiler Implementation in C" - Appel
3. "The Architecture of Open Source Applications: LLVM"
4. "V8 Design Elements" - Google V8 Team


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
