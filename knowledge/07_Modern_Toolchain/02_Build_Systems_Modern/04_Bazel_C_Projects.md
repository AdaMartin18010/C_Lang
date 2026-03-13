# Bazel企业级C项目构建

> **层级定位**: 07 Modern Toolchain / 02 Build Systems
> **难度级别**: L4 专家
> **预估学习时间**: 8-12小时

---

## 📋 Bazel简介

Bazel是Google开源的构建工具：

- **可扩展**: 支持超大规模代码库
- **快速**: 增量构建，远程缓存
- **可靠**: 沙盒构建，可重现
- **多语言**: C/C++, Java, Go等

### 适用场景

- 大型代码库
- 多语言项目
- 需要远程缓存/执行
- 企业级项目

---

## 🚀 快速开始

### 安装

```bash
# macOS
brew install bazel

# Linux
npm install -g @bazel/bazelisk

# Windows
choco install bazel
```

### 最小项目

**WORKSPACE** (项目根目录):

```text
workspace(name = "myproject")
```

**BUILD** (src目录):

```python
cc_binary(
    name = "myapp",
    srcs = ["main.c"],
)
```

**构建**:

```bash
bazel build //src:myapp
bazel run //src:myapp
```

---

## 📚 核心概念

### BUILD文件规则

```python
# 可执行文件
cc_binary(
    name = "app",
    srcs = ["main.c", "utils.c"],
    hdrs = ["utils.h"],
    copts = ["-Wall", "-O2"],
    linkopts = ["-lm"],
    deps = [":mylib"],
)

# 静态库
cc_library(
    name = "mylib",
    srcs = ["lib.c"],
    hdrs = ["lib.h"],
    visibility = ["//visibility:public"],
)

# 测试
cc_test(
    name = "lib_test",
    srcs = ["lib_test.c"],
    deps = [":mylib", "@unity//:unity"],
)
```

### 外部依赖

**WORKSPACE**:

```python
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "zlib",
    urls = ["https://zlib.net/zlib-1.2.13.tar.gz"],
    sha256 = "...",
    build_file = "//third_party:zlib.BUILD",
)
```

---

## 🔧 高级特性

### 远程缓存

```bash
# 启用远程缓存
bazel build --remote_cache=grpc://cache.example.com:9092 //...
```

### Bazel配置

**.bazelrc**:

```text
build --jobs=auto
build --copt=-Wall
build --copt=-Werror

test --test_output=errors
test --test_verbose_timeout_warnings
```

---

## 平台特定配置

### 条件编译

```python
# 根据平台选择不同的编译选项
config_setting(
    name = "linux",
    constraint_values = ["@platforms//os:linux"],
)

config_setting(
    name = "macos",
    constraint_values = ["@platforms//os:macos"],
)

cc_library(
    name = "platform_lib",
    srcs = select({
        ":linux": ["linux_impl.c"],
        ":macos": ["macos_impl.c"],
        "//conditions:default": ["generic_impl.c"],
    }),
    hdrs = ["platform.h"],
)
```

### 交叉编译

```python
# .bazelrc 配置交叉编译
build:arm --crosstool_top=//toolchain:arm-linux-gnueabihf
build:arm --cpu=armv7a

# 使用
bazel build --config=arm //src:myapp
```

## 性能优化

### 构建性能最佳实践

```python
# 1. 合理使用 visibility
cc_library(
    name = "internal",
    srcs = ["internal.c"],
    visibility = ["//src:__subpackages__"],
)

# 2. 细粒度目标
cc_library(
    name = "utils",
    srcs = [
        "string_utils.c",
        "math_utils.c",
        "time_utils.c",
    ],
    # 更好的增量构建
)

# 3. 使用 includes 简化头文件路径
cc_library(
    name = "api",
    hdrs = ["include/api.h"],
    includes = ["include"],
)
```

### 查询与分析

```bash
# 查看依赖图
bazel query 'deps(//src:myapp)' --output graph | dot -Tpng > deps.png

# 查看构建时间
bazel build --profile=profile.json //src:myapp
bazel analyze-profile profile.json

# 查找未使用的依赖
bazel query 'rdeps(//..., //src:unused_lib)'
```

## 与 CMake 对比

