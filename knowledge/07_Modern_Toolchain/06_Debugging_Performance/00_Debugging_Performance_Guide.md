# C 语言调试与性能分析完全指南

> **难度**: L3-L5
> **工具链**: GDB, LLDB, Valgrind, perf, flamegraph
> **核心目标**: 从崩溃现场到性能瓶颈的系统化诊断

---

## 1. GDB 高级调试技巧

### 1.1 条件断点

```bash
# 只在变量满足条件时停止
(gdb) break process_item if index == 42
(gdb) break malloc if $size > 1000000   # 追踪大内存分配

# 忽略前N次命中
(gdb) break parse_line
(gdb) ignore 1 100    # 忽略断点1的前100次命中
```

### 1.2 观察点（数据断点）

```bash
# 硬件观察点：当变量被写入时停止（极慢，但精准）
(gdb) watch global_counter
(gdb) rwatch buffer[0]   # 读取时停止
(gdb) aware *0x7fff1234  # 对地址设置访问观察点

# 软件观察点：GDB单步检查（更慢，但无数量限制）
(gdb) set can-use-hw-watchpoints 0
(gdb) watch *0x601000
```

### 1.3 反向调试（时间旅行调试）

```bash
# 记录执行过程（内存开销大）
(gdb) target record-full
(gdb) continue

# 程序崩溃后，反向执行
(gdb) reverse-step    # 单步回退
(gdb) reverse-next    # 回退一步（不进入函数）
(gdb) reverse-continue # 回退到上一个断点

# 找到写入NULL指针的源头
(gdb) watch ptr
(gdb) reverse-continue   # 回退到ptr最后一次被修改的位置
```

### 1.4 GDB 脚本化

```python
# .gdbinit — 自动化调试会话
set pagination off
set confirm off

# 自定义命令：打印链表
define plist
    set $node = $arg0
    while $node
        print *$node
        set $node = $node->next
    end
end

# 自定义命令：打印调用栈中所有局部变量
define locals_all
    set $i = 0
    while $i < 10
        frame $i
        info locals
        set $i = $i + 1
    end
end
```

---

## 2. LLDB（macOS/Clang首选）

```bash
# 等效于 GDB 的命令对照
lldb ./myprogram

(lldb) breakpoint set --name main
(lldb) b process_item --condition 'index == 42'
(lldb) watchpoint set variable global_counter
(lldb) register read rax rdi rsi
(lldb) memory read --format x --count 16 $rsp

# Python脚本扩展
(lldb) script
>>> for frame in lldb.debugger.GetTargetAtIndex(0).GetProcess().GetThreadAtIndex(0):
...     print(frame.GetFunctionName())
```

---

## 3. Valgrind 内存检测

### 3.1 Memcheck（内存错误）

```bash
# 基本用法
valgrind --leak-check=full --show-leak-kinds=all \
         --track-origins=yes ./myprogram

# 输出解读
==12345== Invalid write of size 4          ← 错误类型
==12345==    at 0x401234: main (main.c:42)  ← 位置
==12345==    by 0x401156: process (main.c:20)
==12345==  Address 0x5203048 is 0 bytes after a block of size 40 alloc'd
==12345==    at 0x4C2FB0F: malloc (vg_replace_malloc.c:299)
==12345==    by 0x4010F3: init_buffer (main.c:15)
```

### 3.2 Cachegrind（缓存分析）

```bash
valgrind --tool=cachegrind ./myprogram
cg_annotate cachegrind.out.12345

# 输出：每个函数的缓存 misses
--------------------------------------------------------------------------------
         Ir          I1mr          ILmr          Dr          D1mr          DLmr
--------------------------------------------------------------------------------
6,742,811,586  1,234,567,890   123,456,789  2,345,678  456,789,012   45,678,901  ???:main
```

### 3.3 Callgrind（性能分析）

```bash
valgrind --tool=callgrind ./myprogram
kcachegrind callgrind.out.12345   # GUI可视化
```

---

## 4. AddressSanitizer 实战

### 4.1 编译与运行

```bash
# 编译（开发构建必备）
clang -fsanitize=address -g -O1 -fno-omit-frame-pointer \
      -o myprogram main.c

# 运行（自动检测并报告）
./myprogram

# 环境变量控制
ASAN_OPTIONS=detect_stack_use_after_return=1:halt_on_error=0 \
  ./myprogram
```

