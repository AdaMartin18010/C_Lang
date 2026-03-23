# 系统化调试方法论

> **层级定位**: 06 Thinking Representation
> **难度级别**: L4-L5
> **目标**: 建立可复现、可度量、可改进的调试体系

---

## 一、调试方法论框架

### 1.1 科学调试法

```
科学调试法（基于假设检验）：

观察现象 ──> 形成假设 ──> 设计实验 ──> 执行实验 ──> 分析结果
    ▲                                              │
    └──────────────── 验证/修正假设 ←───────────────┘

与随机调试的区别：
┌─────────────────┬──────────────────────────────────────────────┐
│   随机调试       │   科学调试                                    │
├─────────────────┼──────────────────────────────────────────────┤
│ 试错式修改       │ 基于假设的验证                                │
│ 改完不知道原因   │ 每次实验都增加知识                            │
│ 容易引入新问题   │ 修改范围可控                                  │
│ 无法复现解决方案 │ 形成可复用的诊断流程                          │
└─────────────────┴──────────────────────────────────────────────┘
```

### 1.2 DEBUG框架

```
D - Define: 定义问题
    • 什么现象？
    • 什么时候发生？
    • 影响范围？
    • 可复现性？

E - Examine: 检查证据
    • 日志分析
    • 监控指标
    • 核心转储
    • 用户报告

B - Bound: 界定范围
    • 二分法缩小范围
    • 确定问题边界
    • 识别相关组件

U - Understand: 理解原因
    • 根因分析
    • 代码审查
    • 数据流追踪

G - Generate: 生成修复
    • 最小化修复
    • 回归测试
    • 文档更新
```

---

## 二、问题分类与诊断策略

### 2.1 问题分类矩阵

```
┌─────────────────────────────────────────────────────────────┐
│                    问题分类与诊断策略                        │
├───────────────────┬─────────────────────────────────────────┤
│ 功能性问题         │ 症状: 输出错误、逻辑错误                │
│                   │ 工具: GDB、单元测试、断点               │
│                   │ 方法: 输入输出对比、边界值分析          │
├───────────────────┼─────────────────────────────────────────┤
│ 性能问题           │ 症状: 响应慢、资源占用高                │
│                   │ 工具: perf、火焰图、profiler            │
│                   │ 方法: 瓶颈分析、Amdahl定律              │
├───────────────────┼─────────────────────────────────────────┤
│ 内存问题           │ 症状: 崩溃、泄漏、越界                  │
│                   │ 工具: ASan、Valgrind、GDB               │
│                   │ 方法: 影子内存分析、引用追踪            │
├───────────────────┼─────────────────────────────────────────┤
│ 并发问题           │ 症状: 死锁、数据竞争、时序问题          │
│                   │ 工具: TSan、Helgrind、逻辑分析器        │
│                   │ 方法: happens-before分析、模型检测      │
├───────────────────┼─────────────────────────────────────────┤
│ 环境/配置问题      │ 症状: 只在特定环境出现                  │
│                   │ 工具: 配置对比、容器化复现              │
│                   │ 方法: 控制变量法、最小复现环境          │
└───────────────────┴─────────────────────────────────────────┘
```

### 2.2 二分法调试

```c
/*
 * 二分法调试：快速定位问题引入点
 */

// 场景：1000次提交后发现问题
// 方法：git bisect自动化

// 步骤1: 标记好提交和坏提交
git bisect start
git bisect bad HEAD          // 当前有问题的版本
git bisect good v1.0         // 已知正常的版本

// 步骤2: 自动化测试脚本
// bisect_test.sh
#!/bin/bash
make clean && make
./run_tests.sh
exit $?  # 0=good, 非0=bad

// 步骤3: 自动二分
git bisect run ./bisect_test.sh

// 结果：Git会自动定位到引入问题的精确提交

// ========== 代码级二分 ==========

// 在函数中二分查找问题
void problematic_function() {
    // 检查点1: 函数入口
    ASSERT(check_state_A());

    // ... 代码段1 ...

    // 检查点2: 中间状态
    ASSERT(check_state_B());

    // ... 代码段2 ...

    // 检查点3: 函数出口
    ASSERT(check_state_C());
}
```

---

## 三、动态分析技术

### 3.1 执行追踪

```c
/*
 * 函数进入/退出追踪
 */

#define TRACE_ENTER() \
    do { \
        fprintf(stderr, "[TRACE] ENTER %s:%d %s\n", \
                __FILE__, __LINE__, __func__); \
    } while(0)

#define TRACE_EXIT() \
    do { \
        fprintf(stderr, "[TRACE] EXIT %s:%d %s\n", \
                __FILE__, __LINE__, __func__); \
    } while(0)

#define TRACE_VAR(var) \
    do { \
        fprintf(stderr, "[TRACE] %s = %d (at %s:%d)\n", \
                #var, (int)(var), __FILE__, __LINE__); \
    } while(0)

// 使用
void complex_function(int x) {
    TRACE_ENTER();
    TRACE_VAR(x);

    int result = calculate(x);
    TRACE_VAR(result);

    TRACE_EXIT();
    return result;
}
```

