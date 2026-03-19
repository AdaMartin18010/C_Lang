/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\01_TPM2_TSS.md
 * Line: 281
 * Language: c
 * Block ID: 7b47c5a3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* PCR选择结构 */
typedef struct {
    uint8_t  sizeof_select;  /* 通常为3 (SHA-1, SHA-256, SHA-384) */
    uint8_t  pcr_select[3];  /* 位图表示选中的PCR */
} TPMS_PCR_SELECTION;

/* TPM2_PCR_Read - 读取PCR值 */
int tpm_pcr_read(uint32_t pcr_index, uint8_t *pcr_value, size_t *len) {
    TPM_CMD_BUFFER cmd = {0};

    tpm_build_header(&cmd, TPM_ST_NO_SESSIONS, 0, TPM_CC_PCR_Read);

    /* pcrSelectionIn - 计数 */
    tpm_cmd_write32(&cmd, 1);  /* 1个hash算法 */

    /* hash算法 (SHA-256 = 0x000B) */
    tpm_cmd_write16(&cmd, 0x000B);

    /* pcrSelect */
    cmd.buffer[cmd.offset++] = 3;  /* sizeofSelect */
    memset(cmd.buffer + cmd.offset, 0, 3);
    cmd.buffer[cmd.offset + (pcr_index / 8)] = 1 << (pcr_index % 8);
    cmd.offset += 3;

    /* 更新大小 */
    cmd.size = cmd.offset;

    /* 发送并接收... */
    /* 解析返回的PCR值... */

    return 0;
}

/* TPM2_PCR_Extend - 扩展PCR */
int tpm_pcr_extend(uint32_t pcr_index, const uint8_t *digest, size_t digest_len) {
    TPM_CMD_BUFFER cmd = {0};

    /* 需要授权会话 */
    tpm_build_header(&cmd, TPM_ST_SESSIONS, 0, TPM_CC_PCR_Extend);

    /* pcrHandle */
    tpm_cmd_write32(&cmd, pcr_index);

    /* 授权区域 - 使用密码授权 */
    tpm_cmd_write32(&cmd, 1);  /* authArea count */
    tpm_cmd_write32(&cmd, TPM_RS_PW);  /* authHandle */
    tpm_cmd_write16(&cmd, 0);  /* nonceEmpty */
    cmd.buffer[cmd.offset++] = 0x00;  /* sessionAttributes */
    tpm_cmd_write16(&cmd, 0);  /* authEmpty */

    /* digests count */
    tpm_cmd_write32(&cmd, 1);
    /* hash算法 */
    tpm_cmd_write16(&cmd, 0x000B);  /* SHA-256 */
    /* digest */
    tpm_cmd_write_bytes(&cmd, digest, digest_len);

    cmd.size = cmd.offset;

    return 0;
}
