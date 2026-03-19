/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\01_TPM2_TSS.md
 * Line: 172
 * Language: c
 * Block ID: 2663a8c6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 命令缓冲区结构 */
typedef struct {
    uint8_t buffer[4096];
    size_t  size;
    size_t  offset;
} TPM_CMD_BUFFER;

/* 写入基本类型 */
static void tpm_cmd_write16(TPM_CMD_BUFFER *cmd, uint16_t val) {
    cmd->buffer[cmd->offset++] = (val >> 8) & 0xFF;
    cmd->buffer[cmd->offset++] = val & 0xFF;
}

static void tpm_cmd_write32(TPM_CMD_BUFFER *cmd, uint32_t val) {
    cmd->buffer[cmd->offset++] = (val >> 24) & 0xFF;
    cmd->buffer[cmd->offset++] = (val >> 16) & 0xFF;
    cmd->buffer[cmd->offset++] = (val >> 8) & 0xFF;
    cmd->buffer[cmd->offset++] = val & 0xFF;
}

static void tpm_cmd_write_bytes(TPM_CMD_BUFFER *cmd,
                                 const uint8_t *data, uint16_t len) {
    tpm_cmd_write16(cmd, len);
    memcpy(cmd->buffer + cmd->offset, data, len);
    cmd->offset += len;
}

/* 构建命令头 */
void tpm_build_header(TPM_CMD_BUFFER *cmd, TPM_ST tag, uint32_t size, TPM_CC cc) {
    cmd->offset = 0;
    tpm_cmd_write16(cmd, tag);
    tpm_cmd_write32(cmd, size);
    tpm_cmd_write32(cmd, cc);
}

/* 响应解析 */
typedef struct {
    const uint8_t *buffer;
    size_t         size;
    size_t         offset;
} TPM_RSP_BUFFER;

static uint32_t tpm_rsp_read32(TPM_RSP_BUFFER *rsp) {
    uint32_t val = ((uint32_t)rsp->buffer[rsp->offset] << 24) |
                   ((uint32_t)rsp->buffer[rsp->offset + 1] << 16) |
                   ((uint32_t)rsp->buffer[rsp->offset + 2] << 8) |
                   rsp->buffer[rsp->offset + 3];
    rsp->offset += 4;
    return val;
}

static uint16_t tpm_rsp_read16(TPM_RSP_BUFFER *rsp) {
    uint16_t val = ((uint16_t)rsp->buffer[rsp->offset] << 8) |
                   rsp->buffer[rsp->offset + 1];
    rsp->offset += 2;
    return val;
}
