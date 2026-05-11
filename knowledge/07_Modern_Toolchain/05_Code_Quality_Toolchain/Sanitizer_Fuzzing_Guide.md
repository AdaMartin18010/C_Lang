# Sanitizer 与模糊测试：C 代码安全检测完全指南

> **适用编译器**: GCC 13+, Clang 16+, MSVC 2022 (部分支持)
> **核心目标**: 在开发阶段捕获 90%+ 的内存安全和 UB 问题

---

## 1. Sanitizer 家族概览

### 1.1 四大 Sanitizer 对比

```
┌─────────────────────────────────────────────────────────────┐
│                        Sanitizer 矩阵                         │
├──────────────┬────────────────────┬───────────┬─────────────┤
│   Sanitizer  │    检测问题         │  开销    │   平台      │
├──────────────┼────────────────────┼───────────┼─────────────┤
│ ASan         │ 堆/栈/全局溢出      │   2-3x   │ x86, ARM    │
│              │ Use-After-Free     │          │             │
│              │ Double-Free        │          │             │
├──────────────┼────────────────────┼───────────┼─────────────┤
│ UBSan        │ 有符号溢出         │  1.1-1.5x│ 所有平台    │
│              │ 空指针解引用       │          │             │
│              │ 对齐违规           │          │             │
│              │ 数组越界 (编译期)  │          │             │
├──────────────┼────────────────────┼───────────┼─────────────┤
│ MSan         │ 未初始化内存读取   │   3-5x   │ x86_64      │
│              │                    │          │ (仅 Clang)  │
├──────────────┼────────────────────┼───────────┼─────────────┤
│ HWASan       │ 基于标签的内存错误 │  1.5-2x  │ ARM64 (TBI) │
│              │ (ASan 的硬件加速版)│          │ x86_64 (LAM)│
├──────────────┼────────────────────┼───────────┼─────────────┤
│ TSan         │ 数据竞争           │   5-15x  │ x86, ARM    │
│              │ 死锁               │          │             │
└──────────────┴────────────────────┴───────────┴─────────────┘
```

### 1.2 C23 编译器支持状态 (2026)

| 编译器 | ASan | UBSan | MSan | HWASan | TSan |
|:-------|:-----|:------|:-----|:-------|:-----|
| **GCC 15** | ✅ | ✅ | ❌ | ❌ | ✅ |
| **Clang 19** | ✅ | ✅ | ✅ | ✅ (ARM64) | ✅ |
| **MSVC 2022** | ✅ (实验) | ⚠️ (部分) | ❌ | ❌ | ✅ (实验) |

> ⚠️ **MSVC 警告**: MSVC 的 C23 支持和 Sanitizer 功能严重滞后于 GCC/Clang。生产环境建议使用 Clang 或 GCC。

---

## 2. AddressSanitizer (ASan)

### 2.1 工作原理

ASan 使用**影子内存 (Shadow Memory)** 技术：

```
应用程序内存每 8 字节 → 影子内存 1 字节
影子内存值编码：
  0  = 全部 8 字节可寻址
  k  = 前 k 字节可寻址 (1-7)
  负值 = 不可寻址（红区）
```

```
┌──────────────┐      ┌──────────────┐
│  应用程序     │      │   影子内存    │
│  堆内存       │─────→│   (1/8 大小) │
│              │      │              │
│ [红区][数据][红区] │      │ [FF][00][FF] │
└──────────────┘      └──────────────┘
        ↑
   访问红区 → 影子内存 FF → ASan 报告错误
```

### 2.2 编译与运行

```bash
# GCC
 gcc -fsanitize=address -g -O1 main.c -o main
 ./main

# Clang (支持更多检测)
clang -fsanitize=address -fno-omit-frame-pointer -g -O1 main.c -o main

# 环境变量控制
ASAN_OPTIONS=detect_stack_use_after_return=1:abort_on_error=1 ./main
```

### 2.3 典型错误检测示例

```c
// === 堆缓冲区溢出 ===
int *arr = malloc(10 * sizeof(int));
arr[10] = 42;  // ASan: ERROR: AddressSanitizer: heap-buffer-overflow

// === Use-After-Free ===
free(arr);
arr[0] = 42;   // ASan: ERROR: AddressSanitizer: heap-use-after-free

// === 栈缓冲区溢出 ===
void stack_overflow() {
    char buf[10];
    strcpy(buf, "this is way too long");  // stack-buffer-overflow
}

// === 全局缓冲区溢出 ===
int global_arr[10];
void global_overflow() {
    global_arr[10] = 42;  // global-buffer-overflow
}
```

### 2.4 ASan 输出解读

```
==12345==ERROR: AddressSanitizer: heap-buffer-overflow on address 0x602000000014
WRITE of size 4 at 0x602000000014 thread T0
    #0 0x55a3e7 in main /path/to/main.c:42:5
    #1 0x7f8b2c in __libc_start_main
    #2 0x55a2a0 in _start

0x602000000014 is located 0 bytes after 40-byte region [0x602000000010,0x602000000038)
allocated by thread T0 here:
    #0 0x7f8a1b in malloc
    #1 0x55a3c0 in main /path/to/main.c:38:15
```