| 特性 | Bazel | CMake |
|:-----|:------|:------|
| 增量构建 | 精确文件级 | 目标级 |
| 远程缓存 | 内置支持 | 需外部工具 |
| 沙盒构建 | 默认启用 | 需配置 |
| 学习曲线 | 陡峭 | 平缓 |
| IDE支持 | 有限 | 广泛 |
| 多语言 | 原生支持 | 需find_package |

## 实战示例：完整项目

```text
myproject/
├── WORKSPACE
├── .bazelrc
├── .bazelversion
├── src/
│   ├── BUILD
│   ├── main.c
│   ├── utils.c
│   └── utils.h
├── lib/
│   ├── BUILD
│   ├── core.c
│   └── core.h
├── tests/
│   ├── BUILD
│   └── test_core.c
└── third_party/
    ├── BUILD
    └── zlib.BUILD
```

**WORKSPACE**:

```python
workspace(name = "myproject")

# 加载规则
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# 添加 zlib
http_archive(
    name = "zlib",
    urls = ["https://zlib.net/zlib-1.3.tar.gz"],
    sha256 = "...",
    build_file = "//third_party:zlib.BUILD",
)

# 添加 Google Test
http_archive(
    name = "googletest",
    urls = ["https://github.com/google/googletest/archive/v1.14.0.tar.gz"],
    strip_prefix = "googletest-1.14.0",
)
```

**.bazelrc**:

```text
# 构建配置
build --copt=-Wall
build --copt=-Wextra
build --copt=-Werror
build --copt=-std=c17

# 测试配置
test --test_output=errors
test --test_verbose_timeout_warnings

# 性能
build --jobs=auto
fetch --experimental_repository_cache_hardlinks
```

---

## 🏢 Bazel 企业级配置详解

### 1. 自定义规则 (Rules)

**自定义C库规则**:

```python
# rules/my_cc_library.bzl
def _my_cc_library_impl(ctx):
    # 收集源文件
    srcs = ctx.files.srcs
    hdrs = ctx.files.hdrs

    # 编译对象文件
    objects = []
    for src in srcs:
        obj = ctx.actions.declare_file(src.basename + ".o")
        ctx.actions.run(
            outputs = [obj],
            inputs = [src] + hdrs,
            executable = ctx.fragments.cpp.compiler_executable,
            arguments = [
                "-c",
                "-o", obj.path,
                "-I.",
                "-std=c17",
                "-Wall",
                "-Wextra",
                src.path,
            ],
            mnemonic = "CompileC",
            progress_message = "Compiling %s" % src.short_path,
        )
        objects.append(obj)

    # 创建静态库
    library = ctx.actions.declare_file("lib" + ctx.attr.name + ".a")
    ctx.actions.run(
        outputs = [library],
        inputs = objects,
        executable = ctx.fragments.cpp.ar_executable,
        arguments = ["rcs", library.path] + [obj.path for obj in objects],
        mnemonic = "Archive",
        progress_message = "Archiving %s" % library.short_path,
    )

    # 返回提供者
    return [DefaultInfo(files = depset([library]))]

my_cc_library = rule(
    implementation = _my_cc_library_impl,
    attrs = {
        "srcs": attr.label_list(allow_files = [".c"]),
        "hdrs": attr.label_list(allow_files = [".h"]),
    },
    fragments = ["cpp"],
)
```

**代码生成规则**:

```python
# rules/codegen.bzl
def _generate_protobuf_impl(ctx):
    outputs = []
    for src in ctx.files.srcs:
        # 生成.c和.h文件
        c_file = ctx.actions.declare_file(src.basename.replace(".proto", ".pb.c"))
        h_file = ctx.actions.declare_file(src.basename.replace(".proto", ".pb.h"))
        outputs.extend([c_file, h_file])

        ctx.actions.run_shell(
            outputs = [c_file, h_file],
            inputs = [src, ctx.executable._protoc],
            command = "{protoc} --c_out={out_dir} {proto}".format(
                protoc = ctx.executable._protoc.path,
                out_dir = c_file.dirname,
                proto = src.path,
            ),
            progress_message = "Generating protobuf code for %s" % src.short_path,
        )

    return [DefaultInfo(files = depset(outputs))]

generate_protobuf = rule(
    implementation = _generate_protobuf_impl,
    attrs = {
        "srcs": attr.label_list(allow_files = [".proto"]),
        "_protoc": attr.label(
            default = "@com_google_protobuf//:protoc",
            allow_single_file = True,
            executable = True,
            cfg = "host",
        ),
    },
)
```

