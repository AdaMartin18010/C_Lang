/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\02_Key_Sealing.md
 * Line: 368
 * Language: c
 * Block ID: 6d5f0dac
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 密封数据到TPM
 * 将数据与当前PCR状态绑定
 */
int tpm_seal_data(uint32_t parent_handle,           /* 父密钥句柄 */
                  const uint8_t *data,               /* 要密封的数据 */
                  uint16_t data_len,
                  const TPM2B_DIGEST *auth_policy,   /* 授权策略 */
                  const TPML_PCR_SELECTION *pcr_select,
                  SealedDataPackage *sealed) {

    TPM_CMD_BUFFER cmd = {0};

    /* 1. 构建敏感区域 */
    /* sensitive.userAuth - 对象授权 */
    /* sensitive.data - 要密封的数据 */

    /* 2. 构建公共区域模板 */
    TPMT_PUBLIC public_template = {
        .type = 0x0025,           /* TPM_ALG_KEYEDHASH */
        .nameAlg = 0x000B,        /* SHA-256 */
        .objectAttributes = 0x00020072,  /* userWithAuth | adminWithPolicy |
                                            noDA | sign | decrypt */
    };

    /* 设置授权策略 */
    if (auth_policy) {
        public_template.authPolicy.size = auth_policy->size;
        memcpy(public_template.authPolicy.buffer,
               auth_policy->buffer, auth_policy->size);
        /* 清除userWithAuth，要求必须满足策略 */
        public_template.objectAttributes &= ~0x00000040;
    }

    /* 3. 调用TPM2_Create */
    tpm_build_header(&cmd, TPM_ST_SESSIONS, 0, TPM_CC_Create);

    /* parentHandle */
    tpm_cmd_write32(&cmd, parent_handle);

    /* 授权区域 (使用父密钥的授权) */
    tpm_cmd_write32(&cmd, 1);
    tpm_cmd_write32(&cmd, TPM_RS_PW);  /* 或具体的授权会话 */
    tpm_cmd_write16(&cmd, 0);
    cmd.buffer[cmd.offset++] = 0x00;
    tpm_cmd_write16(&cmd, 0);  /* 空密码 */

    /* inSensitive */
    uint16_t sensitive_pos = cmd.offset;
    cmd.offset += 2;

    /* userAuth */
    tpm_cmd_write16(&cmd, 0);  /* 空授权 */

    /* data (要密封的数据) */
    tpm_cmd_write16(&cmd, data_len);
    memcpy(cmd.buffer + cmd.offset, data, data_len);
    cmd.offset += data_len;

    /* 回填sensitive大小 */
    uint16_t sensitive_size = cmd.offset - sensitive_pos - 2;
    cmd.buffer[sensitive_pos] = (sensitive_size >> 8) & 0xFF;
    cmd.buffer[sensitive_pos + 1] = sensitive_size & 0xFF;

    /* inPublic - 序列化public_template... */

    /* 外部敏感数据标记 */
    tpm_cmd_write16(&cmd, 0);

    cmd.size = cmd.offset;

    /* 4. 发送命令并接收响应... */
    uint8_t rsp[2048];
    size_t rsp_len = sizeof(rsp);
    int ret = tcti_transmit_receive(cmd.buffer, cmd.size, rsp, &rsp_len);
    if (ret != 0) return ret;

    /* 5. 解析返回的private和public blob */
    TPM_RSP_BUFFER rp = {rsp, rsp_len, 0};
    /* 跳过header... */

    /* 保存密封数据包 */
    /* ... */

    return 0;
}
