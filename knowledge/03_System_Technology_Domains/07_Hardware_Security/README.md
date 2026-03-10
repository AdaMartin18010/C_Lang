# 硬件安全：TPM、安全元件与HSM

> **层级定位**: 03 System Technology Domains / 07 Hardware Security
> **学习路径**: 从可信计算到企业级密钥管理

---

## 📖 技术概述

本目录深入探讨**硬件安全**核心技术——TPM 2.0可信平台模块、安全元件(SE)和硬件安全模块(HSM)。
这些技术构成了现代系统安全的基础设施，广泛应用于安全启动、磁盘加密、身份认证和企业级密钥管理。

### 核心知识点

| 主题 | 描述 | 应用场景 |
|:-----|:-----|:---------|
| TPM 2.0 | 可信平台模块，提供硬件级安全功能 | BitLocker、Secure Boot、远程证明 |
| 密钥密封 | 将数据与平台状态(PCR)绑定 | 磁盘加密密钥保护 |
| 安全元件(SE) | 防篡改硬件，密钥永不离开芯片 | 移动支付、物联网、身份卡 |
| HSM | 企业级密钥管理和加密运算 | PKI、数字签名、金融交易 |

### 安全层级对比

| 特性 | TPM | SE | HSM |
|:-----|:----|:---|:----|
| 安全级别 | 中等 | 高 | 最高 |
| 成本 | 低(集成) | 中 | 高 |
| 性能 | 一般 | 受限 | 高(专用硬件) |
| 适用场景 | 个人设备 | 移动/IoT | 企业/金融 |
| 标准 | TCG | GlobalPlatform | PKCS#11 |

---

## 📁 文件清单

| 文件名 | 内容描述 | 行数 | 难度 |
|:-------|:---------|:-----|:-----|
| [01_TPM2_TSS.md](./01_TPM2_TSS.md) | TPM 2.0可信软件栈：命令结构、PCR操作、密钥管理、HMAC/签名 | ~562行 | L4分析 |
| [02_Key_Sealing.md](./02_Key_Sealing.md) | 密钥密封与解封：PCR策略、授权会话、PolicyPCR、复合策略 | ~629行 | L4分析 |
| [02_Secure_Element.md](./02_Secure_Element.md) | 安全元件集成：GlobalPlatform、APDU协议、PC/SC接口、安全通道 | ~753行 | L4分析 |
| [03_HSM_Integration.md](./03_HSM_Integration.md) | HSM集成：PKCS#11标准、密钥生成、加解密、数字签名 | ~1000+行 | L4分析 |

### 01_TPM2_TSS.md 详细内容

- **TPM 2.0架构**：FAPI/ESAPI/SAPI/TCTI层次结构
- **命令结构**：TPM_CC命令码、TPM_ST标签、TPM_RC响应码
- **命令封包**：大端序序列化、缓冲区构造、响应解析
- **核心操作**：
  - GetRandom：获取硬件随机数
  - PCR_Read/Extend：平台配置寄存器操作
  - Create/Load：密钥创建与加载
  - HMAC/Sign：认证与签名
- **密钥管理**：密钥层次、持久对象、句柄生命周期
- **授权机制**：密码授权、HMAC会话

### 02_Key_Sealing.md 详细内容

- **密封类型对比**：Seal(PCR绑定) vs Wrap(仅父密钥) vs Bind(外部密钥)
- **PCR策略管理**：
  - PCR读取与摘要计算
  - 策略会话建立(StartAuthSession)
  - PolicyPCR设置
  - PolicyPassword设置
  - PolicyGetDigest获取策略摘要
- **密封操作**：TPM2_Create敏感区域构造、公共区域模板
- **解封操作**：TPM2_Unseal、授权验证、PCR匹配检查
- **高级策略**：OR/AND复合策略、PolicyAuthorize、物理存在策略
- **安全考虑**：PCR值变化后的恢复策略、策略摘要不匹配处理

### 02_Secure_Element.md 详细内容

- **GlobalPlatform标准**：
  - 安全架构：ISD/SSD/CRE层次
  - 应用生命周期：安装、选择、删除
  - 安全通道：SCP02/SCP03
- **APDU协议**：
  - C-APDU格式：CLA/INS/P1/P2/Lc/Data/Le
  - R-APDU格式：Data/SW1/SW2
  - T=0/T=1传输协议
  - 常见状态字解析(0x9000、0x6A82、0x6D00等)
- **PC/SC接口**：
  - 架构：应用层→资源管理器→IFD驱动→读卡器→卡片
  - API：SCardEstablishContext、SCardConnect、SCardTransmit
  - ATR解析：复位应答分析
- **安全操作**：
  - SELECT AID选择应用
  - VERIFY PIN验证
  - READ BINARY读取数据
  - 安全报文(Secure Messaging)

### 03_HSM_Integration.md 详细内容