**关键信息**:
- 错误类型: `heap-buffer-overflow`
- 操作: `WRITE of size 4`
- 位置: `main.c:42:5`
- 内存块大小: `40-byte region`（10个int）
- 分配位置: `main.c:38:15`

---

## 3. UndefinedBehaviorSanitizer (UBSan)

### 3.1 检测的 UB 类型

```c
// === 有符号整数溢出 ===
int x = INT_MAX;
x++;  // UBSan: runtime error: signed integer overflow

// === 空指针解引用 ===
int *p = NULL;
*p = 42;  // UBSan: runtime error: load of null pointer

// === 对齐违规 ===
char buf[8];
int *ip = (int*)(buf + 1);
*ip = 42;  // UBSan: runtime error: store to misaligned address

// === 数组越界（VLA/指针算术）===
int arr[10];
int *p = &arr[10];  // 合法（尾后指针）
*p = 42;            // UBSan: runtime error: index out of bounds

// === 未定义移位 ===
int x = 1 << 33;   // UBSan: shift exponent 33 too large
int y = -1 << 1;   // UBSan: left shift of negative value
```

### 3.2 编译配置

```bash
# 基本 UBSan
clang -fsanitize=undefined -g -O1 main.c -o main

# 细分控制（推荐）
clang -fsanitize=alignment,array-bounds,bool,builtin,enum, \
      float-cast-overflow,float-divide-by-zero,integer-divide-by-zero, \
      nonnull-attribute,null,object-size,return,returns-nonnull-attribute, \
      shift,signed-integer-overflow,unreachable,vla-bound \
      -g -O1 main.c -o main

# 不终止执行（仅记录）
clang -fsanitize=undefined -fno-sanitize-recover=all main.c
```

### 3.3 与 ASan 联合使用

```bash
# 推荐的开发构建配置
CFLAGS="\
    -fsanitize=address,undefined \
    -fno-omit-frame-pointer \
    -fno-sanitize-recover=all \
    -g3 -O1 \
    -Wall -Wextra -Werror"

# 说明：
# -O1 足够保留调试信息，-O0 会让 ASan 误报
# -fno-omit-frame-pointer 保证栈跟踪准确
# -fno-sanitize-recover=all 遇到错误立即终止（CI 需要）
```

---

## 4. MemorySanitizer (MSan)

### 4.1 为什么需要 MSan

ASan 检测**越界访问**，但无法检测**使用未初始化内存**：

```c
// ASan 无法检测此错误！
int *p = malloc(sizeof(int));
// 未初始化
if (*p > 0) {   // 使用未初始化值 → 未定义行为
    printf("true\n");
}
```

MSan 追踪每个内存位的初始化状态。

### 4.2 使用限制

```bash
# ⚠️ MSan 仅 Clang 支持
cdang -fsanitize=memory -fno-omit-frame-pointer -g -O2 main.c -o main

# ⚠️ 重要限制：
# 1. 所有库必须也用 MSan 编译（包括 libc）
# 2. 不能与其他 sanitizer 混用
# 3. 仅 Linux x86_64
# 4. 需要干净的构建环境
```

### 4.3 检测示例

```c
// MSan 输出：WARNING: MemorySanitizer: use-of-uninitialized-value
void msan_demo() {
    int x;
    if (x == 0) {  // 读取未初始化变量
        printf("x is zero\n");
    }
}

// 传播追踪
void msan_propagation() {
    int x;           // 未初始化
    int y = x + 1;   // y 也变为"污染"
    int z = y * 2;   // z 也变为"污染"
    if (z > 0) {     // MSan 报告：z 的创建追溯到 x
        ...
    }
}
```

---

## 5. ThreadSanitizer (TSan)

### 5.1 数据竞争检测

```c
// === 数据竞争示例 ===
int shared = 0;

void *thread1(void *arg) {
    shared = 1;       // 写操作，无锁保护
    return NULL;
}

void *thread2(void *arg) {
    int x = shared;   // 读操作，无锁保护 → 与 thread1 数据竞争
    return NULL;
}

// TSan 输出：
// WARNING: ThreadSanitizer: data race
//   Write of size 4 at 0x... by thread T1:
//   Previous read of size 4 at 0x... by thread T2:
```

### 5.2 编译与使用

```bash
clang -fsanitize=thread -g -O1 threaded_program.c -o threaded_program -pthread
./threaded_program

# TSan 会报告：
# - 数据竞争
# - 死锁 (lock-order-inversion)
# - 线程泄漏
```

### 5.3 与 ASan 的关系

> ⚠️ **TSan 和 ASan 不能同时使用！**
> 
> 建议分开运行：
> - 开发阶段：ASan + UBSan（单线程测试）
> - 并发测试：TSan（专门阶段）

---

## 6. CI/CD 集成

### 6.1 GitHub Actions 配置