### 3.2 状态机验证

```c
/*
 * 运行时状态机验证
 */

typedef enum {
    STATE_INIT,
    STATE_CONNECTING,
    STATE_CONNECTED,
    STATE_DISCONNECTING,
    STATE_CLOSED
} connection_state_t;

// 有效状态转换表
static const int valid_transitions[5][5] = {
    /* FROM \ TO   INIT  CONN  COND  DISC  CLOS */
    /* INIT */  { 0,    1,    0,    0,    1 },
    /* CONN */  { 0,    0,    1,    1,    0 },
    /* COND */  { 0,    0,    0,    1,    1 },
    /* DISC */  { 0,    0,    0,    0,    1 },
    /* CLOS */  { 0,    0,    0,    0,    0 }
};

void state_transition(connection_t *conn, connection_state_t new_state) {
    connection_state_t old_state = conn->state;

    // 验证转换合法性
    if (!valid_transitions[old_state][new_state]) {
        fprintf(stderr, "Invalid state transition: %d -> %d\n",
                old_state, new_state);
        abort();  // 或记录并恢复
    }

    conn->state = new_state;
}
```

---

## 四、内存调试策略

### 4.1 内存问题诊断流程

```
内存问题诊断流程：

┌─────────────────────────────────────────────────────────────┐
│  问题: 程序崩溃、异常行为                                    │
└─────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────┐
│  步骤1: 识别症状                                             │
│  • SIGSEGV/SIGABRT                                          │
│  • 数据损坏                                                  │
│  • 性能下降                                                  │
└─────────────────────────────────────────────────────────────┘
                            │
              ┌─────────────┼─────────────┐
              ▼             ▼             ▼
        ┌─────────┐   ┌─────────┐   ┌─────────┐
        │越界访问 │   │使用已释放│   │内存泄漏 │
        └────┬────┘   └────┬────┘   └────┬────┘
             │             │             │
             ▼             ▼             ▼
        ┌─────────┐   ┌─────────┐   ┌─────────┐
        │ ASan    │   │ ASan    │   │ Valgrind│
        │ GDB+hw  │   │ 僵尸指针 │   │ LSan    │
        │ bp     │   │ 检测     │   │ 自定义  │
        └─────────┘   └─────────┘   └─────────┘
```

### 4.2 内存损坏定位

```c
/*
 * 内存损坏检测：使用Guard Pages
 */

#include <sys/mman.h>
#include <signal.h>

// 分配受保护的内存区域
void *guarded_malloc(size_t size) {
    // 分配额外两页作为保护页
    size_t pagesize = sysconf(_SC_PAGESIZE);
    size_t total = ((size + pagesize - 1) / pagesize + 2) * pagesize;

    char *mem = mmap(NULL, total, PROT_NONE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    // 中间页可读写
    mprotect(mem + pagesize, size, PROT_READ | PROT_WRITE);

    // 返回可写区域起始地址
    return mem + pagesize;
}

void guarded_free(void *ptr, size_t size) {
    size_t pagesize = sysconf(_SC_PAGESIZE);
    char *base = (char *)ptr - pagesize;
    size_t total = ((size + pagesize - 1) / pagesize + 2) * pagesize;

    munmap(base, total);
}

// 越界访问将触发SIGSEGV，可被GDB捕获
```

---

## 五、并发问题调试

### 5.1 死锁检测

```c
/*
 * 死锁检测：锁顺序验证
 */

#include <pthread.h>
#include <stdio.h>

#define MAX_LOCKS 100

static pthread_mutex_t *lock_order[MAX_LOCKS];
static int lock_count = 0;
static pthread_mutex_t order_mutex = PTHREAD_MUTEX_INITIALIZER;

void lock_with_check(pthread_mutex_t *mutex) {
    pthread_mutex_lock(&order_mutex);

    // 检查是否违反锁顺序
    for (int i = 0; i < lock_count; i++) {
        if (lock_order[i] == mutex) {
            // 重复加锁
            fprintf(stderr, "Warning: recursive lock detected\n");
        }
    }

    // 记录锁顺序
    if (lock_count < MAX_LOCKS) {
        lock_order[lock_count++] = mutex;
    }

    pthread_mutex_unlock(&order_mutex);
    pthread_mutex_lock(mutex);
}

void unlock_with_check(pthread_mutex_t *mutex) {
    pthread_mutex_unlock(mutex);

    pthread_mutex_lock(&order_mutex);
    // 从顺序表中移除
    for (int i = lock_count - 1; i >= 0; i--) {
        if (lock_order[i] == mutex) {
            lock_order[i] = NULL;
            lock_count--;
            break;
        }
    }
    pthread_mutex_unlock(&order_mutex);
}

// 使用
#define LOCK(m) lock_with_check(m)
#define UNLOCK(m) unlock_with_check(m)
```

