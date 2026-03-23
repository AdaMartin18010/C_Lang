# 系统化测试方法论

> **层级定位**: 07 Modern Toolchain
> **核心目标**: 建立从单元到集成的完整测试体系
> **难度级别**: L4

---

## 一、测试金字塔理论

### 1.1 经典金字塔

```
                    ┌─────────┐
                    │  E2E测试 │  <- 少而精，用户场景
                    │  (10%)  │     成本高，反馈慢
                   ┌┴─────────┴┐
                   │  集成测试  │  <- 服务边界
                   │  (20%)   │     API契约验证
                  ┌┴───────────┴┐
                  │   单元测试   │  <- 核心，快速反馈
                  │   (70%)    │     函数/类级别
                 ┌┴─────────────┴┐
                 │   静态分析     │  <- 编译时检查
                 │  (100%覆盖)   │
                ┌┴───────────────┴┐
                │    形式化验证    │  <- 关键路径
                │   (选择性)      │
                └─────────────────┘

投资比例建议：
• 单元测试：70%投入，最高ROI
• 集成测试：20%投入，边界验证
• E2E测试：10%投入，用户信心

反模式：冰淇淋筒 (测试主要集中在E2E)
• 反馈慢 (分钟级)
• 不稳定 (flaky)
• 难以定位问题
```

### 1.2 C语言测试特殊性

```
C语言测试挑战：

┌─────────────────────────────────────────────────────────────┐
│  1. 内存管理                                                │
│     • 需要测试malloc失败路径                                 │
│     • 检测内存泄漏                                           │
│     • 验证缓冲区边界                                         │
├─────────────────────────────────────────────────────────────┤
│  2. 未定义行为                                              │
│     • 有符号溢出                                             │
│     • 未初始化变量                                           │
│     • 数据竞争                                               │
│     → 需要Sanitizer配合                                      │
├─────────────────────────────────────────────────────────────┤
│  3. 平台依赖                                                │
│     • 字节序差异                                             │
│     • 对齐要求                                               │
│     • 系统调用差异                                           │
│     → 需要跨平台CI                                           │
├─────────────────────────────────────────────────────────────┤
│  4. 无运行时类型信息                                        │
│     • 难以自动Mock                                           │
│     • 需要手动设计接口                                       │
│     • 依赖注入复杂                                           │
└─────────────────────────────────────────────────────────────┘
```

---

## 二、单元测试框架

### 2.1 测试设计原则

```c
/*
 * AAA模式：Arrange-Act-Assert
 */

void test_queue_push_pop(void) {
    // Arrange (准备)
    queue_t *q = queue_create(10);
    int item = 42;

    // Act (执行)
    queue_push(q, &item);
    int *result = queue_pop(q);

    // Assert (验证)
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL(42, *result);

    // Cleanup (清理)
    queue_destroy(q);
}

/*
 * FIRST原则：
 * F - Fast: 快速执行 (<10ms)
 * I - Independent: 测试间无依赖
 * R - Repeatable: 任何环境结果一致
 * S - Self-validating: 明确Pass/Fail
 * T - Timely: 与代码同时编写
 */

/*
 * 边界条件测试 (Right-BICEP):
 * Right: 正常情况
 * B - Boundary: 边界值
 * I - Inverse: 反向操作
 * C - Cross-check: 交叉验证
 * E - Error: 错误条件
 * P - Performance: 性能特征
 */

void test_queue_boundary(void) {
    queue_t *q = queue_create(2);
    int items[] = {1, 2, 3};

    // Boundary: 满队列
    TEST_ASSERT_TRUE(queue_push(q, &items[0]));
    TEST_ASSERT_TRUE(queue_push(q, &items[1]));
    TEST_ASSERT_FALSE(queue_push(q, &items[2]));  // 应该失败

    // Boundary: 空队列
    queue_pop(q);
    queue_pop(q);
    TEST_ASSERT_NULL(queue_pop(q));  // 空队列pop

    queue_destroy(q);
}
```

### 2.2 Mock与依赖注入

