# 代码覆盖率指南

> **层级定位**: 07 Modern Toolchain / 05 Code Quality
> **难度级别**: L3 进阶
> **预估学习时间**: 2-3小时

---

## 📋 覆盖率工具

| 工具 | 编译器 | 报告格式 |
|:-----|:-------|:---------|
| **gcov/lcov** | GCC | HTML |
| **llvm-cov** | Clang | HTML/文本 |
| **gcovr** | GCC | XML/HTML/Cobertura |

---

## 🚀 使用gcov/lcov

### 编译带覆盖率

```bash
# 编译
gcc -fprofile-arcs -ftest-coverage -g main.c utils.c -o myapp

# 运行
./myapp

# 生成覆盖率报告
gcov main.c utils.c
```

### 生成HTML报告

```bash
# 收集数据
lcov --capture --directory . --output-file coverage.info

# 过滤系统文件
lcov --remove coverage.info '/usr/*' --output-file coverage.info

# 生成HTML
genhtml coverage.info --output-directory coverage_report

# 打开报告
open coverage_report/index.html
```

### CMake集成

```cmake
option(ENABLE_COVERAGE "Enable coverage reporting" OFF)

if(ENABLE_COVERAGE)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage -O0 -g")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
    endif()
endif()

# 添加覆盖率目标
add_custom_target(coverage
    COMMAND lcov --capture --directory ${CMAKE_BINARY_DIR} --output-file coverage.info
    COMMAND lcov --remove coverage.info '/usr/*' --output-file coverage.info
    COMMAND genhtml coverage.info --output-directory coverage_report
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
```

---

## 🚀 使用llvm-cov (Clang)

```bash
# 编译
clang -fprofile-instr-generate -fcoverage-mapping main.c -o myapp

# 运行
./myapp

# 生成报告
llvm-profdata merge -sparse default.profraw -o default.profdata
llvm-cov report ./myapp -instr-profile=default.profdata

# HTML报告
llvm-cov show ./myapp -instr-profile=default.profdata \
    --format=html -output-dir=coverage_report
```

---

## 🚀 CI集成

### GitHub Actions

```yaml
- name: Build with Coverage
  run: |
    cmake -B build -DENABLE_COVERAGE=ON
    cmake --build build

- name: Run Tests
  run: ctest --test-dir build

- name: Generate Coverage
  run: |
    cd build
    lcov --capture --directory . --output-file coverage.info
    lcov --remove coverage.info '/usr/*' --output-file coverage.info

- name: Upload to Codecov
  uses: codecov/codecov-action@v3
  with:
    files: ./build/coverage.info
    fail_ci_if_error: true
```

### 覆盖率阈值检查

```bash
# 检查覆盖率是否达到80%
lcov --summary coverage.info | grep "lines" | awk '{print $2}' | sed 's/%//' | \
  awk '{if ($1 < 80) exit 1}'
```

---

## 📊 覆盖率指标

| 指标 | 说明 | 目标 |
|:-----|:-----|:----:|
| 行覆盖率 | 执行的代码行比例 | >80% |
| 函数覆盖率 | 执行的函数比例 | >80% |
| 分支覆盖率 | 执行的分支比例 | >70% |
| 条件覆盖率 | 执行的条件比例 | >70% |

---

## ✅ 最佳实践

- [ ] 集成到CI流程
- [ ] 设置覆盖率阈值
- [ ] 定期查看覆盖率报告
- [ ] 关注未覆盖的关键路径
- [ ] 使用lcov过滤无关文件

---

**← [返回工具链主页](../README.md)**
