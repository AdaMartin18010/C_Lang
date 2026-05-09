# OpenSSL 3.0 生命周期终止 (EOL) 迁移指南

> **文档版本**: 1.0.0
> **最后更新**: 2026-05-10
> **关键日期**: OpenSSL 3.0 EOL — 2026年9月

---

## 时间线

| 版本 | 发布日期 | 支持状态 | 建议操作 |
|------|----------|----------|----------|
| OpenSSL 3.0 | 2021年9月 | **2026年9月 EOL** | **立即迁移** |
| OpenSSL 3.1 | 2023年3月 | 已EOL | 已过期 |
| OpenSSL 3.2 | 2023年11月 | 已EOL | 已过期 |
| OpenSSL 3.3 | 2024年4月 | 已EOL | 已过期 |
| OpenSSL 3.4 | 2024年10月 | 活跃 | 短期过渡 |
| OpenSSL 3.5 | 2025年4月 | **LTS** | 推荐目标 |
| OpenSSL 3.6 | 2025年10月 | 活跃 | 新特性 |
| OpenSSL 4.0 | 预计2026年4月 | 开发中 | 关注发布 |

---

## C语言兼容性要求

OpenSSL 3.6.0起，最低编译器要求从**ANSI C (C89)** 提升为 **C99**：

```c
/* OpenSSL 3.6+ 内部使用的C99特性 */
#include <stdint.h>   /* 固定宽度整数 */
#include <stdbool.h>  /* bool类型 */

/* 变长数组（VLA）在某些模块中使用 */
void process_blocks(size_t n) {
    uint8_t buffer[n];  /* VLA - C99特性 */
    // ...
}
```

> ⚠️ 使用旧编译器（如仅支持C89的嵌入式工具链）的项目需要升级工具链。

---

## 迁移检查清单

### 1. 版本检测

```c
#include <openssl/opensslv.h>

#if OPENSSL_VERSION_MAJOR < 3 || \
    (OPENSSL_VERSION_MAJOR == 3 && OPENSSL_VERSION_MINOR < 5)
#   error "OpenSSL 3.5+ required for LTS support"
#endif
```

### 2. 已废弃API替换

| 废弃API (3.0) | 替代方案 (3.5+) | 说明 |
|--------------|----------------|------|
| `EVP_MD_CTX_md()` | `EVP_MD_CTX_get0_md()` | 获取摘要算法 |
| `EVP_CIPHER_CTX_cipher()` | `EVP_CIPHER_CTX_get0_cipher()` | 获取加密算法 |
| `DH_*` / `DSA_*` | `EVP_PKEY_*` | 弃用DH/DSA直接API |

### 3. 构建系统更新

```bash
# 旧配置（3.0）
./config --prefix=/usr/local/openssl-3.0

# 新配置（3.5 LTS）
./config --prefix=/usr/local/openssl-3.5 \
         --release \
         enable-fips
```

---

## 对C项目的影响

### 嵌入式系统

- 资源受限设备可能需要评估wolfSSL或mbed TLS作为替代
- OpenSSL 3.5的FIPS模块对医疗设备、政府系统有意义

### 容器化部署

```dockerfile
# 使用Alpine Linux + OpenSSL 3.5
FROM alpine:3.22
RUN apk add --no-cache openssl-dev>=3.5
```

---

## 参考

- [OpenSSL Release Strategy](https://www.openssl.org/policies/releasestrat.html)
- [OpenSSL 3.5 Changelog](https://www.openssl.org/news/changelog.html)
- [C23 Default Standard Migration](C23_Default_Standard_Migration_Guide.md)