```c
/*
 * 接口设计便于测试：
 */

// 可替换的文件系统接口
typedef struct {
    int (*open)(const char *path, int flags);
    ssize_t (*read)(int fd, void *buf, size_t count);
    int (*close)(int fd);
} fs_ops_t;

// 生产实现
extern const fs_ops_t real_fs;

// 被测函数接受接口
int process_file(const char *path, fs_ops_t *fs) {
    int fd = fs->open(path, O_RDONLY);
    if (fd < 0) return -1;

    char buf[1024];
    ssize_t n = fs->read(fd, buf, sizeof(buf));
    fs->close(fd);

    return process_buffer(buf, n);
}

// 测试用Mock
typedef struct {
    const char *expected_path;
    const char *mock_data;
    size_t mock_len;
    int open_called;
    int read_called;
    int close_called;
} mock_fs_state_t;

int mock_open(const char *path, int flags) {
    mock_state.open_called++;
    if (strcmp(path, mock_state.expected_path) == 0) {
        return 42;  // 模拟fd
    }
    return -1;
}

void test_process_file(void) {
    // 设置Mock状态
    mock_state = (mock_fs_state_t){
        .expected_path = "/tmp/test.txt",
        .mock_data = "test data",
        .mock_len = 9
    };

    fs_ops_t mock_fs = {
        .open = mock_open,
        .read = mock_read,
        .close = mock_close
    };

    // 测试
    int result = process_file("/tmp/test.txt", &mock_fs);

    // 验证
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(1, mock_state.open_called);
    TEST_ASSERT_EQUAL(1, mock_state.read_called);
    TEST_ASSERT_EQUAL(1, mock_state.close_called);
}
```

---

## 三、测试覆盖率

### 3.1 覆盖率类型

```
┌─────────────────────────────────────────────────────────────┐
│  行覆盖率 (Line Coverage)                                    │
│  • 最基本的度量                                              │
│  • 容易被游戏化 (删除代码提高比例)                           │
│  • 目标: 80-90%                                              │
├─────────────────────────────────────────────────────────────┤
│  分支覆盖率 (Branch Coverage)                                │
│  • 每个if/else分支都要执行                                   │
│  • 比行覆盖率更严格                                          │
│  • 目标: 80%+                                                │
├─────────────────────────────────────────────────────────────┤
│  条件覆盖率 (Condition Coverage)                             │
│  • 复合条件的每个子条件                                      │
│  • if (a && b): 需要TT, TF, FT                               │
│  • MC/DC: 更严格的航空标准                                   │
├─────────────────────────────────────────────────────────────┤
│  函数覆盖率 (Function Coverage)                              │
│  • 每个函数至少被调用一次                                    │
│  • 最低要求，通常100%                                        │
├─────────────────────────────────────────────────────────────┤
│  路径覆盖率 (Path Coverage)                                  │
│  • 所有可能的执行路径                                        │
│  • 理论上理想，实践中不可行                                  │
│  • 循环限制展开次数                                          │
└─────────────────────────────────────────────────────────────┘

// 示例：条件覆盖的重要性
void process(int a, int b) {
    if (a > 0 && b > 0) {  // 需要4种组合
        // ...
    }
}

// 仅测试 (a=1,b=1) 达到行覆盖
// 需要额外测试：
// (a=1,b=-1) - a真b假
// (a=-1,b=1) - a假b真
// (a=-1,b=-1) - 都假
// 才能达到条件覆盖
```

### 3.2 gcov使用

```bash
# 编译带覆盖率
gcc -fprofile-arcs -ftest-coverage -g program.c -o program

# 运行测试
./program

# 生成报告
gcov program.c

# HTML报告
gcovr -r . --html --html-details -o coverage.html

# 排除系统头文件
gcovr -r . --exclude='.*/test/.*' --exclude='.*/usr/.*'

# CI集成阈值
gcovr -r . --fail-under-line=80 --fail-under-branch=75
```

---

## 四、模糊测试 (Fuzzing)

### 4.1 模糊测试原理