- **HSM架构**：本地HSM、云HSM、网络HSM部署模式
- **PKCS#11标准**：
  - 核心概念：Slot/Token/Session/Object
  - 数据类型：CK_SESSION_HANDLE、CK_OBJECT_HANDLE、CK_ATTRIBUTE
  - 返回值：CKR_*错误码详解
  - 机制类型：CKM_*算法标识
- **Cryptoki编程**：
  - 库加载与初始化(C_Initialize)
  - 插槽枚举与令牌信息获取
  - 会话管理：RW/RO会话、登录状态
- **密钥管理**：
  - AES密钥生成(C_GenerateKey)
  - RSA密钥对生成(C_GenerateKeyPair)
  - 密钥属性：敏感、不可导出、可提取
  - 密钥查找(C_FindObjects)与删除
- **加密运算**：
  - AES ECB/CBC/GCM加密解密
  - RSA PKCS#1 v1.5/OAEP加密
  - RSA签名与验签
- **错误处理**：PKCS#11错误码映射、调试技巧

---

## 🎯 学习目标

完成本目录学习后，你将能够：

1. **TPM开发**：使用TSS开发安全启动、远程证明应用
2. **密钥密封**：设计基于PCR的磁盘加密方案
3. **SE集成**：开发智能卡/安全元件应用，实现安全通信
4. **HSM对接**：通过PKCS#11接口集成企业级加密服务
5. **安全架构**：根据威胁模型选择合适的安全硬件

---

## 📚 学习资源

### 官方规范

| 文档 | 组织 | 说明 |
|:-----|:-----|:-----|
| TPM 2.0 Library Specification Part 1-4 | TCG | 可信计算组官方规范 |
| TSS 2.0 Specification | TCG | 可信软件栈规范 |
| GlobalPlatform Card Specification | GP | 安全元件标准 |
| PKCS#11 Cryptographic Token Interface | OASIS | HSM接口标准 |

### 工具与模拟器

| 工具 | 用途 |
|:-----|:-----|
| IBM TPM 2.0 Simulator | 软件TPM模拟器 |
| tpm2-tss | 开源TSS实现 |
| OpenSC | 智能卡工具集 |
| SoftHSM | 软件HSM模拟器 |

### 书籍与教程

| 资源 | 说明 |
|:-----|:-----|
| *A Practical Guide to TPM 2.0* | TPM开发实用指南 |
| Microsoft BitLocker文档 | 密封技术实际应用 |
| *Understanding Cryptography* | 密码学基础 |

---

## 🔗 知识关联

### 前置知识

- 密码学基础(对称加密、非对称加密、哈希)
- C语言系统编程
- 基础安全概念(认证、授权、审计)

### 后续延伸

- **可信计算**：远程证明、安全启动链、TEE(可信执行环境)
- **密钥管理**：KMS、密钥轮换、密钥托管
- **合规认证**：FIPS 140-2、Common Criteria
- **量子安全**：后量子密码、抗量子算法

### 横向关联

| 关联目录 | 关联内容 |
|:---------|:---------|
| `../11_In_Memory_Database/` | 加密存储、密钥管理 |
| `../../04_Industrial_Scenarios/` | 工业控制系统安全 |

---

## ⚠️ 学习建议

### 常见陷阱

1. **安全与便利的权衡**：过度安全导致用户体验差，需要合理设计
2. **密钥生命周期管理**：生成、分发、使用、轮换、销毁全流程需规划
3. **错误处理**：安全操作失败后的回退策略
4. **性能影响**：硬件加密可能引入延迟，需要异步处理

### 实践建议

- 使用模拟器进行开发测试，避免损坏真实硬件
- 建立完整的审计日志，追踪所有敏感操作
- 定期进行安全评估和渗透测试
- 关注TCG、GlobalPlatform等组织的最新规范更新

---

## 📈 进阶路径

```
本目录 (硬件安全基础)
    ↓
可信执行环境 (Intel SGX、ARM TrustZone)
    ↓
企业级密钥管理 (KMS、HSM集群)
    ↓
零信任架构 (设备认证、持续验证)
    ↓
后量子密码迁移 (抗量子算法)
```

---

## 📝 实践项目建议

### 项目1：安全启动演示

实现一个简化版安全启动链：

- 使用TPM PCR记录启动度量
- 验证每个引导组件的完整性
- 密钥密封保护磁盘加密密钥

### 项目2：智能卡应用

开发一个基于JavaCard的电子钱包应用：

- 余额查询和充值功能
- PIN码认证机制
- 安全报文保护交易

### 项目3：PKI密钥管理系统

构建基于HSM的企业密钥管理方案：

- 证书生命周期管理
- 自动密钥轮换
- 审计日志和合规报告

---

> [← 返回上级目录](../README.md)
> [← 上一主题：内存数据库](../11_In_Memory_Database/README.md)
> [→ 进入工业应用场景](../../04_Industrial_Scenarios/README.md)
