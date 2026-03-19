/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\05_Secure_Boot_Deep_Dive.md
 * Line: 144
 * Language: c
 * Block ID: e027cc0d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 信任链数据结构定义
 * 基于ARM Trusted Firmware和TPM 2.0规范
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/* 镜像认证类型 */
typedef enum {
    AUTH_NONE = 0,
    AUTH_RSA_PSS,
    AUTH_RSA_PKCS1_V15,
    AUTH_ECDSA,
    AUTH_EDDSA,
} auth_type_t;

/* 哈希算法 */
typedef enum {
    HASH_SHA256 = 0,
    HASH_SHA384,
    HASH_SHA512,
    HASH_SHA3_256,
} hash_alg_t;

/* 镜像头部（简化版） */
typedef struct __attribute__((packed)) {
    uint32_t magic;                 /* 'A', 'T', 'F', 'M' */
    uint32_t version;               /* 头部版本 */
    uint64_t flags;                 /* 安全属性标志 */
    uint32_t image_size;            /* 镜像大小 */
    uint32_t image_offset;          /* 镜像数据偏移 */

    /* 认证数据 */
    auth_type_t auth_type;          /* 认证类型 */
    hash_alg_t hash_alg;            /* 哈希算法 */
    uint32_t signature_offset;      /* 签名偏移 */
    uint32_t signature_size;        /* 签名大小 */

    /* 密钥信息 */
    uint32_t pk_offset;             /* 公钥偏移 */
    uint32_t pk_size;               /* 公钥大小 */

    /* 扩展 */
    uint32_t plat_flags;            /* 平台特定标志 */
    uint32_t reserved[4];
} image_header_t;

/* 信任链节点 */
typedef struct trust_chain_node {
    const char *name;               /* 组件名称 */
    uint8_t hash[64];               /* SHA-512哈希值 */
    uint32_t hash_len;              /* 实际哈希长度 */
    uint32_t pcr_index;             /* TPM PCR索引 */
    struct trust_chain_node *next;  /* 下一级 */
} trust_chain_node_t;

/* 启动测量记录 */
typedef struct {
    uint32_t event_type;            /* 事件类型 */
    uint32_t pcr_index;             /* PCR索引 */
    uint8_t digest[64];             /* 测量值 */
    uint32_t digest_len;
    uint32_t event_size;            /* 事件数据大小 */
    uint8_t event_data[256];        /* 事件描述 */
} tpm_event_t;

#define IMAGE_MAGIC     0x4D465441  /* "ATFM" */
#define HEADER_VERSION  0x00010000  /* 1.0 */

/* 安全启动错误码 */
typedef enum {
    SECURE_BOOT_OK = 0,
    ERR_INVALID_MAGIC,
    ERR_VERSION_MISMATCH,
    ERR_HASH_VERIFY_FAIL,
    ERR_SIGNATURE_INVALID,
    ERR_CERT_CHAIN_BROKEN,
    ERR_ROLLBACK_DETECTED,
    ERR_TPM_COMMUNICATION,
} secure_boot_err_t;
