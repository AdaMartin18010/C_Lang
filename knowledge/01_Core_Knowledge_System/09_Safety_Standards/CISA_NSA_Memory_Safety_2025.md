# CISA/NSA 内存安全合规指南 (2024-2025)

> **威胁等级**: 严重 (Critical)
> **适用范围**: 使用 C/C++ 的新产品开发、现有系统维护
> **法规依据**: CISA、NSA、FBI、ACSC 联合公告 (2023-2024)

---

## 1. 政策背景：为什么内存安全成为国家安全议题

### 1.1 政府联合公告时间线

| 日期 | 发布机构 | 文件 | 核心要求 |
|:-----|:---------|:-----|:---------|
| 2022-11 | NSA | 《软件内存安全路线图》 | 推荐使用内存安全语言 |
| 2023-12 | CISA/NSA/FBI 等 | 《内存安全路线图案例研究》 | 提供迁移指南 |
| 2024-02 | CISA/NSA/FBI/ACSC | 《内存安全实践指南》 | 具体技术建议 |
| 2024-09 | CISA | 《产品安全不良实践目录》 | **#1 不良实践: 新用内存不安全语言** |
| 2024-12 | 美国网络安全委员会 | 最终报告 | 建议立法强制内存安全 |

### 1.2 数据驱动的紧迫性

```
┌─────────────────────────────────────────────────────┐
│  约 70% 的安全漏洞与内存安全问题相关 (Microsoft/Google) │
│  约 90% 的 0-day 漏洞利用涉及内存损坏 (Project Zero)    │
│  C/C++ 代码占所有漏洞的 67% (Android 统计)              │
└─────────────────────────────────────────────────────┘
```

CISA 在 2024 年 9 月发布的《产品安全不良实践目录》中明确将以下行为列为**最高风险**：

> **"使用内存不安全的语言（如 C/C++）开发新产品或服务，且不提供内存安全缓解措施"**
>
> — CISA, *Bad Practices Catalog*, 2024-09

---

## 2. 内存安全语言迁移策略

### 2.1 渐进式迁移路线图

```
阶段 1: 边界硬化 (0-6 个月)
    ├── 启用所有编译器安全缓解措施
    ├── 部署 ASan/UBSan 到 CI
    └── 代码审计高危模块
    
阶段 2: 新代码策略 (6-18 个月)
    ├── 新模块优先使用 Rust/Go/Swift
    ├── C↔Rust FFI 边界标准化
    └── 安全关键组件重写
    
阶段 3: 存量迁移 (18-36 个月)
    ├── 高危模块逐步 Rust 化
    ├── 自动化翻译工具 (DARPA TRACTOR)
    └── 保留 C 的模块强化隔离
```

### 2.2 DARPA TRACTOR 项目

**TRACTOR** (Translating All C to Rust) 是 DARPA 2024 年启动的项目：

| 特性 | 说明 |
|:-----|:-----|
| 目标 | 自动将 C 代码翻译为内存安全的 Rust |
| 方法 | AI + 形式化验证结合 |
| 现状 | 原型阶段，适合简单模块 |
| 局限 | 指针算术、 unions、宏翻译仍困难 |

> **结论**: TRACTOR 尚不能用于生产迁移，但值得跟踪。

---

## 3. C 代码的内存安全强化措施

在无法立即迁移的情况下，必须实施以下**最低安全基线**：

### 3.1 编译器缓解措施（必须启用）

```bash
# GCC/Clang 安全编译选项
CFLAGS="\
    -fstack-protector-strong    # 栈溢出保护
    -D_FORTIFY_SOURCE=3         # 缓冲区溢出检测 (GCC 12+)
    -Wformat -Wformat-security  # 格式化字符串检查
    -fPIE -pie                  # 位置无关执行
    -Wl,-z,relro,-z,now         # 重定位只读 + 立即绑定
    -fzero-call-used-regs=used  # 返回前清零寄存器 (GCC 14+)
"
```

### 3.2 Sanitizer 矩阵（CI 必须）

| Sanitizer | 检测问题 | 性能开销 | CI 要求 |
|:----------|:---------|:---------|:--------|
| **AddressSanitizer** | 堆/栈/全局缓冲区溢出、UAF、double-free | 2-3x | 每次提交 |
| **UndefinedBehaviorSanitizer** | 有符号溢出、对齐违规、空指针解引用 | 1.1-1.5x | 每次提交 |
| **MemorySanitizer** | 未初始化内存读取 | 3-5x | 每日构建 |
| **HWAddressSanitizer** | 基于硬件标签的内存错误 | 1.5-2x | ARM64 CI |

### 3.3 静态分析工具链

```bash
# 最低静态分析配置
# 1. 编译器内置警告 (GCC/Clang)
-Wall -Wextra -Wpedantic \
-Wconversion -Wsign-conversion \
-Wnull-dereference -Wdouble-promotion \
-Wformat=2 -Wstrict-overflow=4

# 2. 专用工具
$ clang-tidy --checks='cert-*,misra-*,cppcoreguidelines-*' src/*.c
$ cppcheck --enable=all --std=c11 src/
$ infer run -- make
```

---

## 4. 安全编码规范

### 4.1 高风险 API 禁用清单

```c
// ❌ 禁止使用的函数（无条件不安全）
strcpy(), strcat(), sprintf(), gets()
scanf("%s", buf)   // 无边界检查
strdup()           // 如果无法检查分配结果

// ⚠️ 受限使用（必须验证）
strncpy()          // 不保证 null 终止
memcpy()           // 必须验证 dst/src/len
malloc()           // 必须检查返回值
realloc()          // 必须处理失败时的原指针

// ✅ 推荐替代
strlcpy(), strlcat()           // BSD，保证终止
snprintf()                     // 带长度限制
getline()                      // 动态分配，需检查
strdup_safe()                  // 自定义，检查失败
```

