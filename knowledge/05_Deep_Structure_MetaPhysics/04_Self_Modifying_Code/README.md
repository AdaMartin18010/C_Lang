# 自修改代码 (Self-Modifying Code)

## 目录概述

本目录深入探讨**自修改代码(Self-Modifying Code, SMC)**的理论与技术，涵盖JIT(Just-In-Time)编译基础、追踪JIT优化、二进制翻译以及运行时代码生成等核心主题。

从运行时代码生成视角重新审视程序执行模型，探讨动态编译技术在虚拟机、模拟器和性能优化中的核心应用，同时分析安全考虑与合法用途。

---

## 理论背景

### 自修改代码的数学本质

自修改代码突破了传统**静态程序**的假设，引入**元循环(Meta-circular)**执行模型：

**定义 (自修改程序)**:
程序 P 是一个四元组 P = (C0, D, T, =>)，其中：

- C0：初始代码段
- D：数据段（可能包含代码模板）
- T: C x D -> C：代码变换函数
- =>：执行转换关系

程序执行的**双层级结构**：

```text
Level 1: 元级别 (Meta-level)
    +-----------------------+
    |   代码生成/优化引擎    |
    |   (JIT Compiler)      |
    +-----------+-----------+
                | 生成
                v
Level 0: 对象级别 (Object-level)
    +-----------------------+
    |    执行代码 (IR/机器码) |
    |    (Generated Code)   |
    +-----------------------+
```

---

## 1. JIT编译 (Just-In-Time Compilation)

### 1.1 JIT vs 解释器 vs AOT

| 执行方式 | 启动延迟 | 峰值性能 | 内存占用 | 适用场景 |
|----------|----------|----------|----------|----------|
| 解释器 | 极低 | 低 | 低 | 脚本、调试 |
| JIT编译 | 低-中 | 高-极高 | 中-高 | 虚拟机、动态语言 |
| AOT编译 | 无 | 高 | 低 | 系统程序、移动应用 |
| 分层编译 | 低 | 极高 | 高 | 通用JVM/JS引擎 |

### 1.2 JIT编译器架构

```text
+-------------------------------------------------+
|  源代码 / 字节码                                  |
+-------------------------------------------------+
                        |
                        v
+-------------------------------------------------+
|  前端                                            |
|  - 词法分析 -> 语法分析 -> 语义分析               |
|  - 生成中间表示 (IR: SSA / Three-address code)   |
+-------------------------------------------------+
                        |
                        v
+-------------------------------------------------+
|  优化层                                          |
|  - 内联、常量传播、死代码消除                      |
|  - 循环优化、向量化                                |
+-------------------------------------------------+
                        |
                        v
+-------------------------------------------------+
|  代码生成                                        |
|  - 指令选择、寄存器分配                            |
|  - 生成机器码到可执行内存                          |
+-------------------------------------------------+
                        |
                        v
+-------------------------------------------------+
|  代码缓存 & 执行                                   |
+-------------------------------------------------+
```

### 1.3 基础JIT实现示例

```c
// 简单的表达式JIT编译器
#include <sys/mman.h>
#include <string.h>

// x86-64机器码模板
typedef struct {
    uint8_t mov_rax_imm[2];   // 48 C7 C0: mov rax, imm32
    uint32_t imm;
    uint8_t ret;              // C3: ret
} CodeTemplate;

// 编译常量表达式：直接返回常量值
void* jit_compile_constant(int value) {
    // 分配可执行内存
    void* mem = mmap(NULL, 4096,
                     PROT_READ | PROT_WRITE | PROT_EXEC,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    CodeTemplate code = {
        .mov_rax_imm = {0x48, 0xC7, 0xC0},  // mov rax, imm32
        .imm = value,
        .ret = 0xC3                          // ret
    };

    memcpy(mem, &code, sizeof(code));

    // 刷新指令缓存 (x86需要)
    __builtin___clear_cache(mem, (char*)mem + sizeof(code));

    return mem;
}

// 使用
int main() {
    typedef long (*Func)(void);

    Func f = jit_compile_constant(42);
    printf("Result: %ld\n", f());  // 输出: 42

    munmap(f, 4096);
    return 0;
}
```

### 1.4 部分求值理论 (Partial Evaluation)

Futamura投影建立了编译与解释的关系：

**第一投影** (解释器编译化):

```text
target_program = mix(interpreter, source_program)
```

**第二投影** (编译器生成):

```text
compiler = mix(mix, interpreter)
```

**第三投影** (编译器生成器):

```text
cogen = mix(mix, mix)
```

```text
         源程序 + 输入
              |
    解释器 -> 输出
       |
       v
mix + 解释器 -> 目标程序
       |
       v
mix + mix -> 编译器生成器
```

---

## 2. 追踪JIT (Tracing JIT)

### 2.1 追踪概念