```
模糊测试 = 自动化生成输入 + 监控异常

┌─────────────────────────────────────────────────────────────┐
│  1. 基于变异的模糊测试                                       │
│     • 从种子输入开始                                         │
│     • 随机变异 (位翻转、算术、字典替换)                      │
│     • 保留触发新路径的输入                                   │
│     • 代表: AFL, libFuzzer                                 │
├─────────────────────────────────────────────────────────────┤
│  2. 基于生成的模糊测试                                       │
│     • 从语法/协议规范生成                                    │
│     • 确保语法正确性                                         │
│     • 代表: Peach, Grammarinator                           │
├─────────────────────────────────────────────────────────────┤
│  3. 覆盖引导模糊测试                                         │
│     • 编译时插桩收集覆盖率                                   │
│     • 优先探索未覆盖代码                                     │
│     • 反馈驱动的进化                                         │
│     • 当前最有效的方法                                       │
└─────────────────────────────────────────────────────────────┘
```

### 4.2 libFuzzer集成

```c
// 待测函数
int parse_header(const uint8_t *data, size_t len, header_t *out);

// Fuzzer目标函数
int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    header_t hdr;
    parse_header(data, size, &hdr);
    return 0;
}

// 编译
clang -g -O1 -fsanitize=fuzzer,address \
    parse.c fuzz_target.c -o fuzzer

// 运行
./fuzzer corpus/ -max_len=1024 -jobs=4

// 字典辅助
// headers.dict:
kwd("GET")
kwd("POST")
kwd("HTTP/1.1")
kwd("Content-Length")

./fuzzer corpus/ -dict=headers.dict

// 结构化输入 (Custom Mutator)
typedef struct {
    uint32_t magic;
    uint16_t version;
    uint32_t data_len;
    uint8_t data[];
} packet_t;

// 确保变异产生结构有效输入
extern "C" size_t LLVMFuzzerCustomMutator(
    uint8_t *Data, size_t Size,
    size_t MaxSize, unsigned int Seed) {
    // 解析-变异-序列化
    packet_t *pkt = deserialize(Data, Size);
    mutate_packet(pkt);
    return serialize(pkt, Data, MaxSize);
}
```

---

## 五、CI/CD中的测试

```yaml
# .github/workflows/test.yml
name: Comprehensive Testing

on: [push, pull_request]

jobs:
  unit-tests:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v3

    - name: Build with coverage
      run: |
        mkdir build && cd build
        cmake -DCMAKE_BUILD_TYPE=Debug \
              -DENABLE_COVERAGE=ON ..
        make -j

    - name: Run tests
      run: |
        cd build
        ctest --output-on-failure -j4

    - name: Coverage report
      run: |
        gcovr -r . --fail-under-line=80

    - name: Upload coverage
      uses: codecov/codecov-action@v3

  sanitizer-tests:
    runs-on: ubuntu-latest
    strategy:
      matrix:
        sanitizer: [address, thread, undefined]
    steps:
    - uses: actions/checkout@v3

    - name: Build with ${{ matrix.sanitizer }}
      run: |
        mkdir build && cd build
        cmake -DSANITIZER=${{ matrix.sanitizer }} ..
        make -j

    - name: Run tests
      run: |
        cd build
        ctest --output-on-failure

  fuzz-testing:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v3

    - name: Build fuzzer
      run: clang -fsanitize=fuzzer,address fuzz.c -o fuzzer

    - name: Run fuzzer (5 minutes)
      run: |
        mkdir corpus
        timeout 300 ./fuzzer corpus/ -max_total_time=280 || true

    - name: Upload crashes
      if: failure()
      uses: actions/upload-artifact@v3
      with:
        name: crashes
        path: crash-*
```

---

## 关联导航

- [静态分析原理](./08_Static_Analysis_Principles.md)
- [动态分析原理](./09_Dynamic_Analysis_Principles.md)
- [调试原理](./10_Debugging_Principles_and_Framework.md)

## 参考

- xUnit Test Patterns, Gerard Meszaros
- Growing Object-Oriented Software, Freeman & Pryce
- The Art of Software Testing, Myers et al.
