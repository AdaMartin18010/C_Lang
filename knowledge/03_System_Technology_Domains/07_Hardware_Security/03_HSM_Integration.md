# 硬件安全模块(HSM)集成


---

## 📑 目录

- [硬件安全模块(HSM)集成](#硬件安全模块hsm集成)
  - [📑 目录](#-目录)
  - [1. HSM架构与概述](#1-hsm架构与概述)
    - [1.1 什么是HSM](#11-什么是hsm)
    - [1.2 HSM架构](#12-hsm架构)
    - [1.3 HSM部署模式](#13-hsm部署模式)
  - [2. PKCS#11标准接口](#2-pkcs11标准接口)
    - [2.1 PKCS#11概述](#21-pkcs11概述)
    - [2.2 PKCS#11架构](#22-pkcs11架构)
    - [2.3 PKCS#11数据类型](#23-pkcs11数据类型)
  - [3. PKCS#11 C语言编程](#3-pkcs11-c语言编程)
    - [3.1 HSM连接与会话管理](#31-hsm连接与会话管理)
    - [3.2 密钥生成与管理](#32-密钥生成与管理)
    - [3.3 加解密与签名操作](#33-加解密与签名操作)
  - [4. OpenSSL引擎与HSM集成](#4-openssl引擎与hsm集成)
    - [4.1 OpenSSL引擎架构](#41-openssl引擎架构)
    - [4.2 使用示例](#42-使用示例)
  - [5. 密钥生命周期管理](#5-密钥生命周期管理)
    - [5.1 密钥管理流程](#51-密钥管理流程)
    - [5.2 密钥轮换策略](#52-密钥轮换策略)
  - [6. 总结](#6-总结)


---

## 1. HSM架构与概述

### 1.1 什么是HSM

硬件安全模块(Hardware Security Module, HSM)是一种专用硬件设备，用于保护和管理数字密钥，同时提供加密运算服务。
HSM设计为防篡改和抗侧信道攻击，确保密钥在设备内部生成、存储和使用，永不在明文状态下离开设备。

**HSM的主要功能：**

| 功能 | 描述 |
|------|------|
| 密钥生成 | 在安全边界内生成高质量随机密钥 |
| 密钥存储 | 安全存储密钥，防提取 |
| 加密运算 | 执行AES、RSA、ECC等算法 |
| 数字签名 | 创建和验证数字签名 |
| 证书管理 | 存储和管理数字证书 |

### 1.2 HSM架构

```text
┌─────────────────────────────────────────────────────────┐
│                      主机应用程序                         │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐     │
│  │   OpenSSL   │  │    JCE      │  │   PKCS#11   │     │
│  │    引擎     │  │   Provider  │  │    API      │     │
│  └──────┬──────┘  └──────┬──────┘  └──────┬──────┘     │
├─────────┴────────────────┴────────────────┴────────────┤
│                    PKCS#11 / CSP 接口层                  │
├─────────────────────────────────────────────────────────┤
│  ┌─────────────────────────────────────────────────┐   │
│  │              硬件安全模块 (HSM)                   │   │
│  │  ┌─────────────┐    ┌───────────────────────┐   │   │
│  │  │  密码处理器  │    │     密钥存储区         │   │   │
│  │  │  (AES/RSA)  │    │ ┌─────┬─────┬─────┐   │   │   │
│  │  └─────────────┘    │ │密钥1│密钥2│密钥3│   │   │   │
│  │  ┌─────────────┐    │ └─────┴─────┴─────┘   │   │   │
│  │  │  真随机数   │    │  (防篡改安全存储)      │   │   │
│  │  │  生成器(TRNG)│    └───────────────────────┘   │   │
│  │  └─────────────┘                               │   │
│  │  ┌─────────────┐    ┌───────────────────────┐   │   │
│  │  │  安全MCU    │    │      审计日志          │   │   │
│  │  │  (固件)     │    └───────────────────────┘   │   │
│  │  └─────────────┘                               │   │
│  └─────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────┘
```

### 1.3 HSM部署模式

**本地HSM (On-premise)**

- 物理设备部署在数据中心
- 提供最高级别的物理安全
- 示例：Thales Luna, Utimaco CryptoServer

**云HSM (Cloud HSM)**

- 云服务提供商托管
- 便于扩展和按需付费
- 示例：AWS CloudHSM, Azure Dedicated HSM

**网络HSM (Network HSM)**

- 通过网络提供服务
- 多客户端共享访问
- 通常支持PKCS#11、REST API

---

## 2. PKCS#11标准接口

### 2.1 PKCS#11概述

PKCS#11 (Public-Key Cryptography Standards #11) 是RSA实验室制定的标准，定义了加密令牌接口(Cryptoki)，用于与HSM、智能卡等加密设备通信。

**PKCS#11核心概念：**

- **Slot**：插槽，代表一个物理或逻辑设备
- **Token**：令牌，即加密设备本身
- **Session**：会话，与令牌的逻辑连接
- **Object**：对象，包括密钥、证书、数据等

### 2.2 PKCS#11架构

```text
┌────────────────────────────────────┐
│         应用程序 (Application)       │
│   ┌──────────────────────────┐     │
│   │      Cryptoki API        │     │
│   │  C_Initialize, C_Login   │     │
│   │  C_GenerateKey, C_Encrypt │    │
│   └───────────┬──────────────┘     │
├───────────────┴────────────────────┤
│      PKCS#11 Library (pkcs11.dll)  │
│   ┌──────────────────────────┐     │
│   │    设备特定实现          │     │
│   └───────────┬──────────────┘     │
├───────────────┴────────────────────┤
│         硬件驱动层                  │
├───────────────┬────────────────────┤
│   ┌───────────┴──────────┐         │
│   │      HSM设备          │         │
│   └──────────────────────┘         │
└────────────────────────────────────┘
```

### 2.3 PKCS#11数据类型

```c
/* pkcs11_types.h - PKCS#11常用数据类型 */
#ifndef PKCS11_TYPES_H
#define PKCS11_TYPES_H

#include <stdint.h>
#include <stddef.h>

/* 基本类型定义 */
typedef unsigned char     CK_BYTE;
typedef CK_BYTE           CK_CHAR;
typedef CK_BYTE           CK_UTF8CHAR;
typedef CK_BYTE           CK_BBOOL;
typedef unsigned long     CK_ULONG;
typedef long              CK_LONG;
typedef CK_ULONG          CK_FLAGS;
typedef CK_BYTE*          CK_BYTE_PTR;
typedef CK_CHAR*          CK_CHAR_PTR;
typedef CK_UTF8CHAR*      CK_UTF8CHAR_PTR;
typedef CK_ULONG*         CK_ULONG_PTR;
typedef void*             CK_VOID_PTR;

/* Cryptoki类型 */
typedef CK_VOID_PTR       CK_SESSION_HANDLE;
typedef CK_ULONG          CK_OBJECT_HANDLE;
typedef CK_ULONG          CK_SLOT_ID;
typedef CK_ULONG          CK_MECHANISM_TYPE;
typedef CK_ULONG          CK_RV;
typedef CK_ULONG          CK_ATTRIBUTE_TYPE;
typedef CK_ULONG          CK_KEY_TYPE;
typedef CK_ULONG          CK_OBJECT_CLASS;

/* 属性结构 */
typedef struct CK_ATTRIBUTE {
    CK_ATTRIBUTE_TYPE type;
    CK_VOID_PTR       pValue;
    CK_ULONG          ulValueLen;
} CK_ATTRIBUTE;

/* 机制结构 */
typedef struct CK_MECHANISM {
    CK_MECHANISM_TYPE mechanism;
    CK_VOID_PTR       pParameter;
    CK_ULONG          ulParameterLen;
} CK_MECHANISM;

/* 常用返回值 */
#define CKR_OK                    0x00000000
#define CKR_CANCEL                0x00000001
#define CKR_HOST_MEMORY           0x00000002
#define CKR_SLOT_ID_INVALID       0x00000003
#define CKR_GENERAL_ERROR         0x00000005
#define CKR_FUNCTION_FAILED       0x00000006
#define CKR_ARGUMENTS_BAD         0x00000007
#define CKR_NO_EVENT              0x00000008
#define CKR_NEED_TO_CREATE_THREADS 0x00000009
#define CKR_CANT_LOCK             0x0000000A
#define CKR_ATTRIBUTE_READ_ONLY   0x00000010
#define CKR_ATTRIBUTE_SENSITIVE   0x00000011
#define CKR_ATTRIBUTE_TYPE_INVALID 0x00000012
#define CKR_ATTRIBUTE_VALUE_INVALID 0x00000013
#define CKR_DATA_INVALID          0x00000020
#define CKR_DATA_LEN_RANGE        0x00000021
#define CKR_DEVICE_ERROR          0x00000030
#define CKR_DEVICE_MEMORY         0x00000031
#define CKR_DEVICE_REMOVED        0x00000032
#define CKR_ENCRYPTED_DATA_INVALID 0x00000040
#define CKR_ENCRYPTED_DATA_LEN_RANGE 0x00000041
#define CKR_FUNCTION_CANCELED     0x00000050
#define CKR_FUNCTION_NOT_PARALLEL 0x00000051
#define CKR_FUNCTION_NOT_SUPPORTED 0x00000054
#define CKR_KEY_HANDLE_INVALID    0x00000060
#define CKR_KEY_SIZE_RANGE        0x00000062
#define CKR_KEY_TYPE_INCONSISTENT 0x00000063
#define CKR_KEY_NOT_NEEDED        0x00000064
#define CKR_KEY_CHANGED           0x00000065
#define CKR_KEY_NEEDED            0x00000066
#define CKR_KEY_INDIGESTIBLE      0x00000067
#define CKR_KEY_FUNCTION_NOT_PERMITTED 0x00000068
#define CKR_KEY_NOT_WRAPPABLE     0x00000069
#define CKR_KEY_UNEXTRACTABLE     0x0000006A
#define CKR_MECHANISM_INVALID     0x00000070
#define CKR_MECHANISM_PARAM_INVALID 0x00000071
#define CKR_OBJECT_HANDLE_INVALID 0x00000082
#define CKR_OPERATION_ACTIVE      0x00000090
#define CKR_OPERATION_NOT_INITIALIZED 0x00000091
#define CKR_PIN_INCORRECT         0x000000A0
#define CKR_PIN_INVALID           0x000000A1
#define CKR_PIN_LEN_RANGE         0x000000A2
#define CKR_PIN_EXPIRED           0x000000A3
#define CKR_PIN_LOCKED            0x000000A4
#define CKR_SESSION_CLOSED        0x000000B0
#define CKR_SESSION_COUNT         0x000000B1
#define CKR_SESSION_HANDLE_INVALID 0x000000B3
#define CKR_SESSION_PARALLEL_NOT_SUPPORTED 0x000000B4
#define CKR_SESSION_READ_ONLY     0x000000B5
#define CKR_SESSION_EXISTS        0x000000B6
#define CKR_SESSION_READ_ONLY_EXISTS 0x000000B7
#define CKR_SESSION_READ_WRITE_SO_EXISTS 0x000000B8
#define CKR_SIGNATURE_INVALID     0x000000C0
#define CKR_SIGNATURE_LEN_RANGE   0x000000C1
#define CKR_TEMPLATE_INCOMPLETE   0x000000D0
#define CKR_TEMPLATE_INCONSISTENT 0x000000D1
#define CKR_TOKEN_NOT_PRESENT     0x000000E0
#define CKR_TOKEN_NOT_RECOGNIZED  0x000000E1
#define CKR_TOKEN_WRITE_PROTECTED 0x000000E2
#define CKR_UNWRAPPING_KEY_HANDLE_INVALID 0x000000F0
#define CKR_UNWRAPPING_KEY_SIZE_RANGE 0x000000F1
#define CKR_UNWRAPPING_KEY_TYPE_INCONSISTENT 0x000000F2
#define CKR_USER_ALREADY_LOGGED_IN 0x00000100
#define CKR_USER_NOT_LOGGED_IN    0x00000101
#define CKR_USER_PIN_NOT_INITIALIZED 0x00000102
#define CKR_USER_TYPE_INVALID     0x00000103
#define CKR_USER_ANOTHER_ALREADY_LOGGED_IN 0x00000104
#define CKR_USER_TOO_MANY_TYPES   0x00000105
#define CKR_BUFFER_TOO_SMALL      0x00000150
#define CKR_SAVED_STATE_INVALID   0x00000160
#define CKR_INFORMATION_SENSITIVE 0x00000170
#define CKR_STATE_UNSAVEABLE      0x00000180
#define CKR_CRYPTOKI_NOT_INITIALIZED 0x00000190
#define CKR_CRYPTOKI_ALREADY_INITIALIZED 0x00000191
#define CKR_VENDOR_DEFINED        0x80000000

/* 对象类 */
#define CKO_DATA                  0x00000000
#define CKO_CERTIFICATE           0x00000001
#define CKO_PUBLIC_KEY            0x00000002
#define CKO_PRIVATE_KEY           0x00000003
#define CKO_SECRET_KEY            0x00000004
#define CKO_HW_FEATURE            0x00000005
#define CKO_DOMAIN_PARAMETERS     0x00000006
#define CKO_MECHANISM             0x00000007

/* 密钥类型 */
#define CKK_RSA                   0x00000000
#define CKK_DSA                   0x00000001
#define CKK_DH                    0x00000002
#define CKK_EC                    0x00000003
#define CKK_X9_42_DH              0x00000004
#define CKK_KEA                   0x00000005
#define CKK_GENERIC_SECRET        0x00000010
#define CKK_RC2                   0x00000011
#define CKK_RC4                   0x00000012
#define CKK_DES                   0x00000013
#define CKK_DES2                  0x00000014
#define CKK_DES3                  0x00000015
#define CKK_CAST                  0x00000016
#define CKK_CAST3                 0x00000017
#define CKK_CAST128               0x00000018
#define CKK_RC5                   0x00000019
#define CKK_IDEA                  0x0000001A
#define CKK_SKIPJACK              0x0000001B
#define CKK_BATON                 0x0000001C
#define CKK_JUNIPER               0x0000001D
#define CKK_CDMF                  0x0000001E
#define CKK_AES                   0x0000001F
#define CKK_BLOWFISH              0x00000020
#define CKK_TWOFISH               0x00000021

/* 机制类型 */
#define CKM_RSA_PKCS_KEY_PAIR_GEN 0x00000000
#define CKM_RSA_PKCS              0x00000001
#define CKM_RSA_9796              0x00000002
#define CKM_RSA_X_509             0x00000003
#define CKM_MD2_RSA_PKCS          0x00000004
#define CKM_MD5_RSA_PKCS          0x00000005
#define CKM_SHA1_RSA_PKCS         0x00000006
#define CKM_SHA256_RSA_PKCS       0x00000040
#define CKM_SHA384_RSA_PKCS       0x00000041
#define CKM_SHA512_RSA_PKCS       0x00000042
#define CKM_RSA_PKCS_OAEP         0x00000009
#define CKM_RSA_X9_31_KEY_PAIR_GEN 0x0000000A
#define CKM_RSA_X9_31             0x0000000B
#define CKM_SHA1_RSA_X9_31        0x0000000C
#define CKM_RSA_PKCS_PSS          0x0000000D
#define CKM_SHA1_RSA_PKCS_PSS     0x0000000E
#define CKM_SHA256_RSA_PKCS_PSS   0x00000043
#define CKM_SHA384_RSA_PKCS_PSS   0x00000044
#define CKM_SHA512_RSA_PKCS_PSS   0x00000045
#define CKM_DSA_KEY_PAIR_GEN      0x00000010
#define CKM_DSA                   0x00000011
#define CKM_DSA_SHA1              0x00000012
#define CKM_DH_PKCS_KEY_PAIR_GEN  0x00000020
#define CKM_DH_PKCS_DERIVE        0x00000021
#define CKM_X9_42_DH_KEY_PAIR_GEN 0x00000030
#define CKM_X9_42_DH_DERIVE       0x00000031
#define CKM_X9_42_DH_HYBRID_DERIVE 0x00000032
#define CKM_X9_42_MQV_DERIVE      0x00000033
#define CKM_RC2_KEY_GEN           0x00000100
#define CKM_RC2_ECB               0x00000101
#define CKM_RC2_CBC               0x00000102
#define CKM_RC2_MAC               0x00000103
#define CKM_RC2_MAC_GENERAL       0x00000104
#define CKM_RC2_CBC_PAD           0x00000105
#define CKM_RC4_KEY_GEN           0x00000110
#define CKM_RC4                   0x00000111
#define CKM_DES_KEY_GEN           0x00000120
#define CKM_DES_ECB               0x00000121
#define CKM_DES_CBC               0x00000122
#define CKM_DES_MAC               0x00000123
#define CKM_DES_MAC_GENERAL       0x00000124
#define CKM_DES_CBC_PAD           0x00000125
#define CKM_DES2_KEY_GEN          0x00000130
#define CKM_DES3_KEY_GEN          0x00000131
#define CKM_DES3_ECB              0x00000132
#define CKM_DES3_CBC              0x00000133
#define CKM_DES3_MAC              0x00000134
#define CKM_DES3_MAC_GENERAL      0x00000135
#define CKM_DES3_CBC_PAD          0x00000136
#define CKM_CDMF_KEY_GEN          0x00000140
#define CKM_CDMF_ECB              0x00000141
#define CKM_CDMF_CBC              0x00000142
#define CKM_CDMF_MAC              0x00000143
#define CKM_CDMF_MAC_GENERAL      0x00000144
#define CKM_CDMF_CBC_PAD          0x00000145
#define CKM_MD2                   0x00000200
#define CKM_MD2_HMAC              0x00000201
#define CKM_MD2_HMAC_GENERAL      0x00000202
#define CKM_MD5                   0x00000210
#define CKM_MD5_HMAC              0x00000211
#define CKM_MD5_HMAC_GENERAL      0x00000212
#define CKM_SHA_1                 0x00000220
#define CKM_SHA_1_HMAC            0x00000221
#define CKM_SHA_1_HMAC_GENERAL    0x00000222
#define CKM_SHA_256               0x00000250
#define CKM_SHA_256_HMAC          0x00000251
#define CKM_SHA_256_HMAC_GENERAL  0x00000252
#define CKM_SHA_384               0x00000260
#define CKM_SHA_384_HMAC          0x00000261
#define CKM_SHA_384_HMAC_GENERAL  0x00000262
#define CKM_SHA_512               0x00000270
#define CKM_SHA_512_HMAC          0x00000271
#define CKM_SHA_512_HMAC_GENERAL  0x00000272
#define CKM_GENERIC_SECRET_KEY_GEN 0x00000350
#define CKM_SHA256_KEY_DERIVATION 0x00000396
#define CKM_SHA384_KEY_DERIVATION 0x00000397
#define CKM_SHA512_KEY_DERIVATION 0x00000398
#define CKM_AES_KEY_GEN           0x00001080
#define CKM_AES_ECB               0x00001081
#define CKM_AES_CBC               0x00001082
#define CKM_AES_MAC               0x00001083
#define CKM_AES_MAC_GENERAL       0x00001084
#define CKM_AES_CBC_PAD           0x00001085
#define CKM_AES_CTR               0x00001086
#define CKM_AES_GCM               0x00001087
#define CKM_AES_CCM               0x00001088
#define CKM_AES_CTS               0x00001089
#define CKM_AES_CMAC              0x0000108A
#define CKM_AES_CMAC_GENERAL      0x0000108B

/* 属性类型 */
#define CKA_CLASS                 0x00000000
#define CKA_TOKEN                 0x00000001
#define CKA_PRIVATE               0x00000002
#define CKA_LABEL                 0x00000003
#define CKA_APPLICATION           0x00000010
#define CKA_VALUE                 0x00000011
#define CKA_OBJECT_ID             0x00000012
#define CKA_CERTIFICATE_TYPE      0x00000080
#define CKA_ISSUER                0x00000081
#define CKA_SERIAL_NUMBER         0x00000082
#define CKA_AC_ISSUER             0x00000083
#define CKA_OWNER                 0x00000084
#define CKA_ATTR_TYPES            0x00000085
#define CKA_TRUSTED               0x00000086
#define CKA_CERTIFICATE_CATEGORY  0x00000087
#define CKA_JAVA_MIDP_SECURITY_DOMAIN 0x00000088
#define CKA_URL                   0x00000089
#define CKA_HASH_OF_SUBJECT_PUBLIC_KEY 0x0000008A
#define CKA_HASH_OF_ISSUER_PUBLIC_KEY 0x0000008B
#define CKA_CHECK_VALUE           0x0000008C
#define CKA_KEY_TYPE              0x00000100
#define CKA_SUBJECT               0x00000101
#define CKA_ID                    0x00000102
#define CKA_SENSITIVE             0x00000103
#define CKA_ENCRYPT               0x00000104
#define CKA_DECRYPT               0x00000105
#define CKA_WRAP                  0x00000106
#define CKA_UNWRAP                0x00000107
#define CKA_SIGN                  0x00000108
#define CKA_SIGN_RECOVER          0x00000109
#define CKA_VERIFY                0x0000010A
#define CKA_VERIFY_RECOVER        0x0000010B
#define CKA_DERIVE                0x0000010C
#define CKA_START_DATE            0x00000110
#define CKA_END_DATE              0x00000111
#define CKA_MODULUS               0x00000120
#define CKA_MODULUS_BITS          0x00000121
#define CKA_PUBLIC_EXPONENT       0x00000122
#define CKA_PRIVATE_EXPONENT      0x00000123
#define CKA_PRIME_1               0x00000124
#define CKA_PRIME_2               0x00000125
#define CKA_EXPONENT_1            0x00000126
#define CKA_EXPONENT_2            0x00000127
#define CKA_COEFFICIENT           0x00000128
#define CKA_PUBLIC_KEY_INFO       0x00000129
#define CKA_PRIME                 0x00000130
#define CKA_SUBPRIME              0x00000131
#define CKA_BASE                  0x00000132
#define CKA_PRIME_BITS            0x00000133
#define CKA_SUBPRIME_BITS         0x00000134
#define CKA_VALUE_BITS            0x00000160
#define CKA_VALUE_LEN             0x00000161
#define CKA_EXTRACTABLE           0x00000162
#define CKA_LOCAL                 0x00000163
#define CKA_NEVER_EXTRACTABLE     0x00000164
#define CKA_ALWAYS_SENSITIVE      0x00000165
#define CKA_KEY_GEN_MECHANISM     0x00000166
#define CKA_MODIFIABLE            0x00000170
#define CKA_COPYABLE              0x00000171
#define CKA_DESTROYABLE           0x00000172
#define CKA_ECDSA_PARAMS          0x00000180
#define CKA_EC_PARAMS             0x00000180
#define CKA_EC_POINT              0x00000181
#define CKA_SECONDARY_AUTH        0x00000200
#define CKA_AUTH_PIN_FLAGS        0x00000201
#define CKA_ALWAYS_AUTHENTICATE   0x00000202
#define CKA_WRAP_WITH_TRUSTED     0x00000210
#define CKA_WRAP_TEMPLATE         (CKF_ARRAY_ATTRIBUTE|0x00000211)
#define CKA_UNWRAP_TEMPLATE       (CKF_ARRAY_ATTRIBUTE|0x00000212)
#define CKA_DERIVE_TEMPLATE       (CKF_ARRAY_ATTRIBUTE|0x00000213)
#define CKA_OTP_FORMAT            0x00000220
#define CKA_OTP_LENGTH            0x00000221
#define CKA_OTP_TIME_INTERVAL     0x00000222
#define CKA_OTP_USER_FRIENDLY_MODE 0x00000223
#define CKA_OTP_CHALLENGE_REQUIREMENT 0x00000224
#define CKA_OTP_TIME_REQUIREMENT  0x00000225
#define CKA_OTP_COUNTER_REQUIREMENT 0x00000226
#define CKA_OTP_PIN_REQUIREMENT   0x00000227
#define CKA_OTP_COUNTER           0x0000022E
#define CKA_OTP_TIME              0x0000022F
#define CKA_OTP_USER_IDENTIFIER   0x0000022A
#define CKA_OTP_SERVICE_IDENTIFIER 0x0000022B
#define CKA_OTP_SERVICE_LOGO      0x0000022C
#define CKA_OTP_SERVICE_LOGO_TYPE 0x0000022D
#define CKA_GOSTR3410_PARAMS      0x00000250
#define CKA_GOSTR3411_PARAMS      0x00000251
#define CKA_GOST28147_PARAMS      0x00000252
#define CKA_HW_FEATURE_TYPE       0x00000300
#define CKA_RESET_ON_INIT         0x00000301
#define CKA_HAS_RESET             0x00000302
#define CKA_PIXEL_X               0x00000400
#define CKA_PIXEL_Y               0x00000401
#define CKA_RESOLUTION            0x00000402
#define CKA_CHAR_ROWS             0x00000403
#define CKA_CHAR_COLUMNS          0x00000404
#define CKA_COLOR                 0x00000405
#define CKA_BITS_PER_PIXEL        0x00000406
#define CKA_CHAR_SETS             0x00000480
#define CKA_ENCODING_METHODS      0x00000481
#define CKA_MIME_TYPES            0x00000482
#define CKA_MECHANISM_TYPE        0x00000500
#define CKA_REQUIRED_CMS_ATTRIBUTES 0x00000501
#define CKA_DEFAULT_CMS_ATTRIBUTES 0x00000502
#define CKA_SUPPORTED_CMS_ATTRIBUTES 0x00000503
#define CKA_ALLOWED_MECHANISMS    (CKF_ARRAY_ATTRIBUTE|0x00000600)
#define CKA_VENDOR_DEFINED        0x80000000

/* 会话类型 */
#define CKS_RO_PUBLIC_SESSION     0
#define CKS_RO_USER_FUNCTIONS     1
#define CKS_RW_PUBLIC_SESSION     2
#define CKS_RW_USER_FUNCTIONS     3
#define CKS_RW_SO_FUNCTIONS       4

/* 用户类型 */
#define CKU_SO                    0
#define CKU_USER                  1
#define CKU_CONTEXT_SPECIFIC      2

/* 状态标志 */
#define CKF_TOKEN_PRESENT         0x00000001
#define CKF_REMOVABLE_DEVICE      0x00000002
#define CKF_HW_SLOT               0x00000004
#define CKF_RNG                   0x00000001
#define CKF_WRITE_PROTECTED       0x00000002
#define CKF_LOGIN_REQUIRED        0x00000004
#define CKF_USER_PIN_INITIALIZED  0x00000008
#define CKF_RESTORE_KEY_NOT_NEEDED 0x00000020
#define CKF_CLOCK_ON_TOKEN        0x00000040
#define CKF_SECONDARY_AUTHENTICATION 0x00000080
#define CKF_USER_PIN_COUNT_LOW    0x00010000
#define CKF_USER_PIN_FINAL_TRY    0x00020000
#define CKF_USER_PIN_LOCKED       0x00040000
#define CKF_USER_PIN_TO_BE_CHANGED 0x00080000
#define CKF_SO_PIN_COUNT_LOW      0x00100000
#define CKF_SO_PIN_FINAL_TRY      0x00200000
#define CKF_SO_PIN_LOCKED         0x00400000
#define CKF_SO_PIN_TO_BE_CHANGED  0x00800000
#define CKF_RW_SESSION            0x00000002
#define CKF_SERIAL_SESSION        0x00000004

#endif /* PKCS11_TYPES_H */
```

---

## 3. PKCS#11 C语言编程

### 3.1 HSM连接与会话管理

```c
/* hsm_pkcs11.h - HSM PKCS#11接口 */
#ifndef HSM_PKCS11_H
#define HSM_PKCS11_H

#include "pkcs11_types.h"

/* 错误处理宏 */
#define CHECK_RV(rv, msg) \
    do { \
        if ((rv) != CKR_OK) { \
            fprintf(stderr, "%s: 0x%08lx - %s\n", msg, (unsigned long)(rv), pkcs11_error_string(rv)); \
            goto cleanup; \
        } \
    } while(0)

/* 函数声明 */
CK_RV hsm_load_library(const char *library_path);
CK_RV hsm_unload_library(void);
CK_RV hsm_initialize(void);
CK_RV hsm_finalize(void);
CK_RV hsm_get_slot_list(CK_BBOOL token_present, CK_SLOT_ID *slots, CK_ULONG *count);
CK_RV hsm_open_session(CK_SLOT_ID slot_id, CK_FLAGS flags, CK_SESSION_HANDLE *session);
CK_RV hsm_close_session(CK_SESSION_HANDLE session);
CK_RV hsm_login(CK_SESSION_HANDLE session, CK_USER_TYPE user_type, const char *pin);
CK_RV hsm_logout(CK_SESSION_HANDLE session);
const char* pkcs11_error_string(CK_RV rv);

#endif /* HSM_PKCS11_H */
```

```c
/* hsm_pkcs11.c - PKCS#11实现 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include "hsm_pkcs11.h"

/* PKCS#11函数指针类型定义 */
typedef CK_RV (*CK_C_GetFunctionList_t)(CK_FUNCTION_LIST_PTR_PTR);

/* 全局变量 */
static void *pkcs11_lib = NULL;
static CK_FUNCTION_LIST_PTR p11 = NULL;

/* 错误代码转字符串 */
const char* pkcs11_error_string(CK_RV rv)
{
    switch (rv) {
        case CKR_OK: return "CKR_OK";
        case CKR_CANCEL: return "CKR_CANCEL";
        case CKR_HOST_MEMORY: return "CKR_HOST_MEMORY";
        case CKR_SLOT_ID_INVALID: return "CKR_SLOT_ID_INVALID";
        case CKR_GENERAL_ERROR: return "CKR_GENERAL_ERROR";
        case CKR_FUNCTION_FAILED: return "CKR_FUNCTION_FAILED";
        case CKR_ARGUMENTS_BAD: return "CKR_ARGUMENTS_BAD";
        case CKR_ATTRIBUTE_READ_ONLY: return "CKR_ATTRIBUTE_READ_ONLY";
        case CKR_ATTRIBUTE_SENSITIVE: return "CKR_ATTRIBUTE_SENSITIVE";
        case CKR_ATTRIBUTE_TYPE_INVALID: return "CKR_ATTRIBUTE_TYPE_INVALID";
        case CKR_ATTRIBUTE_VALUE_INVALID: return "CKR_ATTRIBUTE_VALUE_INVALID";
        case CKR_DATA_INVALID: return "CKR_DATA_INVALID";
        case CKR_DATA_LEN_RANGE: return "CKR_DATA_LEN_RANGE";
        case CKR_DEVICE_ERROR: return "CKR_DEVICE_ERROR";
        case CKR_DEVICE_MEMORY: return "CKR_DEVICE_MEMORY";
        case CKR_DEVICE_REMOVED: return "CKR_DEVICE_REMOVED";
        case CKR_ENCRYPTED_DATA_INVALID: return "CKR_ENCRYPTED_DATA_INVALID";
        case CKR_ENCRYPTED_DATA_LEN_RANGE: return "CKR_ENCRYPTED_DATA_LEN_RANGE";
        case CKR_FUNCTION_CANCELED: return "CKR_FUNCTION_CANCELED";
        case CKR_FUNCTION_NOT_SUPPORTED: return "CKR_FUNCTION_NOT_SUPPORTED";
        case CKR_KEY_HANDLE_INVALID: return "CKR_KEY_HANDLE_INVALID";
        case CKR_KEY_SIZE_RANGE: return "CKR_KEY_SIZE_RANGE";
        case CKR_KEY_TYPE_INCONSISTENT: return "CKR_KEY_TYPE_INCONSISTENT";
        case CKR_KEY_NOT_NEEDED: return "CKR_KEY_NOT_NEEDED";
        case CKR_KEY_CHANGED: return "CKR_KEY_CHANGED";
        case CKR_KEY_NEEDED: return "CKR_KEY_NEEDED";
        case CKR_KEY_INDIGESTIBLE: return "CKR_KEY_INDIGESTIBLE";
        case CKR_KEY_FUNCTION_NOT_PERMITTED: return "CKR_KEY_FUNCTION_NOT_PERMITTED";
        case CKR_KEY_NOT_WRAPPABLE: return "CKR_KEY_NOT_WRAPPABLE";
        case CKR_KEY_UNEXTRACTABLE: return "CKR_KEY_UNEXTRACTABLE";
        case CKR_MECHANISM_INVALID: return "CKR_MECHANISM_INVALID";
        case CKR_MECHANISM_PARAM_INVALID: return "CKR_MECHANISM_PARAM_INVALID";
        case CKR_OBJECT_HANDLE_INVALID: return "CKR_OBJECT_HANDLE_INVALID";
        case CKR_OPERATION_ACTIVE: return "CKR_OPERATION_ACTIVE";
        case CKR_OPERATION_NOT_INITIALIZED: return "CKR_OPERATION_NOT_INITIALIZED";
        case CKR_PIN_INCORRECT: return "CKR_PIN_INCORRECT";
        case CKR_PIN_INVALID: return "CKR_PIN_INVALID";
        case CKR_PIN_LEN_RANGE: return "CKR_PIN_LEN_RANGE";
        case CKR_PIN_EXPIRED: return "CKR_PIN_EXPIRED";
        case CKR_PIN_LOCKED: return "CKR_PIN_LOCKED";
        case CKR_SESSION_CLOSED: return "CKR_SESSION_CLOSED";
        case CKR_SESSION_COUNT: return "CKR_SESSION_COUNT";
        case CKR_SESSION_HANDLE_INVALID: return "CKR_SESSION_HANDLE_INVALID";
        case CKR_SESSION_PARALLEL_NOT_SUPPORTED: return "CKR_SESSION_PARALLEL_NOT_SUPPORTED";
        case CKR_SESSION_READ_ONLY: return "CKR_SESSION_READ_ONLY";
        case CKR_SESSION_EXISTS: return "CKR_SESSION_EXISTS";
        case CKR_SESSION_READ_ONLY_EXISTS: return "CKR_SESSION_READ_ONLY_EXISTS";
        case CKR_SESSION_READ_WRITE_SO_EXISTS: return "CKR_SESSION_READ_WRITE_SO_EXISTS";
        case CKR_SIGNATURE_INVALID: return "CKR_SIGNATURE_INVALID";
        case CKR_SIGNATURE_LEN_RANGE: return "CKR_SIGNATURE_LEN_RANGE";
        case CKR_TEMPLATE_INCOMPLETE: return "CKR_TEMPLATE_INCOMPLETE";
        case CKR_TEMPLATE_INCONSISTENT: return "CKR_TEMPLATE_INCONSISTENT";
        case CKR_TOKEN_NOT_PRESENT: return "CKR_TOKEN_NOT_PRESENT";
        case CKR_TOKEN_NOT_RECOGNIZED: return "CKR_TOKEN_NOT_RECOGNIZED";
        case CKR_TOKEN_WRITE_PROTECTED: return "CKR_TOKEN_WRITE_PROTECTED";
        case CKR_UNWRAPPING_KEY_HANDLE_INVALID: return "CKR_UNWRAPPING_KEY_HANDLE_INVALID";
        case CKR_UNWRAPPING_KEY_SIZE_RANGE: return "CKR_UNWRAPPING_KEY_SIZE_RANGE";
        case CKR_UNWRAPPING_KEY_TYPE_INCONSISTENT: return "CKR_UNWRAPPING_KEY_TYPE_INCONSISTENT";
        case CKR_USER_ALREADY_LOGGED_IN: return "CKR_USER_ALREADY_LOGGED_IN";
        case CKR_USER_NOT_LOGGED_IN: return "CKR_USER_NOT_LOGGED_IN";
        case CKR_USER_PIN_NOT_INITIALIZED: return "CKR_USER_PIN_NOT_INITIALIZED";
        case CKR_USER_TYPE_INVALID: return "CKR_USER_TYPE_INVALID";
        case CKR_USER_ANOTHER_ALREADY_LOGGED_IN: return "CKR_USER_ANOTHER_ALREADY_LOGGED_IN";
        case CKR_USER_TOO_MANY_TYPES: return "CKR_USER_TOO_MANY_TYPES";
        case CKR_BUFFER_TOO_SMALL: return "CKR_BUFFER_TOO_SMALL";
        case CKR_SAVED_STATE_INVALID: return "CKR_SAVED_STATE_INVALID";
        case CKR_INFORMATION_SENSITIVE: return "CKR_INFORMATION_SENSITIVE";
        case CKR_STATE_UNSAVEABLE: return "CKR_STATE_UNSAVEABLE";
        case CKR_CRYPTOKI_NOT_INITIALIZED: return "CKR_CRYPTOKI_NOT_INITIALIZED";
        case CKR_CRYPTOKI_ALREADY_INITIALIZED: return "CKR_CRYPTOKI_ALREADY_INITIALIZED";
        default: return "Unknown Error";
    }
}

/* 加载PKCS#11库 */
CK_RV hsm_load_library(const char *library_path)
{
    pkcs11_lib = dlopen(library_path, RTLD_NOW);
    if (!pkcs11_lib) {
        fprintf(stderr, "无法加载PKCS#11库: %s\n", dlerror());
        return CKR_GENERAL_ERROR;
    }

    CK_C_GetFunctionList_t get_function_list =
        (CK_C_GetFunctionList_t)dlsym(pkcs11_lib, "C_GetFunctionList");
    if (!get_function_list) {
        fprintf(stderr, "无法获取C_GetFunctionList: %s\n", dlerror());
        dlclose(pkcs11_lib);
        pkcs11_lib = NULL;
        return CKR_GENERAL_ERROR;
    }

    CK_RV rv = get_function_list(&p11);
    if (rv != CKR_OK) {
        fprintf(stderr, "C_GetFunctionList失败: 0x%08lx\n", (unsigned long)rv);
        dlclose(pkcs11_lib);
        pkcs11_lib = NULL;
        p11 = NULL;
    }

    return rv;
}

/* 卸载PKCS#11库 */
CK_RV hsm_unload_library(void)
{
    if (pkcs11_lib) {
        dlclose(pkcs11_lib);
        pkcs11_lib = NULL;
        p11 = NULL;
    }
    return CKR_OK;
}

/* 初始化Cryptoki */
CK_RV hsm_initialize(void)
{
    if (!p11) return CKR_CRYPTOKI_NOT_INITIALIZED;
    return p11->C_Initialize(NULL);
}

/* 结束Cryptoki */
CK_RV hsm_finalize(void)
{
    if (!p11) return CKR_CRYPTOKI_NOT_INITIALIZED;
    return p11->C_Finalize(NULL);
}

/* 获取插槽列表 */
CK_RV hsm_get_slot_list(CK_BBOOL token_present, CK_SLOT_ID *slots, CK_ULONG *count)
{
    if (!p11) return CKR_CRYPTOKI_NOT_INITIALIZED;
    return p11->C_GetSlotList(token_present, slots, count);
}

/* 获取插槽信息 */
CK_RV hsm_get_slot_info(CK_SLOT_ID slot_id, CK_SLOT_INFO *info)
{
    if (!p11) return CKR_CRYPTOKI_NOT_INITIALIZED;
    return p11->C_GetSlotInfo(slot_id, info);
}

/* 获取令牌信息 */
CK_RV hsm_get_token_info(CK_SLOT_ID slot_id, CK_TOKEN_INFO *info)
{
    if (!p11) return CKR_CRYPTOKI_NOT_INITIALIZED;
    return p11->C_GetTokenInfo(slot_id, info);
}

/* 打开会话 */
CK_RV hsm_open_session(CK_SLOT_ID slot_id, CK_FLAGS flags, CK_SESSION_HANDLE *session)
{
    if (!p11) return CKR_CRYPTOKI_NOT_INITIALIZED;
    return p11->C_OpenSession(slot_id, flags, NULL, NULL, session);
}

/* 关闭会话 */
CK_RV hsm_close_session(CK_SESSION_HANDLE session)
{
    if (!p11) return CKR_CRYPTOKI_NOT_INITIALIZED;
    return p11->C_CloseSession(session);
}

/* 登录 */
CK_RV hsm_login(CK_SESSION_HANDLE session, CK_USER_TYPE user_type, const char *pin)
{
    if (!p11) return CKR_CRYPTOKI_NOT_INITIALIZED;
    return p11->C_Login(session, user_type, (CK_UTF8CHAR_PTR)pin,
                        pin ? (CK_ULONG)strlen(pin) : 0);
}

/* 登出 */
CK_RV hsm_logout(CK_SESSION_HANDLE session)
{
    if (!p11) return CKR_CRYPTOKI_NOT_INITIALIZED;
    return p11->C_Logout(session);
}
```

### 3.2 密钥生成与管理

```c
/* hsm_key_mgmt.c - 密钥管理实现 */
#include "hsm_pkcs11.h"

/* 生成AES密钥 */
CK_RV hsm_generate_aes_key(CK_SESSION_HANDLE session,
                            CK_ULONG key_length,
                            CK_OBJECT_HANDLE *key_handle)
{
    CK_MECHANISM mechanism = { CKM_AES_KEY_GEN, NULL_PTR, 0 };

    CK_BBOOL true_val = CK_TRUE;
    CK_BBOOL false_val = CK_FALSE;

    CK_ATTRIBUTE template[] = {
        {CKA_CLASS, &(CK_OBJECT_CLASS){CKO_SECRET_KEY}, sizeof(CK_OBJECT_CLASS)},
        {CKA_KEY_TYPE, &(CK_KEY_TYPE){CKK_AES}, sizeof(CK_KEY_TYPE)},
        {CKA_VALUE_LEN, &key_length, sizeof(key_length)},
        {CKA_ENCRYPT, &true_val, sizeof(true_val)},
        {CKA_DECRYPT, &true_val, sizeof(true_val)},
        {CKA_SIGN, &true_val, sizeof(true_val)},
        {CKA_VERIFY, &true_val, sizeof(true_val)},
        {CKA_WRAP, &true_val, sizeof(true_val)},
        {CKA_UNWRAP, &true_val, sizeof(true_val)},
        {CKA_SENSITIVE, &true_val, sizeof(true_val)},
        {CKA_EXTRACTABLE, &false_val, sizeof(false_val)},
        {CKA_TOKEN, &true_val, sizeof(true_val)},
        {CKA_PRIVATE, &true_val, sizeof(true_val)},
        {CKA_LABEL, "AES Key", 7}
    };

    return p11->C_GenerateKey(session, &mechanism, template,
                              sizeof(template)/sizeof(CK_ATTRIBUTE), key_handle);
}

/* 生成RSA密钥对 */
CK_RV hsm_generate_rsa_keypair(CK_SESSION_HANDLE session,
                                CK_ULONG modulus_bits,
                                CK_OBJECT_HANDLE *pub_key_handle,
                                CK_OBJECT_HANDLE *priv_key_handle)
{
    CK_MECHANISM mechanism = { CKM_RSA_PKCS_KEY_PAIR_GEN, NULL_PTR, 0 };

    CK_BBOOL true_val = CK_TRUE;
    CK_BBOOL false_val = CK_FALSE;
    CK_BYTE public_exponent[] = { 0x01, 0x00, 0x01 }; /* 65537 */

    /* 公钥模板 */
    CK_ATTRIBUTE pub_template[] = {
        {CKA_CLASS, &(CK_OBJECT_CLASS){CKO_PUBLIC_KEY}, sizeof(CK_OBJECT_CLASS)},
        {CKA_KEY_TYPE, &(CK_KEY_TYPE){CKK_RSA}, sizeof(CK_KEY_TYPE)},
        {CKA_MODULUS_BITS, &modulus_bits, sizeof(modulus_bits)},
        {CKA_PUBLIC_EXPONENT, public_exponent, sizeof(public_exponent)},
        {CKA_ENCRYPT, &true_val, sizeof(true_val)},
        {CKA_VERIFY, &true_val, sizeof(true_val)},
        {CKA_WRAP, &true_val, sizeof(true_val)},
        {CKA_TOKEN, &true_val, sizeof(true_val)},
        {CKA_LABEL, "RSA Public Key", 14}
    };

    /* 私钥模板 */
    CK_ATTRIBUTE priv_template[] = {
        {CKA_CLASS, &(CK_OBJECT_CLASS){CKO_PRIVATE_KEY}, sizeof(CK_OBJECT_CLASS)},
        {CKA_KEY_TYPE, &(CK_KEY_TYPE){CKK_RSA}, sizeof(CK_KEY_TYPE)},
        {CKA_DECRYPT, &true_val, sizeof(true_val)},
        {CKA_SIGN, &true_val, sizeof(true_val)},
        {CKA_UNWRAP, &true_val, sizeof(true_val)},
        {CKA_SENSITIVE, &true_val, sizeof(true_val)},
        {CKA_EXTRACTABLE, &false_val, sizeof(false_val)},
        {CKA_TOKEN, &true_val, sizeof(true_val)},
        {CKA_PRIVATE, &true_val, sizeof(true_val)},
        {CKA_LABEL, "RSA Private Key", 15}
    };

    return p11->C_GenerateKeyPair(session, &mechanism,
                                   pub_template, sizeof(pub_template)/sizeof(CK_ATTRIBUTE),
                                   priv_template, sizeof(priv_template)/sizeof(CK_ATTRIBUTE),
                                   pub_key_handle, priv_key_handle);
}

/* 查找密钥 */
CK_RV hsm_find_key(CK_SESSION_HANDLE session,
                    const char *label,
                    CK_OBJECT_CLASS key_class,
                    CK_KEY_TYPE key_type,
                    CK_OBJECT_HANDLE *key_handle)
{
    CK_ATTRIBUTE template[] = {
        {CKA_CLASS, &key_class, sizeof(key_class)},
        {CKA_KEY_TYPE, &key_type, sizeof(key_type)},
        {CKA_LABEL, (void*)label, label ? (CK_ULONG)strlen(label) : 0}
    };

    CK_RV rv = p11->C_FindObjectsInit(session, template,
                                       label ? 3 : 2);
    if (rv != CKR_OK) return rv;

    CK_ULONG count = 0;
    rv = p11->C_FindObjects(session, key_handle, 1, &count);

    p11->C_FindObjectsFinal(session);

    if (rv == CKR_OK && count == 0) {
        return CKR_OBJECT_HANDLE_INVALID;
    }

    return rv;
}

/* 删除密钥 */
CK_RV hsm_destroy_key(CK_SESSION_HANDLE session, CK_OBJECT_HANDLE key_handle)
{
    return p11->C_DestroyObject(session, key_handle);
}

/* 获取密钥属性 */
CK_RV hsm_get_key_attribute(CK_SESSION_HANDLE session,
                             CK_OBJECT_HANDLE key_handle,
                             CK_ATTRIBUTE_TYPE attr_type,
                             void *value,
                             CK_ULONG *value_len)
{
    CK_ATTRIBUTE attr = { attr_type, value, *value_len };
    CK_RV rv = p11->C_GetAttributeValue(session, key_handle, &attr, 1);
    *value_len = attr.ulValueLen;
    return rv;
}
```

### 3.3 加解密与签名操作

```c
/* hsm_crypto.c - 加解密和签名实现 */
#include "hsm_pkcs11.h"

/* AES加密 */
CK_RV hsm_aes_encrypt(CK_SESSION_HANDLE session,
                       CK_OBJECT_HANDLE key_handle,
                       const CK_BYTE *iv,
                       CK_ULONG iv_len,
                       const CK_BYTE *plaintext,
                       CK_ULONG plaintext_len,
                       CK_BYTE *ciphertext,
                       CK_ULONG *ciphertext_len)
{
    CK_MECHANISM mechanism;

    if (iv && iv_len > 0) {
        mechanism.mechanism = CKM_AES_CBC_PAD;
        mechanism.pParameter = (void*)iv;
        mechanism.ulParameterLen = iv_len;
    } else {
        mechanism.mechanism = CKM_AES_ECB;
        mechanism.pParameter = NULL_PTR;
        mechanism.ulParameterLen = 0;
    }

    CK_RV rv = p11->C_EncryptInit(session, &mechanism, key_handle);
    if (rv != CKR_OK) return rv;

    rv = p11->C_Encrypt(session, (CK_BYTE_PTR)plaintext, plaintext_len,
                        ciphertext, ciphertext_len);

    return rv;
}

/* AES解密 */
CK_RV hsm_aes_decrypt(CK_SESSION_HANDLE session,
                       CK_OBJECT_HANDLE key_handle,
                       const CK_BYTE *iv,
                       CK_ULONG iv_len,
                       const CK_BYTE *ciphertext,
                       CK_ULONG ciphertext_len,
                       CK_BYTE *plaintext,
                       CK_ULONG *plaintext_len)
{
    CK_MECHANISM mechanism;

    if (iv && iv_len > 0) {
        mechanism.mechanism = CKM_AES_CBC_PAD;
        mechanism.pParameter = (void*)iv;
        mechanism.ulParameterLen = iv_len;
    } else {
        mechanism.mechanism = CKM_AES_ECB;
        mechanism.pParameter = NULL_PTR;
        mechanism.ulParameterLen = 0;
    }

    CK_RV rv = p11->C_DecryptInit(session, &mechanism, key_handle);
    if (rv != CKR_OK) return rv;

    rv = p11->C_Decrypt(session, (CK_BYTE_PTR)ciphertext, ciphertext_len,
                        plaintext, plaintext_len);

    return rv;
}

/* RSA签名 */
CK_RV hsm_rsa_sign(CK_SESSION_HANDLE session,
                    CK_OBJECT_HANDLE priv_key_handle,
                    CK_MECHANISM_TYPE mechanism_type,
                    const CK_BYTE *data,
                    CK_ULONG data_len,
                    CK_BYTE *signature,
                    CK_ULONG *signature_len)
{
    CK_MECHANISM mechanism = { mechanism_type, NULL_PTR, 0 };

    CK_RV rv = p11->C_SignInit(session, &mechanism, priv_key_handle);
    if (rv != CKR_OK) return rv;

    rv = p11->C_Sign(session, (CK_BYTE_PTR)data, data_len,
                     signature, signature_len);

    return rv;
}

/* RSA验签 */
CK_RV hsm_rsa_verify(CK_SESSION_HANDLE session,
                      CK_OBJECT_HANDLE pub_key_handle,
                      CK_MECHANISM_TYPE mechanism_type,
                      const CK_BYTE *data,
                      CK_ULONG data_len,
                      const CK_BYTE *signature,
                      CK_ULONG signature_len)
{
    CK_MECHANISM mechanism = { mechanism_type, NULL_PTR, 0 };

    CK_RV rv = p11->C_VerifyInit(session, &mechanism, pub_key_handle);
    if (rv != CKR_OK) return rv;

    rv = p11->C_Verify(session, (CK_BYTE_PTR)data, data_len,
                       (CK_BYTE_PTR)signature, signature_len);

    return rv;
}

/* RSA加密 */
CK_RV hsm_rsa_encrypt(CK_SESSION_HANDLE session,
                       CK_OBJECT_HANDLE pub_key_handle,
                       CK_MECHANISM_TYPE mechanism_type,
                       const CK_BYTE *plaintext,
                       CK_ULONG plaintext_len,
                       CK_BYTE *ciphertext,
                       CK_ULONG *ciphertext_len)
{
    CK_MECHANISM mechanism = { mechanism_type, NULL_PTR, 0 };

    CK_RV rv = p11->C_EncryptInit(session, &mechanism, pub_key_handle);
    if (rv != CKR_OK) return rv;

    rv = p11->C_Encrypt(session, (CK_BYTE_PTR)plaintext, plaintext_len,
                        ciphertext, ciphertext_len);

    return rv;
}

/* RSA解密 */
CK_RV hsm_rsa_decrypt(CK_SESSION_HANDLE session,
                       CK_OBJECT_HANDLE priv_key_handle,
                       CK_MECHANISM_TYPE mechanism_type,
                       const CK_BYTE *ciphertext,
                       CK_ULONG ciphertext_len,
                       CK_BYTE *plaintext,
                       CK_ULONG *plaintext_len)
{
    CK_MECHANISM mechanism = { mechanism_type, NULL_PTR, 0 };

    CK_RV rv = p11->C_DecryptInit(session, &mechanism, priv_key_handle);
    if (rv != CKR_OK) return rv;

    rv = p11->C_Decrypt(session, (CK_BYTE_PTR)ciphertext, ciphertext_len,
                        plaintext, plaintext_len);

    return rv;
}

/* 哈希运算 */
CK_RV hsm_digest(CK_SESSION_HANDLE session,
                  CK_MECHANISM_TYPE mechanism_type,
                  const CK_BYTE *data,
                  CK_ULONG data_len,
                  CK_BYTE *digest,
                  CK_ULONG *digest_len)
{
    CK_MECHANISM mechanism = { mechanism_type, NULL_PTR, 0 };

    CK_RV rv = p11->C_DigestInit(session, &mechanism);
    if (rv != CKR_OK) return rv;

    rv = p11->C_Digest(session, (CK_BYTE_PTR)data, data_len,
                       digest, digest_len);

    return rv;
}
```

---

## 4. OpenSSL引擎与HSM集成

### 4.1 OpenSSL引擎架构

```c
/* hsm_engine.c - OpenSSL HSM引擎实现 */
#include <openssl/engine.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/bn.h>
#include "hsm_pkcs11.h"

#define HSM_ENGINE_ID "hsm_pkcs11"
#define HSM_ENGINE_NAME "HSM PKCS#11 Engine"

/* 引擎私有数据结构 */
typedef struct {
    CK_SESSION_HANDLE session;
    CK_OBJECT_HANDLE private_key;
    CK_OBJECT_HANDLE public_key;
} HSM_KEY_CTX;

static const char *hsm_library_path = NULL;
static CK_SESSION_HANDLE hsm_session = 0;

/* 引擎初始化 */
static int hsm_engine_init(ENGINE *e)
{
    (void)e;

    if (!hsm_library_path) {
        ERR_put_error(ERR_LIB_ENGINE, 0, ENGINE_R_INVALID_ARGUMENT,
                      __FILE__, __LINE__);
        return 0;
    }

    CK_RV rv = hsm_load_library(hsm_library_path);
    if (rv != CKR_OK) return 0;

    rv = hsm_initialize();
    if (rv != CKR_OK) {
        hsm_unload_library();
        return 0;
    }

    return 1;
}

/* 引擎销毁 */
static int hsm_engine_finish(ENGINE *e)
{
    (void)e;

    if (hsm_session) {
        hsm_logout(hsm_session);
        hsm_close_session(hsm_session);
        hsm_session = 0;
    }

    hsm_finalize();
    hsm_unload_library();

    return 1;
}

/* RSA私钥签名 (使用HSM) */
static int hsm_rsa_sign(int type, const unsigned char *m, unsigned int m_length,
                         unsigned char *sigret, unsigned int *siglen,
                         const RSA *rsa)
{
    HSM_KEY_CTX *ctx = RSA_get_ex_data(rsa, 0);
    if (!ctx) return 0;

    CK_MECHANISM_TYPE mech;
    switch (type) {
        case NID_sha256: mech = CKM_SHA256_RSA_PKCS; break;
        case NID_sha384: mech = CKM_SHA384_RSA_PKCS; break;
        case NID_sha512: mech = CKM_SHA512_RSA_PKCS; break;
        default: mech = CKM_RSA_PKCS; break;
    }

    CK_ULONG sig_len = *siglen;
    CK_RV rv = hsm_rsa_sign(ctx->session, ctx->private_key, mech,
                            m, m_length, sigret, &sig_len);
    if (rv != CKR_OK) return 0;

    *siglen = (unsigned int)sig_len;
    return 1;
}

/* RSA公钥解密 (使用HSM) */
static int hsm_rsa_decrypt(int flen, const unsigned char *from,
                            unsigned char *to, RSA *rsa, int padding)
{
    HSM_KEY_CTX *ctx = RSA_get_ex_data(rsa, 0);
    if (!ctx) return -1;

    if (padding != RSA_PKCS1_PADDING) {
        ERR_put_error(ERR_LIB_RSA, 0, RSA_R_UNKNOWN_PADDING_TYPE,
                      __FILE__, __LINE__);
        return -1;
    }

    CK_ULONG out_len = RSA_size(rsa);
    CK_RV rv = hsm_rsa_decrypt(ctx->session, ctx->private_key,
                                CKM_RSA_PKCS, from, flen, to, &out_len);
    if (rv != CKR_OK) return -1;

    return (int)out_len;
}

/* 设置HSM库路径 */
static int hsm_ctrl(ENGINE *e, int cmd, long i, void *p, void (*f)(void))
{
    (void)e; (void)i; (void)f;

    switch (cmd) {
        case 1: /* 设置库路径 */
            if (hsm_library_path) {
                free((void*)hsm_library_path);
            }
            hsm_library_path = strdup((const char*)p);
            return 1;
        default:
            return 0;
    }
}

/* 绑定引擎 */
static int bind_engine(ENGINE *e)
{
    if (!ENGINE_set_id(e, HSM_ENGINE_ID) ||
        !ENGINE_set_name(e, HSM_ENGINE_NAME) ||
        !ENGINE_set_init_function(e, hsm_engine_init) ||
        !ENGINE_set_finish_function(e, hsm_engine_finish) ||
        !ENGINE_set_ctrl_function(e, hsm_ctrl)) {
        return 0;
    }

    return 1;
}

IMPLEMENT_DYNAMIC_CHECK_FN()
IMPLEMENT_DYNAMIC_BIND_FN(bind_engine)
```

### 4.2 使用示例

```c
/* hsm_example.c - HSM使用示例 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include "hsm_pkcs11.h"

int main(int argc, char *argv[])
{
    CK_RV rv;

    printf("=== HSM PKCS#11 演示 ===\n\n");

    /* 加载PKCS#11库 */
    const char *library_path = (argc > 1) ? argv[1] : "/usr/lib/libCryptoki2.so";
    printf("加载库: %s\n", library_path);

    rv = hsm_load_library(library_path);
    CHECK_RV(rv, "加载库失败");

    /* 初始化 */
    rv = hsm_initialize();
    CHECK_RV(rv, "初始化失败");

    /* 获取插槽列表 */
    CK_SLOT_ID slots[10];
    CK_ULONG slot_count = 10;
    rv = hsm_get_slot_list(CK_TRUE, slots, &slot_count);
    CHECK_RV(rv, "获取插槽失败");

    printf("发现 %lu 个带令牌的插槽\n", slot_count);
    if (slot_count == 0) {
        fprintf(stderr, "未找到HSM设备\n");
        goto cleanup;
    }

    /* 使用第一个插槽 */
    CK_SLOT_ID slot = slots[0];

    /* 打开读写会话 */
    CK_SESSION_HANDLE session;
    rv = hsm_open_session(slot, CKF_RW_SESSION | CKF_SERIAL_SESSION, &session);
    CHECK_RV(rv, "打开会话失败");

    printf("会话已打开\n");

    /* 用户登录 */
    const char *pin = (argc > 2) ? argv[2] : "1234";
    rv = hsm_login(session, CKU_USER, pin);
    CHECK_RV(rv, "登录失败");

    printf("登录成功\n\n");

    /* 生成AES密钥 */
    printf("生成AES-256密钥...\n");
    CK_OBJECT_HANDLE aes_key;
    rv = hsm_generate_aes_key(session, 32, &aes_key);
    CHECK_RV(rv, "生成AES密钥失败");
    printf("AES密钥句柄: %lu\n", (unsigned long)aes_key);

    /* AES加密测试 */
    CK_BYTE plaintext[] = "Hello, HSM! This is a secret message.";
    CK_BYTE ciphertext[256] = {0};
    CK_BYTE iv[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                       0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10};
    CK_ULONG ciphertext_len = sizeof(ciphertext);

    rv = hsm_aes_encrypt(session, aes_key, iv, sizeof(iv),
                         plaintext, sizeof(plaintext)-1,
                         ciphertext, &ciphertext_len);
    CHECK_RV(rv, "AES加密失败");
    printf("加密完成，密文长度: %lu\n", ciphertext_len);

    /* AES解密测试 */
    CK_BYTE decrypted[256] = {0};
    CK_ULONG decrypted_len = sizeof(decrypted);

    rv = hsm_aes_decrypt(session, aes_key, iv, sizeof(iv),
                         ciphertext, ciphertext_len,
                         decrypted, &decrypted_len);
    CHECK_RV(rv, "AES解密失败");
    decrypted[decrypted_len] = '\0';
    printf("解密完成: %s\n\n", decrypted);

    /* 生成RSA密钥对 */
    printf("生成RSA-2048密钥对...\n");
    CK_OBJECT_HANDLE rsa_pub, rsa_priv;
    rv = hsm_generate_rsa_keypair(session, 2048, &rsa_pub, &rsa_priv);
    CHECK_RV(rv, "生成RSA密钥对失败");
    printf("RSA公钥句柄: %lu, 私钥句柄: %lu\n",
           (unsigned long)rsa_pub, (unsigned long)rsa_priv);

    /* RSA签名测试 */
    CK_BYTE data[] = "Data to be signed";
    CK_BYTE signature[256] = {0};
    CK_ULONG signature_len = sizeof(signature);

    rv = hsm_rsa_sign(session, rsa_priv, CKM_SHA256_RSA_PKCS,
                      data, sizeof(data)-1, signature, &signature_len);
    CHECK_RV(rv, "RSA签名失败");
    printf("签名完成，签名长度: %lu\n", signature_len);

    /* RSA验签测试 */
    rv = hsm_rsa_verify(session, rsa_pub, CKM_SHA256_RSA_PKCS,
                        data, sizeof(data)-1, signature, signature_len);
    if (rv == CKR_OK) {
        printf("签名验证成功!\n\n");
    } else {
        printf("签名验证失败: %s\n\n", pkcs11_error_string(rv));
    }

    /* 清理 */
    hsm_destroy_key(session, aes_key);
    hsm_destroy_key(session, rsa_pub);
    hsm_destroy_key(session, rsa_priv);

    hsm_logout(session);
    hsm_close_session(session);

cleanup:
    hsm_finalize();
    hsm_unload_library();

    printf("演示完成\n");
    return (rv == CKR_OK) ? 0 : 1;
}
```

---

## 5. 密钥生命周期管理

### 5.1 密钥管理流程

```text
┌─────────────┐    ┌─────────────┐    ┌─────────────┐    ┌─────────────┐
│   密钥生成   │───▶│   密钥分发   │───▶│   密钥使用   │───▶│   密钥销毁   │
│  Generation │    │ Distribution│    │   Usage     │    │ Destruction │
└─────────────┘    └─────────────┘    └─────────────┘    └─────────────┘
       │                  │                  │                  │
       ▼                  ▼                  ▼                  ▼
  · 在HSM内生成      · 安全传输         · 加密运算         · 安全擦除
  · 真随机数         · 密钥分割         · 签名/验签        · 审计记录
  · 不可导出         · 密钥协商         · 密钥包装        · 物理销毁
```

### 5.2 密钥轮换策略

```c
/* key_rotation.c - 密钥轮换实现 */
#include "hsm_pkcs11.h"

/* 创建新版本的密钥 */
CK_RV hsm_rotate_aes_key(CK_SESSION_HANDLE session,
                          CK_OBJECT_HANDLE old_key,
                          CK_OBJECT_HANDLE *new_key,
                          const char *base_label)
{
    CK_RV rv;

    /* 生成新密钥 */
    rv = hsm_generate_aes_key(session, 32, new_key);
    if (rv != CKR_OK) return rv;

    /* 设置新标签，包含版本号 */
    char new_label[256];
    snprintf(new_label, sizeof(new_label), "%s_v%d", base_label,
             (int)time(NULL));

    CK_ATTRIBUTE label_attr = {CKA_LABEL, new_label, strlen(new_label)};
    rv = p11->C_SetAttributeValue(session, *new_key, &label_attr, 1);

    /* 使用旧密钥解密、新密钥重新加密数据 */
    /* ... */

    return rv;
}

/* 密钥备份 (使用密钥包装) */
CK_RV hsm_backup_key(CK_SESSION_HANDLE session,
                      CK_OBJECT_HANDLE key_to_backup,
                      CK_OBJECT_HANDLE wrap_key,
                      CK_BYTE *wrapped_key,
                      CK_ULONG *wrapped_len)
{
    CK_MECHANISM mechanism = { CKM_AES_CBC_PAD, NULL_PTR, 0 };

    /* 初始化包装操作 */
    CK_RV rv = p11->C_WrapKey(session, &mechanism, wrap_key,
                               key_to_backup, wrapped_key, wrapped_len);

    return rv;
}
```

---

## 6. 总结

本文详细介绍了HSM与C语言的集成方法：

1. **HSM基础**：理解硬件安全模块的架构和安全特性
2. **PKCS#11标准**：掌握跨厂商的通用加密接口
3. **会话管理**：实现安全的连接和认证
4. **密钥管理**：生成、存储、轮换和销毁密钥
5. **加密操作**：使用HSM执行加解密和签名
6. **OpenSSL集成**：通过引擎将HSM与标准库结合

HSM为高安全性应用提供了硬件级保护，是金融、政府和关键基础设施的必备组件。