### 5.2 数据竞争检测

```bash
# 使用ThreadSanitizer
export TSAN_OPTIONS=second_deadlock_stack=1
./program

# 使用Helgrind
valgrind --tool=helgrind ./program

# 使用DRD
valgrind --tool=drd ./program
```

---

## 六、性能问题调试

### 6.1 性能分析流程

```
性能问题诊断：

┌─────────────────────────────────────────────────────────────┐
│  现象: 系统响应慢                                            │
└─────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────┐
│  步骤1: 资源识别                                             │
│  • CPU利用率？                                              │
│  • 内存使用？                                               │
│  • I/O等待？                                                │
│  • 网络延迟？                                               │
└─────────────────────────────────────────────────────────────┘
                            │
              ┌─────────────┼─────────────┐
              ▼             ▼             ▼
        ┌─────────┐   ┌─────────┐   ┌─────────┐
        │CPU瓶颈  │   │内存瓶颈 │   │I/O瓶颈  │
        └────┬────┘   └────┬────┘   └────┬────┘
             │             │             │
             ▼             ▼             ▼
        ┌─────────┐   ┌─────────┐   ┌─────────┐
        │perf top │   │massif   │   │iostat   │
        │火焰图   │   │cachestat│   │iotop    │
        │算法优化 │   │分配器   │   │异步I/O  │
        └─────────┘   └─────────┘   └─────────┘
```

### 6.2 延迟分析

```c
/*
 * 延迟测量：请求全链路追踪
 */

typedef struct {
    const char *name;
    uint64_t start;
    uint64_t end;
} span_t;

#define MAX_SPANS 100

typedef struct {
    span_t spans[MAX_SPANS];
    int count;
    uint64_t trace_id;
} trace_t;

void trace_begin(trace_t *t, const char *name) {
    if (t->count < MAX_SPANS) {
        t->spans[t->count].name = name;
        t->spans[t->count].start = get_ns_timestamp();
    }
}

void trace_end(trace_t *t) {
    if (t->count < MAX_SPANS) {
        t->spans[t->count].end = get_ns_timestamp();
        t->count++;
    }
}

void trace_report(trace_t *t) {
    printf("Trace Report:\n");
    for (int i = 0; i < t->count; i++) {
        uint64_t duration = t->spans[i].end - t->spans[i].start;
        printf("  %s: %lu ns\n", t->spans[i].name, duration);
    }
}

// 使用示例
void process_request(request_t *req) {
    trace_t trace = {0};

    trace_begin(&trace, "parse");
    parse_request(req);
    trace_end(&trace);

    trace_begin(&trace, "auth");
    authenticate(req);
    trace_end(&trace);

    trace_begin(&trace, "handle");
    handle_business(req);
    trace_end(&trace);

    trace_report(&trace);
}
```

---

## 七、调试工具链集成

### 7.1 CI/CD调试集成

```yaml
# .github/workflows/debug-ci.yml
name: Debug Verification

on: [push, pull_request]

jobs:
  static-analysis:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v3

    - name: Clang Static Analyzer
      run: scan-build -o reports make

    - name: Upload reports
      uses: actions/upload-artifact@v3
      with:
        name: static-analysis
        path: reports/

  dynamic-analysis:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v3

    - name: Build with ASan
      run: gcc -fsanitize=address -g *.c -o test_asan

    - name: Run with ASan
      run: ./test_asan

    - name: Build with TSan
      run: gcc -fsanitize=thread -g *.c -o test_tsan

    - name: Run with TSan
      run: ./test_tsan || true
```

---

## 关联导航

### 相关文档

- [Linux运行时调试](../07_Modern_Toolchain/04_Linux_Runtime_Debugging.md)
- [静态分析工具](../07_Modern_Toolchain/05_Static_Analysis_Tools.md)
- [动态分析工具](../07_Modern_Toolchain/06_Dynamic_Analysis_Tools.md)
- [性能追踪技术](../07_Modern_Toolchain/07_Performance_Tracing.md)

### 参考

- Why Programs Fail, Andreas Zeller
- Debugging by Thinking, Robert Charles Metzger
- Systematic Debugging, IBM DeveloperWorks
