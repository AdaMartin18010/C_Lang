/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\02_Key_Sealing.md
 * Line: 195
 * Language: c
 * Block ID: f24d6f16
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 读取多个PCR并计算复合摘要 */
int tpm_pcr_read_digest(const TPML_PCR_SELECTION *pcr_select,
                        TPM2B_DIGEST *pcr_digest) {
    TPM_CMD_BUFFER cmd = {0};

    /* TPM2_PCR_Read */
    tpm_build_header(&cmd, TPM_ST_NO_SESSIONS, 0, TPM_CC_PCR_Read);

    /* 序列化pcrSelect */
    tpm_cmd_write32(&cmd, pcr_select->count);
    for (uint32_t i = 0; i < pcr_select->count; i++) {
        TPMS_PCR_SELECTION *sel = &pcr_select->pcrSelections[i];
        tpm_cmd_write16(&cmd, sel->hash);
        cmd.buffer[cmd.offset++] = sel->sizeofSelect;
        memcpy(cmd.buffer + cmd.offset, sel->pcrSelect, sel->sizeofSelect);
        cmd.offset += sel->sizeofSelect;
    }

    cmd.size = cmd.offset;

    /* 发送命令... */
    uint8_t rsp[1024];
    size_t rsp_len = sizeof(rsp);
    int ret = tcti_transmit_receive(cmd.buffer, cmd.size, rsp, &rsp_len);
    if (ret != 0) return ret;

    /* 解析返回的PCR值并计算摘要... */

    return 0;
}

/* 计算PCR策略摘要
 * policy_digest = hash(pcr_digest | policy_ref)
 */
void compute_pcr_policy_digest(const TPM2B_DIGEST *pcr_digest,
                               const TPM2B_DIGEST *policy_ref,
                               uint16_t hash_alg,
                               TPM2B_DIGEST *policy_digest) {
    /* 使用指定的hash算法计算策略摘要 */
    uint8_t buffer[128];
    uint16_t len = 0;

    /* 序列化PCR摘要 */
    buffer[len++] = (pcr_digest->size >> 8) & 0xFF;
    buffer[len++] = pcr_digest->size & 0xFF;
    memcpy(buffer + len, pcr_digest->buffer, pcr_digest->size);
    len += pcr_digest->size;

    /* 添加policyRef */
    if (policy_ref && policy_ref->size > 0) {
        buffer[len++] = (policy_ref->size >> 8) & 0xFF;
        buffer[len++] = policy_ref->size & 0xFF;
        memcpy(buffer + len, policy_ref->buffer, policy_ref->size);
        len += policy_ref->size;
    }

    /* 计算hash */
    hash_compute(hash_alg, buffer, len, policy_digest->buffer,
                 &policy_digest->size);
}