**追踪(Trace)**: 热点路径的线性指令序列

**定义 (追踪区域)**:
给定控制流图 G = (V, E)，追踪 tau 是路径 v0 -> v1 -> ... -> vn，满足：

- v0 是**追踪入口(Trace Head)**：入边来自区域外部
- vn 是**追踪出口(Trace Exit)**：出边指向区域外部

### 2.2 追踪JIT工作流程

```text
1. 解释执行 + profiling
        |
        v
2. 识别热点循环/路径
        |
        v
3. 记录字节码序列 (Trace)
        |
        v
4. 编译为优化机器码
   - 类型特化
   - 内联
   - 冗余消除
        |
        v
5. 执行优化代码
   - Guard检查
   - Side Exit处理
```

### 2.3 旁路处理 (Side Exit)

```c
// 追踪编译假设的类型特化
// 原始代码: x + y (可能为int或string)

// 假设观察到总是int，编译为：
guard_type(x, TYPE_INT);    // 检查失败则side exit
guard_type(y, TYPE_INT);
result = x + y;             // 直接整数加法

// Side Exit处理
// 1. 保存当前状态 (寄存器映射到栈)
// 2. 跳转回解释器对应位置
// 3. 可能触发新的追踪编译
```

### 2.4 内联缓存 (Inline Caching)

```c
// 多态调用优化
// obj->method(args);

// 单态内联缓存 (Monomorphic IC)
if (obj->vtable == expected_vtable) {
    // 快速路径：直接调用缓存的方法
    cached_method(obj, args);
} else {
    // 慢路径：查找方法，更新缓存
    method = lookup(obj, "method");
    update_cache(obj->vtable, method);
    method(obj, args);
}

// 多态内联缓存 (Megamorphic IC)
// 使用跳表处理多个常见类型
switch (obj->vtable->id) {
    case VTABLE_A: method_A(obj, args); break;
    case VTABLE_B: method_B(obj, args); break;
    default: lookup_and_call(obj, args);
}
```

---

## 3. 运行时代码生成

### 3.1 代码生成API设计

```c
// 简单的代码生成器

typedef struct {
    uint8_t *code;      // 代码缓冲区
    size_t size;        // 当前大小
    size_t capacity;    // 容量
} CodeGen;

CodeGen* codegen_create(void) {
    CodeGen *cg = malloc(sizeof(CodeGen));
    cg->capacity = 4096;
    cg->code = mmap(NULL, cg->capacity,
                    PROT_READ | PROT_WRITE | PROT_EXEC,
                    MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    cg->size = 0;
    return cg;
}

// _emit helpers
void emit_u8(CodeGen *cg, uint8_t b) {
    cg->code[cg->size++] = b;
}

void emit_u32(CodeGen *cg, uint32_t v) {
    memcpy(&cg->code[cg->size], &v, 4);
    cg->size += 4;
}

// x86-64指令生成
void emit_mov_rax_imm(CodeGen *cg, uint64_t imm) {
    if (imm <= 0xFFFFFFFF) {
        // mov eax, imm32 (零扩展)
        emit_u8(cg, 0xB8);  // opcode
        emit_u32(cg, (uint32_t)imm);
    } else {
        // mov rax, imm64
        emit_u8(cg, 0x48);  // REX.W
        emit_u8(cg, 0xB8);  // opcode
        memcpy(&cg->code[cg->size], &imm, 8);
        cg->size += 8;
    }
}

void emit_ret(CodeGen *cg) {
    emit_u8(cg, 0xC3);
}

// 生成函数
void* codegen_finalize(CodeGen *cg) {
    __builtin___clear_cache(cg->code, cg->code + cg->size);
    return cg->code;
}
```

### 3.2 动态代码修补

```c
// 热补丁技术：运行时替换函数实现

// 原始函数
int original_func(int x) {
    return x * 2;
}

// 新实现
int new_func(int x) {
    return x * 3;
}

// x86-64热补丁：5字节jmp相对跳转
void hot_patch(void *target, void *replacement) {
    // 计算相对偏移
    intptr_t offset = (intptr_t)replacement -
                      ((intptr_t)target + 5);

    // 修改内存保护为可写
    mprotect(PAGE_ALIGN(target), 4096,
             PROT_READ | PROT_WRITE | PROT_EXEC);

    // 写入跳转指令: E9 xx xx xx xx (jmp rel32)
    uint8_t *code = target;
    code[0] = 0xE9;  // jmp opcode
    memcpy(&code[1], &offset, 4);

    // 刷新缓存
    __builtin___clear_cache(target, (char*)target + 5);
}

// 使用
// hot_patch(original_func, new_func);
// original_func(10) 现在返回 30 而不是 20
```

---

## 4. 安全考虑

### 4.1 W XOR X (Write XOR Execute)

现代操作系统通过内存保护防止恶意自修改：