### 2. 工具链配置 (Toolchains)

**自定义C工具链**:

```python
# toolchain/toolchain_config.bzl
load("@bazel_tools//tools/cpp:cc_toolchain_config_lib.bzl",
     "toolchain_config", "feature", "flag_group", "flag_set", "action_names")

def _impl(ctx):
    tool_paths = [
        tool_path(name = "gcc", path = ctx.attr.compiler_path),
        tool_path(name = "ld", path = ctx.attr.linker_path),
        tool_path(name = "ar", path = ctx.attr.ar_path),
        tool_path(name = "cpp", path = "/bin/false"),
        tool_path(name = "gcov", path = "/bin/false"),
        tool_path(name = "nm", path = "/bin/false"),
        tool_path(name = "objdump", path = "/bin/false"),
        tool_path(name = "strip", path = ctx.attr.strip_path),
    ]

    default_compile_flags = feature(
        name = "default_compile_flags",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = [
                    action_names.c_compile,
                    action_names.cpp_compile,
                ],
                flag_groups = [
                    flag_group(flags = [
                        "-std=c17",
                        "-Wall",
                        "-Wextra",
                        "-Werror",
                        "-fPIC",
                    ]),
                ],
            ),
        ],
    )

    default_link_flags = feature(
        name = "default_link_flags",
        enabled = True,
        flag_sets = [
            flag_set(
                actions = [
                    action_names.cpp_link_executable,
                ],
                flag_groups = [
                    flag_group(flags = [
                        "-lstdc++",
                        "-lm",
                    ]),
                ],
            ),
        ],
    )

    opt_feature = feature(name = "opt")
    dbg_feature = feature(name = "dbg")

    features = [default_compile_flags, default_link_flags, opt_feature, dbg_feature]

    return cc_common.create_cc_toolchain_config_info(
        ctx = ctx,
        toolchain_identifier = ctx.attr.toolchain_identifier,
        host_system_name = "x86_64-unknown-linux-gnu",
        target_system_name = ctx.attr.target_system_name,
        target_cpu = ctx.attr.target_cpu,
        target_libc = "unknown",
        compiler = ctx.attr.compiler,
        abi_version = "unknown",
        abi_libc_version = "unknown",
        tool_paths = tool_paths,
        features = features,
    )

custom_cc_toolchain_config = rule(
    implementation = _impl,
    attrs = {
        "compiler_path": attr.string(mandatory = True),
        "linker_path": attr.string(mandatory = True),
        "ar_path": attr.string(mandatory = True),
        "strip_path": attr.string(mandatory = True),
        "target_system_name": attr.string(mandatory = True),
        "target_cpu": attr.string(mandatory = True),
        "toolchain_identifier": attr.string(mandatory = True),
        "compiler": attr.string(mandatory = True),
    },
    provides = [CcToolchainConfigInfo],
)
```

**工具链BUILD文件**:

```python
# toolchain/BUILD
package(default_visibility = ["//visibility:public"])

load(":toolchain_config.bzl", "custom_cc_toolchain_config")

# ARM交叉编译工具链
custom_cc_toolchain_config(
    name = "arm_toolchain_config",
    compiler_path = "/opt/arm-toolchain/bin/arm-linux-gnueabihf-gcc",
    linker_path = "/opt/arm-toolchain/bin/arm-linux-gnueabihf-ld",
    ar_path = "/opt/arm-toolchain/bin/arm-linux-gnueabihf-ar",
    strip_path = "/opt/arm-toolchain/bin/arm-linux-gnueabihf-strip",
    target_system_name = "arm-linux-gnueabihf",
    target_cpu = "armv7",
    toolchain_identifier = "arm-linux-gnueabihf",
    compiler = "arm-linux-gnueabihf-gcc",
)

cc_toolchain(
    name = "arm_cc_toolchain",
    toolchain_identifier = "arm-linux-gnueabihf",
    toolchain_config = ":arm_toolchain_config",
    all_files = ":empty",
    compiler_files = ":empty",
    dwp_files = ":empty",
    linker_files = ":empty",
    objcopy_files = ":empty",
    strip_files = ":empty",
    supports_param_files = 0,
)

toolchain(
    name = "arm_cc_toolchain_toolchain",
    toolchain = ":arm_cc_toolchain",
    toolchain_type = "@bazel_tools//tools/cpp:toolchain_type",
    exec_compatible_with = [
        "@platforms//cpu:x86_64",
        "@platforms//os:linux",
    ],
    target_compatible_with = [
        "@platforms//cpu:arm",
        "@platforms//os:linux",
    ],
)
```

### 3. 远程缓存与执行

**远程缓存配置** (.bazelrc):

```text
# 远程缓存设置
build --remote_cache=grpc://bazel-cache.company.com:9092
build --remote_upload_local_results=true
build --remote_timeout=3600

# 认证
build --remote_header=x-buildbuddy-api-key=YOUR_API_KEY

# 缓存读取策略
build --experimental_remote_cache_async
build --remote_local_fallback

# 压缩
build --remote_cache_compression
```

**远程执行配置**:

```text
# 远程执行
build --remote_executor=grpc://bazel-exec.company.com:9092
build --remote_default_exec_properties=OSFamily=Linux
build --remote_default_exec_properties=container-image=docker://gcr.io/remote-executor/default:latest

# 执行策略
build --jobs=200
build --remote_local_fallback
build --remote_download_minimal
```

**分布式构建分析**:

```bash
# 启用执行日志
bazel build --execution_log_json_file=exec_log.json //...

# 分析缓存命中率
bazel analyze-profile profile.json --html --html_details
```

### 4. 企业级项目结构

**大型单体仓库 (Monorepo)**:

```text
monorepo/
├── WORKSPACE
├── .bazelrc
├── .bazelversion
├── bazel/
│   ├── common.bazelrc
│   ├── ci.bazelrc
│   ├── user.bazelrc
│   └── tools/
│       ├── buildifier
│       └── format.sh
├── third_party/
│   ├── zlib.BUILD
│   ├── openssl.BUILD
│   └── WORKSPACE.bzl
├── tools/
│   ├── cpp/
│   │   ├── lint.bzl
│   │   └── coverage.bzl
│   └── build_rules/
├── libs/
│   ├── core/
│   │   ├── BUILD
│   │   ├── core.h
│   │   └── core.c
│   ├── network/
│   │   ├── BUILD
│   │   ├── socket.h
│   │   └── socket.c
│   └── storage/
├── services/
│   ├── api/
│   │   ├── BUILD
│   │   └── main.c
│   └── worker/
└── tests/
    ├── integration/
    └── e2e/
```

**模块化WORKSPACE**:

```python
# WORKSPACE
workspace(name = "monorepo")

# 加载依赖管理
load("//third_party:workspace.bzl", "load_dependencies")
load_dependencies()

# 加载工具链
load("//toolchains:workspace.bzl", "register_toolchains")
register_toolchains()
```

```python
# third_party/workspace.bzl
def load_dependencies():
    """加载所有外部依赖"""

    # 基础库
    _zlib()
    _openssl()
    _protobuf()

    # 测试框架
    _googletest()
    _benchmark()

    # 工具
    _buildifier()

def _zlib():
    native.http_archive(
        name = "zlib",
        urls = ["https://zlib.net/zlib-1.3.tar.gz"],
        sha256 = "...",
        build_file = "//third_party:zlib.BUILD",
    )

def _openssl():
    native.http_archive(
        name = "openssl",
        urls = ["https://www.openssl.org/source/openssl-3.0.8.tar.gz"],
        sha256 = "...",
        build_file = "//third_party:openssl.BUILD",
    )

# ... 其他依赖
```

### 5. 质量与合规规则

**代码覆盖率规则**:

```python
# tools/cpp/coverage.bzl
def _cc_coverage_test_impl(ctx):
    # 收集覆盖率数据
    coverage_script = ctx.actions.declare_file(ctx.attr.name + "_coverage.sh")

    ctx.actions.write(
        output = coverage_script,
        content = """
#!/bin/bash
set -e

# 运行测试
{test_binary}

# 收集覆盖率
llvm-profdata merge -sparse default.profraw -o default.profdata
llvm-cov report {test_binary} -instr-profile=default.profdata

# 检查阈值
COVERAGE=$(llvm-cov report {test_binary} -instr-profile=default.profdata | grep TOTAL | awk '{{print $4}}' | sed 's/%//')
if (( $(echo "$COVERAGE < {min_coverage}" | bc -l) )); then
    echo "Coverage $COVERAGE% below minimum {min_coverage}%"
    exit 1
fi
""".format(
            test_binary = ctx.executable.test_binary.short_path,
            min_coverage = ctx.attr.min_coverage,
        ),
        is_executable = True,
    )

    return [DefaultInfo(
        executable = coverage_script,
        runfiles = ctx.runfiles(files = [ctx.executable.test_binary]),
    )]

cc_coverage_test = rule(
    implementation = _cc_coverage_test_impl,
    attrs = {
        "test_binary": attr.label(executable = True, cfg = "target"),
        "min_coverage": attr.int(default = 80),
    },
    executable = True,
)
```

**静态分析集成**:

```python
# tools/cpp/lint.bzl
def _cc_lint_test_impl(ctx):
    script = ctx.actions.declare_file(ctx.attr.name + "_lint.sh")

    srcs = ctx.files.srcs
    src_paths = " ".join([src.short_path for src in srcs])

    ctx.actions.write(
        output = script,
        content = """
#!/bin/bash
set -e

# clang-tidy检查
clang-tidy {srcs} -- {includes} 2>&1 | tee lint_output.txt

# cppcheck检查
cppcheck --enable=all --error-exitcode=1 {srcs}

# 检查clang-tidy是否产生警告
if grep -q "warning:" lint_output.txt; then
    echo "Lint warnings found"
    exit 1
fi
""".format(srcs = src_paths, includes = " ".join(["-I" + inc for inc in ctx.attr.includes])),
        is_executable = True,
    )

    return [DefaultInfo(executable = script)]

cc_lint_test = rule(
    implementation = _cc_lint_test_impl,
    attrs = {
        "srcs": attr.label_list(allow_files = [".c", ".h"]),
        "includes": attr.string_list(),
    },
    executable = True,
)
```

### 6. CI/CD集成

**GitHub Actions配置**:

```yaml
# .github/workflows/bazel-ci.yml
name: Bazel CI

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v4

    - name: Setup Bazel
      uses: bazelbuild/setup-bazelisk@v2

    - name: Configure Bazel
      run: |
        cat > .bazelrc.ci << EOF
        build --remote_cache=https://bazel-cache.company.com
        build --remote_header=authorization=Bearer \${{ secrets.BAZEL_CACHE_TOKEN }}
        build --disk_cache=~/.cache/bazel-disk
        test --test_output=errors
        EOF

    - name: Build
      run: bazel build --config=ci //...

    - name: Test
      run: bazel test --config=ci //...

    - name: Coverage
      run: bazel coverage --combined_report=lcov //...

    - name: Upload Coverage
      uses: codecov/codecov-action@v3
      with:
        files: ./bazel-out/_coverage/_coverage_report.dat
```

**Bazel查询命令**:

```bash
# 查找所有目标
bazel query '//...' --output=label

# 查找依赖
bazel query 'deps(//src:app)' --output=graph

# 查找反向依赖
bazel query 'rdeps(//..., //lib:core)' --output=label

# 查找测试
bazel query 'tests(//...)' --output=label

# 查找包
bazel query 'packages(//...)' --output=package
```

### 7. 性能优化策略

**构建性能最佳实践**:

```text
# .bazelrc性能优化

# 内存优化
startup --host_jvm_args=-Xmx8g
build --local_ram_resources=HOST_RAM*0.8

# 并发控制
build --jobs=auto
fetch --experimental_repository_cache_hardlinks

# 沙盒优化
build --experimental_sandbox_async_tree_creation
build --experimental_sandbox_async_tree_creation_idle_threads=2

# 远程执行优化
build --remote_download_minimal
build --remote_local_fallback

# 分析
build --experimental_profile_include_target_label
build --slim_profile
```

**内存分析**:

```bash
# 内存分析
bazel build --memory_profile=memory.profile //...

# 垃圾回收统计
bazel info used-heap-size-after-gc
```

---

## ✅ 企业级检查清单

- [ ] 自定义规则配置
- [ ] 工具链配置完成
- [ ] 远程缓存启用
- [ ] 远程执行配置
- [ ] CI/CD集成
- [ ] 代码覆盖率检查
- [ ] 静态分析集成
- [ ] 安全扫描配置
- [ ] 构建性能优化
- [ ] 文档和培训

---

**← [返回工具链主页](../README.md)**