### 4.2 边界检查模式

```c
// 所有数组访问必须经过边界验证
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

// 安全的索引访问宏
#define SAFE_ACCESS(arr, idx, default) \
    (((idx) < ARRAY_SIZE(arr)) ? (arr)[(idx)] : (default))

// 指针运算必须验证范围
void process_buffer(const uint8_t *data, size_t len) {
    if (len < sizeof(header_t)) {
        return;  // 拒绝过短输入
    }
    
    const header_t *hdr = (const header_t *)data;
    size_t payload_len = hdr->payload_length;
    
    // 验证 payload_len 不会溢出
    if (payload_len > len - sizeof(header_t)) {
        return;  // 拒绝畸形输入
    }
    
    const uint8_t *payload = data + sizeof(header_t);
    // 现在可以安全使用 payload[0..payload_len-1]
}
```

### 4.3 内存分配检查模式

```c
// ❌ 错误：不检查分配结果
char *buf = malloc(size);
strcpy(buf, input);  // 可能 NULL 解引用

// ✅ 正确：每次分配必须检查
char *buf = malloc(size);
if (buf == NULL) {
    // 处理错误：记录日志、返回错误码、释放已分配资源
    return ENOMEM;
}

// ✅ 更好：使用分配包装器（统一错误处理）
void *xmalloc(size_t size) {
    void *p = malloc(size);
    if (p == NULL) {
        fatal_error("Out of memory");
    }
    return p;
}
```

---

## 5. 形式化验证与认证

### 5.1 安全关键领域的合规要求

| 领域 | 标准 | 内存安全要求 |
|:-----|:-----|:-------------|
| 汽车 | ISO 26262 (ASIL D) | 无动态内存分配、指针验证 |
| 航空 | DO-178C | MC/DC 覆盖、编码标准 (MISRA) |
| 医疗 | IEC 62304 | 风险管理、静态分析 |
| 核能 | IEC 61508 (SIL 4) | 形式化验证推荐 |

### 5.2 形式化验证工具

```c
// Frama-C + ACSL 示例
/*@
  requires \valid(p) && \valid_read(q);
  requires n >= 0;
  requires \valid(p + (0..n-1));
  requires \valid_read(q + (0..n-1));
  assigns p[0..n-1];
  ensures \forall integer i; 0 <= i < n ==> p[i] == q[i];
*/
void safe_memcpy(uint8_t *p, const uint8_t *q, size_t n) {
    for (size_t i = 0; i < n; i++) {
        p[i] = q[i];
    }
}
```

| 工具 | 能力 | 适用场景 |
|:-----|:-----|:---------|
| **Frama-C/WP** | Hoare 逻辑验证 | 关键函数正确性 |
| **CBMC** | 有界模型检测 | 循环、指针推理 |
| **seL4** | 完整内核验证 | 操作系统级别 |
| **CompCert** | 编译器正确性 | 编译验证 |

---

## 6. 供应链安全：SBOM 与依赖管理

### 6.1 C 项目的供应链风险

C 语言生态的依赖管理历来薄弱：
- 无标准化包管理器（对比 Rust Cargo、Go Modules）
- 广泛使用的库（OpenSSL、zlib）曾出现严重漏洞
-  vendoring 导致版本碎片化

### 6.2 最低合规要求

```yaml
# SBOM 最小信息 (SPDX/CycloneDX)
spdxVersion: "SPDX-2.3"
packages:
  - name: "my-c-project"
    version: "1.0.0"
    dependencies:
      - name: "openssl"
        version: "3.0.8"
        purl: "pkg:generic/openssl@3.0.8"
        checksum: "sha256:..."
    # 必须包含：
    # - 每个依赖的精确版本
    # - 已知漏洞扫描结果
    # - 许可证合规声明
```

### 6.3 C 依赖安全检查工具

```bash
# 1. 漏洞扫描
$ osv-scanner --lockfile=package-lock.json  # 如果有
$ snyk test  --file=Makefile

# 2. 许可证合规
$ fossology  # 开源许可证扫描
$ scancode   # 代码扫描

# 3. 二进制安全
$ checksec --file=./my_binary
```

---

## 7. 检查清单

### 7.1 新产品开发检查清单

- [ ] **语言选择**: 是否评估了内存安全语言（Rust/Go/Swift）？
- [ ] **编译器安全**: 是否启用了 `-D_FORTIFY_SOURCE=3` 和栈保护？
- [ ] **Sanitizer**: CI 是否运行 ASan + UBSan？
- [ ] **静态分析**: 是否集成 clang-tidy / cppcheck？
- [ ] **高危 API**: 是否禁止了 `strcpy`/`sprintf`/`gets`？
- [ ] **模糊测试**: 是否对输入解析器运行 fuzzing？
- [ ] **SBOM**: 是否生成了完整的软件物料清单？
- [ ] **响应计划**: 是否有漏洞披露和补丁流程？

### 7.2 现有 C 代码基线加固检查清单

- [ ] 识别并隔离最高风险模块（输入解析、网络协议）
- [ ] 在所有模块启用编译器安全标志
- [ ] 为所有字符串/缓冲区操作添加边界检查
- [ ] 替换所有高危 API 调用
- [ ] 实现统一的内存分配错误处理
- [ ] 部署持续模糊测试
- [ ] 建立安全审计周期（季度/半年）

---

> **最后更新**: 2026-05-11
> **依据文档**: 
> - CISA, *Bad Practices for Product Security*, 2024-09
> - NSA, *Software Memory Safety Roadmap*, 2022-11
> - CISA/NSA/FBI/ACSC, *Memory Safety Practices Guide*, 2024-02
> - DARPA, *TRACTOR Program*, 2024