- **代码页**: 可执行但不可写 (RX)
- **数据页**: 可写但不可执行 (RW)

**合法JIT代码生成**需要：

```c
#include <sys/mman.h>

void* allocate_executable_memory(size_t size) {
    // 方法1: 先RW后RX
    void *mem = mmap(NULL, size, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    // 写入代码...
    // memcpy(mem, code, code_size);

    // 切换为只执行
    mprotect(mem, size, PROT_READ | PROT_EXEC);

    return mem;

    // 方法2: 使用双重映射 (更安全)
    // 一个RW映射用于写入，一个RX映射用于执行
}
```

### 4.2 安全威胁模型

| 威胁 | 机制 | 防护措施 |
|------|------|----------|
| 代码注入 | 缓冲区溢出覆写代码 | DEP/NX bit |
| 返回导向编程(ROP) | 重用现有代码片段 | ASLR、CFI |
| JIT喷射 | 预测JIT输出模式 | 常数致盲、随机化 |
| 代码重用攻击 | 重用合法代码 | CFI、Shadow Stack |

### 4.3 沙箱化JIT

```c
// 使用seccomp限制JIT编译器系统调用

#include <seccomp.h>

void setup_jit_sandbox(void) {
    scmp_filter_ctx ctx = seccomp_init(SCMP_ACT_KILL);

    // 允许基本操作
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(read), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(write), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(exit), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(exit_group), 0);

    // 允许mmap/mprotect用于代码生成
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(mmap), 0);
    seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(mprotect), 0);

    // 禁止网络、文件系统等危险操作
    seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(socket), 0);
    seccomp_rule_add(ctx, SCMP_ACT_KILL, SCMP_SYS(open), 0);

    seccomp_load(ctx);
}
```

### 4.4 控制流完整性 (CFI)

```c
// 编译器辅助的CFI检查
// Clang: -fsanitize=cfi

// 间接调用检查
void safe_indirect_call(void *func, void *arg) {
    // 检查目标是否在合法函数地址表中
    if (!is_valid_function_pointer(func)) {
        abort();  // CFI违规
    }
    ((void(*)(void*))func)(arg);
}

// 影子栈 (Shadow Stack)
// 维护独立的返回地址栈，防止ROP
void function_prologue(void) {
    // 保存返回地址到影子栈
    shadow_stack[shadow_sp++] = __builtin_return_address(0);
}

void function_epilogue(void) {
    // 验证返回地址
    void *shadow_ret = shadow_stack[--shadow_sp];
    void *actual_ret = __builtin_return_address(0);
    if (shadow_ret != actual_ret) {
        abort();  // 栈被篡改
    }
}
```

---

## 5. 合法用途

### 5.1 虚拟机实现

```text
+---------------------+
|   高级语言源码       |
+---------------------+
         |
         v
+---------------------+
|   编译器前端         |
|   -> 字节码/IR      |
+---------------------+
         |
         v
+---------------------+
|   执行引擎           |
|   - 解释器 (冷代码)  |
|   - JIT (热点代码)   |
+---------------------+
         |
         v
+---------------------+
|   运行时服务         |
|   GC、异常处理等     |
+---------------------+

实例: JVM HotSpot, JavaScript V8, .NET CLR
```

### 5.2 动态二进制翻译

```c
// QEMU风格的DBT (Dynamic Binary Translation)

// 将guest架构代码翻译为host架构代码
void* translate_basic_block(GuestPC pc) {
    // 1. 获取guest代码
    GuestInst *guest_code = fetch_guest_code(pc);

    // 2. 解码guest指令
    // 3. 生成等效host指令序列
    CodeGen *cg = codegen_create();

    while (!is_block_end(guest_code)) {
        TCGOp *ops = guest_to_tcg(guest_code);  // 中间表示
        tcg_to_host(cg, ops);                    // 生成host代码
        guest_code++;
    }

    // 4. 链接到翻译缓存
    void *host_code = codegen_finalize(cg);
    install_in_tlb(pc, host_code);

    return host_code;
}
```

### 5.3 性能剖析与优化

```c
// 基于JIT的SPEC2017风格优化

void optimize_hot_loop(LoopInfo *loop) {
    // 收集运行时信息
    ProfileData *profile = get_profile(loop);

    // 根据实际执行路径特化
    if (profile->always_taken_branch) {
        // 移除条件分支
        specialize_for_taken(loop);
    }

    if (profile->constant_args) {
        // 常量传播
        constant_fold(loop, profile->constants);
    }

    if (profile->vectorizable) {
        // 自动向量化
        vectorize_loop(loop);
    }

    // 生成优化版本
    return jit_compile_optimized(loop);
}
```

### 5.4 领域特定语言 (DSL)