### 4.2 常见错误模式

```
# 堆缓冲区溢出
ERROR: AddressSanitizer: heap-buffer-overflow
WRITE of size 4 at 0x602000000014
# → 检查数组索引、循环边界

# Use-After-Free
ERROR: AddressSanitizer: heap-use-after-free
READ of size 8 at 0x603000000010
# → 检查指针生命周期、double-free

# Stack缓冲区溢出
ERROR: AddressSanitizer: stack-buffer-overflow
WRITE of size 20 at 0x7ffd1234abcd
# → 检查局部数组大小、strcpy/strcat
```

### 4.3 与 GDB 联用

```bash
# ASan 报告错误后进入 GDB
ASAN_OPTIONS=abort_on_error=1 gdb ./myprogram
(gdb) run
# ASan 检测到错误 → abort → GDB 捕获信号
(gdb) bt full    # 查看完整调用栈和局部变量
```

---

## 5. perf 性能分析（Linux）

### 5.1 基础采样

```bash
# 记录CPU周期
perf record -g ./myprogram
perf report --sort=dso,symbol   # 查看热点函数

# 记录特定事件
perf record -e cache-misses -g ./myprogram
perf record -e branch-misses -g ./myprogram
```

### 5.2 火焰图生成

```bash
# 1. 记录性能数据
perf record -F 99 -a -g -- sleep 30

# 2. 折叠栈
perf script | ./stackcollapse-perf.pl > out.folded

# 3. 生成火焰图
./flamegraph.pl out.folded > perf.svg

# 一键脚本
perf record -F 99 -a -g -- ./myprogram
perf script | ./stackcollapse-perf.pl | ./flamegraph.pl > flame.svg
```

### 5.3 火焰图解读

```
┌─────────────────────────────────────────────────────────────┐
│                    火焰图阅读指南                             │
├─────────────────────────────────────────────────────────────┤
│  • 宽度 = 该函数在采样中的占比（越宽越热）                     │
│  • 高度 = 调用栈深度                                          │
│  • 颜色 = 随机分配（无特殊含义）                               │
│  • 底部 = main() 或线程入口                                   │
│  • 寻找宽 plateau（平台）= 长时间停留在该函数                  │
└─────────────────────────────────────────────────────────────┘
```

---

## 6. 核心转储分析

### 6.1 生成核心转储

```bash
# 启用核心转储（临时）
ulimit -c unlimited

# 带符号表编译
gcc -g -O0 main.c -o myprogram

# 运行 → 崩溃 → 生成 core 文件
./myprogram    # Segmentation fault (core dumped)

# 分析
gdb ./myprogram core
(gdb) bt full           # 完整调用栈
(gdb) info registers    # 寄存器状态
(gdb) x/16x $rsp        # 栈内存
(gdb) info proc mappings # 内存映射
```

### 6.2 分析堆损坏

```bash
# 如果 core 文件显示堆损坏（非崩溃点）
(gdb) info proc mappings
# 找到堆区域 [heap]
(gdb) x/128x 0x555555559000   # 查看堆内容
(gdb) watch *(long*)0x555555559000  # 观察特定地址
```

---

## 7. 调试策略矩阵

| 问题类型 | 首选工具 | 备选工具 |
|:---------|:---------|:---------|
| 段错误/崩溃 | GDB + core文件 | LLDB |
| 内存泄漏 | Valgrind memcheck | ASan + LeakSanitizer |
| UAF/缓冲区溢出 | ASan | Valgrind |
| 数据竞争 | TSan | helgrind |
| 性能瓶颈 | perf + 火焰图 | Valgrind callgrind |
| 缓存未命中 | perf + cachegrind | Intel VTune |
| 死锁 | GDB thread apply all bt | TSan |

---

## 8. 检查清单

- [ ] 开发构建始终使用 `-g -O1`（保留调试信息，适度优化）
- [ ] CI 运行 ASan + UBSan
- [ ] 性能回归测试使用 perf 记录基线
- [ ] 发布前运行 Valgrind memcheck（如果可用）
- [ ] 保存核心转储配置（`ulimit -c unlimited`）
- [ ] 保留符号表用于生产环境调试（`-g` + `objcopy --strip-debug`）

---

> **最后更新**: 2026-05-11
> **参考**: GDB Manual, Brendan Gregg's Linux Performance, Google Sanitizers Wiki