```yaml
# .github/workflows/sanitizer.yml
name: Sanitizer Checks

on: [push, pull_request]

jobs:
  asan:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      
      - name: Build with ASan + UBSan
        run: |
          export CC=clang
          export CFLAGS="-fsanitize=address,undefined \
                         -fno-omit-frame-pointer \
                         -fno-sanitize-recover=all \
                         -g -O1"
          make clean && make
      
      - name: Run tests
        run: |
          ASAN_OPTIONS=detect_leaks=1:abort_on_error=1 \
          UBSAN_OPTIONS=print_stacktrace=1:halt_on_error=1 \
          make test

  tsan:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      
      - name: Build with TSan
        run: |
          export CC=clang
          export CFLAGS="-fsanitize=thread -g -O1"
          make clean && make
      
      - name: Run threaded tests
        run: make test-threaded
```

### 6.2 性能测试分离

```bash
# 生产构建（无 sanitizer）
gcc -O3 -DNDEBUG -fstack-protector-strong main.c -o main_prod

# 测试构建（全 sanitizer）
clang -fsanitize=address,undefined -g -O1 main.c -o main_test

# 模糊测试构建（专门优化）
clang -fsanitize=fuzzer,address,undefined \
      -g -O1 fuzz_target.c -o fuzzer
```

---

## 7. 模糊测试 (Fuzzing)

### 7.1 模糊测试原理

```
输入语料库 → 变异引擎 → 被测程序
                 ↑___________↓
                 代码覆盖率反馈 (覆盖导向)
```

### 7.2 libFuzzer 集成 (Clang)

```c
// fuzz_parser.c - 模糊测试目标
#include <stdint.h>
#include <stddef.h>

// 被测函数
int parse_header(const uint8_t *data, size_t len);

// libFuzzer 入口
int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    parse_header(data, size);
    return 0;  // 非零值表示输入被丢弃
}
```

```bash
# 编译
clang -fsanitize=fuzzer,address,undefined \
      -g -O1 \
      fuzz_parser.c parser.c \
      -o fuzz_parser

# 运行（初始语料库 + 自动变异）
mkdir -p corpus
echo -n "valid" > corpus/seed1
./fuzz_parser corpus/ -max_total_time=3600 -jobs=4

# 输出
# corpus/          - 覆盖最大化的输入集合
# crash-*          - 导致崩溃的输入
# slow-*           - 执行时间过长的输入
# timeout-*        - 超时输入
```

### 7.3 AFL++ 集成

```bash
# 编译 (AFL++ 模式)
afl-clang-fast -fsanitize=address -g -O1 \
    fuzz_parser.c parser.c -o fuzz_parser

# 运行 fuzzer
mkdir -p in out
echo "seed" > in/seed1
afl-fuzz -i in -o out -d ./fuzz_parser

# 并行模式
afl-fuzz -i in -o out -M main ./fuzz_parser &
afl-fuzz -i in -o out -S worker1 ./fuzz_parser &
afl-fuzz -i in -o out -S worker2 ./fuzz_parser &
```

### 7.4 结构化模糊测试（推荐）

对于复杂输入格式（如协议、文件格式），使用结构化模糊测试：

```c
// 使用 libprotobuf-mutator 生成结构化输入
// 或手动定义输入约束

// 自定义变异器示例：确保输入是有效的 TLV 结构
extern "C" size_t LLVMFuzzerCustomMutator(
    uint8_t *data, size_t max_size, unsigned int seed) {
    
    // 1. 尝试解析现有输入
    TLV tlv;
    if (tlv_parse(data, max_size, &tlv)) {
        // 2. 合法变异：修改值、添加字段
        tlv_mutate(&tlv, seed);
        return tlv_serialize(&tlv, data, max_size);
    }
    
    // 3. 回退：生成最小合法输入
    return tlv_generate_minimal(data, max_size);
}
```

---

## 8. 检查清单

### 8.1 项目级 Sanitizer 配置

- [ ] 开发构建默认启用 ASan + UBSan
- [ ] CI 运行 ASan + UBSan 测试套件
- [ ] 夜间构建运行 MSan（如可行）
- [ ] 并发测试运行 TSan
- [ ] 模糊测试持续运行（至少 1 小时/提交）
- [ ] 生产构建使用 `-fstack-protector-strong -D_FORTIFY_SOURCE=3`

### 8.2 常见陷阱

| 陷阱 | 原因 | 解决 |
|:-----|:-----|:-----|
| ASan 误报在 `-O0` | 帧指针优化问题 | 使用 `-O1` 或 `-fno-omit-frame-pointer` |
| UBSan 不终止 | 默认仅打印 | 使用 `-fno-sanitize-recover=all` |
| MSan 需要全量重编译 | 插桩要求 | 建立 MSan 专用构建容器 |
| TSan + ASan 冲突 | 影子内存冲突 | 分开运行 |
| 模糊测试覆盖率低 | 无有效种子 | 提供多样化的初始语料库 |

---

> **最后更新**: 2026-05-11
> **参考**: Clang Documentation, Google Sanitizers Wiki, AFL++ Documentation
