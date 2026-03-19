/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\02_Key_Sealing.md
 * Line: 153
 * Language: c
 * Block ID: a8d25f7c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 策略会话类型 */
typedef enum {
    POLICY_PASSWORD = 0x01,
    POLICY_PCR      = 0x02,
    POLICY_AUTH     = 0x04,
    POLICY_PHYSICAL = 0x08,
} PolicyType;

/* 密封策略配置 */
typedef struct {
    PolicyType type;
    union {
        /* PCR策略 */
        struct {
            TPML_PCR_SELECTION pcr_select;
            TPM2B_DIGEST pcr_digest;  /* PCR值的摘要 */
        } pcr;

        /* 密码授权 */
        struct {
            TPM2B_DIGEST auth_hash;
        } password;
    };
} SealingPolicy;

/* 密封数据包 */
typedef struct {
    TPM2B_PUBLIC public;
    TPM2B_PRIVATE private;
    SealingPolicy policy;
    uint32_t parent_handle;
    uint64_t creation_time;
} SealedDataPackage;