```c
// 图像处理DSL的JIT编译

void process_image_jit(Image *img, FilterExpr *expr) {
    // 分析filter表达式
    // 为特定图像格式和filter组合生成优化代码

    CodeGen *cg = codegen_create();

    // 生成向量化处理循环
    emit_vectorized_loop_header(cg, img->width, img->height);

    // 根据expr生成处理逻辑
    for (int i = 0; i < expr->num_ops; i++) {
        emit_filter_op(cg, expr->ops[i]);
    }

    emit_loop_footer(cg);
    emit_ret(cg);

    // 执行
    void (*filter_func)(Image*) = codegen_finalize(cg);
    filter_func(img);
}
```

---

## 6. 高级主题

### 6.1 分层编译

```text
Tier 0: 解释器
   - 快速启动
   - 收集profiling信息
         |
         v (热点检测)
Tier 1: 简单JIT (C1)
   - 快速编译
   - 基本优化
   - 继续profiling
         |
         v (更热)
Tier 2: 优化JIT (C2)
   - 激进优化
   - 类型特化
   - 向量化
         |
         v (去优化)
回到Tier 1 (假设失败时)
```

### 6.2 去优化 (Deoptimization)

```c
// 当优化假设失败时安全回退

void deoptimize(OptimizedFrame *frame) {
    // 1. 从优化代码状态恢复解释器状态
    InterpreterState *interp_state = allocate_interp_state();

    // 2. 寄存器 -> 栈帧映射
    for (int i = 0; i < frame->num_locals; i++) {
        interp_state->locals[i] = frame->registers[frame->reg_map[i]];
    }

    // 3. 恢复操作数栈
    interp_state->stack = reconstruct_stack(frame);

    // 4. 设置解释器PC
    interp_state->pc = frame->deopt_pc;

    // 5. 跳转到解释器
    enter_interpreter(interp_state);
}
```

---

## 文件结构

| 文件名 | 难度 | 核心内容 |
|--------|------|----------|
| `01_JIT_Basics.md` | 4星 | JIT编译器架构、分层编译、代码缓存管理 |
| `02_Tracing_JIT.md` | 5星 | 追踪记录与编译、旁路处理、内联缓存 |
| `03_Runtime_Code_Generation.md` | 4星 | 代码生成API、动态修补、元编程 |
| `04_Security_Considerations.md` | 4星 | W XOR X、威胁模型、沙箱化、CFI |
| `05_Legitimate_Uses.md` | 3星 | 虚拟机、DBT、DSL、性能优化 |

---

## 技术指标对比

| 技术 | 启动延迟 | 峰值性能 | 内存占用 | 适用场景 |
|------|----------|----------|----------|----------|
| 解释器 | 极低 | 低 | 低 | 冷代码、调试 |
| 方法JIT | 中 | 高 | 中 | 稳定热点方法 |
| 追踪JIT | 低 | 极高 | 中 | 循环密集型 |
| AOT编译 | 无 | 高 | 低 | 启动敏感 |
| 分层编译 | 低 | 极高 | 高 | 通用场景 |

---

## 前置知识

- **编译原理**: 代码生成、寄存器分配、优化pass
- **计算机体系结构**: 指令集架构、流水线、缓存层次
- **操作系统**: 虚拟内存、代码页保护、信号处理
- **运行时系统**: 垃圾回收、栈展开、异常处理

---

## 学术参考文献

### JIT编译与部分求值

1. **Aycock, J.** (2003). *A Brief History of Just-In-Time*. ACM Computing Surveys, 35(2), 97-113.

2. **Jones, N. D., et al.** (1993). *Partial Evaluation and Automatic Program Generation*. Prentice Hall.

3. **Futamura, Y.** (1999). *Partial Evaluation of Computation Process*. Higher-Order and Symbolic Computation.

### 追踪JIT

1. **Gal, A., et al.** (2009). *Trace-Based Just-In-Time Type Specialization*. In PLDI '09.

2. **Bala, V., et al.** (2000). *Dynamo: A Transparent Dynamic Optimization System*. In PLDI '00.

3. **Bolz, C. F., et al.** (2009). *Tracing the Meta-Level: PyPy's Tracing JIT*. In ICOOOLPS '09.

---

## 学习路径建议

```text
编译基础 -> 运行时系统 -> JIT基础 -> 追踪JIT -> 二进制翻译 -> 虚拟机优化
    |            |           |          |            |            |
    v            v           v          v            v            v
 代码生成      内存管理    分层编译   路径优化    指令模拟    元循环优化
```

---

## 相关链接

- [父目录: 深层结构与元物理学](../README.md)
- [形式语义学](../01_Formal_Semantics/README.md)
- [C到汇编映射](../../02_Formal_Semantics_and_Physics/06_C_Assembly_Mapping/README.md)

---

*本目录内容适合虚拟机开发者、运行时系统工程师及高级编译器优化研究者阅读。*

*最后更新: 2026-03-13*:
