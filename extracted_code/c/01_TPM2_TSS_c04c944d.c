/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\01_TPM2_TSS.md
 * Line: 238
 * Language: c
 * Block ID: c04c944d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* TPM2_GetRandom 命令 */
int tpm_get_random(uint16_t bytes_requested, uint8_t *random_bytes) {
    TPM_CMD_BUFFER cmd = {0};

    /* 构建命令 */
    tpm_build_header(&cmd, TPM_ST_NO_SESSIONS, 14, TPM_CC_GetRandom);
    tpm_cmd_write16(&cmd, bytes_requested);

    /* 更新大小 */
    cmd.size = cmd.offset;
    cmd.buffer[2] = (cmd.size >> 24) & 0xFF;
    cmd.buffer[3] = (cmd.size >> 16) & 0xFF;
    cmd.buffer[4] = (cmd.size >> 8) & 0xFF;
    cmd.buffer[5] = cmd.size & 0xFF;

    /* 发送命令 */
    uint8_t rsp_buffer[256];
    size_t rsp_size = sizeof(rsp_buffer);

    int ret = tcti_transmit_receive(cmd.buffer, cmd.size, rsp_buffer, &rsp_size);
    if (ret != 0) return ret;

    /* 解析响应 */
    TPM_RSP_BUFFER rsp = {rsp_buffer, rsp_size, 0};
    uint16_t tag = tpm_rsp_read16(&rsp);
    uint32_t size = tpm_rsp_read32(&rsp);
    uint32_t rc = tpm_rsp_read32(&rsp);

    if (rc != TPM_RC_SUCCESS) {
        return -1;  /* TPM错误 */
    }

    /* 读取随机数 */
    uint16_t random_len = tpm_rsp_read16(&rsp);
    memcpy(random_bytes, rsp.buffer + rsp.offset, random_len);

    return random_len;
}
