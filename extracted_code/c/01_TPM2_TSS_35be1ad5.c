/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\01_TPM2_TSS.md
 * Line: 419
 * Language: c
 * Block ID: 35be1ad5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* TPM2_Load - 加载密钥 */
int tpm_load_key(uint32_t parent_handle,
                 const uint8_t *private_blob, uint16_t private_len,
                 const uint8_t *public_blob, uint16_t public_len,
                 uint32_t *loaded_handle) {
    TPM_CMD_BUFFER cmd = {0};

    tpm_build_header(&cmd, TPM_ST_SESSIONS, 0, TPM_CC_Load);

    /* parentHandle */
    tpm_cmd_write32(&cmd, parent_handle);

    /* 授权区域 */
    tpm_cmd_write32(&cmd, 1);
    tpm_cmd_write32(&cmd, TPM_RS_PW);
    tpm_cmd_write16(&cmd, 0);
    cmd.buffer[cmd.offset++] = 0x00;
    tpm_cmd_write16(&cmd, 0);

    /* inPrivate */
    tpm_cmd_write16(&cmd, private_len);
    memcpy(cmd.buffer + cmd.offset, private_blob, private_len);
    cmd.offset += private_len;

    /* inPublic */
    tpm_cmd_write16(&cmd, public_len);
    memcpy(cmd.buffer + cmd.offset, public_blob, public_len);
    cmd.offset += public_len;

    cmd.size = cmd.offset;

    /* 发送并接收loaded handle... */

    return 0;
}

/* TPM2_FlushContext - 释放密钥句柄 */
int tpm_flush_context(uint32_t handle) {
    TPM_CMD_BUFFER cmd = {0};

    tpm_build_header(&cmd, TPM_ST_NO_SESSIONS, 10, TPM_CC_FlushContext);
    tpm_cmd_write32(&cmd, handle);
    cmd.size = cmd.offset;

    /* 发送命令... */

    return 0;
}
