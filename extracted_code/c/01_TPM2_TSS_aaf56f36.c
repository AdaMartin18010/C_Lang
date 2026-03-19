/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\01_TPM2_TSS.md
 * Line: 351
 * Language: c
 * Block ID: aaf56f36
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 敏感区域结构 */
typedef struct {
    uint16_t user_auth_size;
    uint8_t  user_auth[32];
    uint16_t data_size;
    uint8_t  data[256];
} TPM2B_SENSITIVE_CREATE;

/* 公钥模板 */
typedef struct {
    uint16_t type;           /* 算法类型 */
    uint16_t name_alg;       /* 命名哈希算法 */
    uint32_t object_attrs;   /* 对象属性 */
    uint16_t auth_policy_size;
    uint8_t  auth_policy[64];
    /* 算法特定参数... */
} TPMT_PUBLIC;

/* TPM2_Create - 创建密钥 */
int tpm_create_key(uint32_t parent_handle,
                   const uint8_t *auth_value, uint16_t auth_len,
                   TPMT_PUBLIC *public_template,
                   uint32_t *object_handle) {
    TPM_CMD_BUFFER cmd = {0};

    tpm_build_header(&cmd, TPM_ST_SESSIONS, 0, TPM_CC_Create);

    /* parentHandle */
    tpm_cmd_write32(&cmd, parent_handle);

    /* 授权区域 */
    tpm_cmd_write32(&cmd, 1);
    tpm_cmd_write32(&cmd, TPM_RS_PW);
    tpm_cmd_write16(&cmd, 0);
    cmd.buffer[cmd.offset++] = 0x00;
    tpm_cmd_write16(&cmd, 0);

    /* inSensitive */
    uint16_t sensitive_size_pos = cmd.offset;
    cmd.offset += 2;  /* 预留大小字段 */

    /* userAuth */
    tpm_cmd_write16(&cmd, auth_len);
    memcpy(cmd.buffer + cmd.offset, auth_value, auth_len);
    cmd.offset += auth_len;

    /* data (for sealed data, empty for key) */
    tpm_cmd_write16(&cmd, 0);

    /* 回填sensitive大小 */
    uint16_t sensitive_size = cmd.offset - sensitive_size_pos - 2;
    cmd.buffer[sensitive_size_pos] = (sensitive_size >> 8) & 0xFF;
    cmd.buffer[sensitive_size_pos + 1] = sensitive_size & 0xFF;

    /* inPublic */
    /* ... 序列化public_template ... */

    cmd.size = cmd.offset;

    /* 发送命令并解析返回的private和public blob... */

    return 0;
}
