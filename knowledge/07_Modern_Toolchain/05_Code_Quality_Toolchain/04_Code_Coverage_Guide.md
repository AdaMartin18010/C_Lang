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
gcc -fprofile-arcs -ftest-coverage -g main.c -o main
./main
gcov main.c
```

### 生成HTML报告

```bash
# 收集数据
lcov --capture --directory . --output-file coverage.info

# 过滤系统文件
lcov --remove coverage.info '/usr/*' --output-file coverage.info

# 生成HTML
genhtml coverage.info --output-directory coverage_report
```

### CMake集成

```cmake
option(ENABLE_COVERAGE "Enable coverage reporting" OFF)

if(ENABLE_COVERAGE)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage -O0 -g")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()
```

---

## 🚀 CI集成

```yaml
- name: Coverage
  run: |
    cmake -B build -DENABLE_COVERAGE=ON
    cmake --build build
    ctest --test-dir build
    lcov --capture --directory build --output-file coverage.info
    lcov --remove coverage.info '/usr/*' --output-file coverage.info

- name: Upload to Codecov
  uses: codecov/codecov-action@v3
  with:
    files: ./coverage.info
```

---

**← [返回工具链主页](../README.md)**
