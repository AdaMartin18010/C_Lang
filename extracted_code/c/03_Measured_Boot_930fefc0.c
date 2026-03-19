/*
 * Auto-generated from: 03_System_Technology_Domains\03_Measured_Boot.md
 * Line: 220
 * Language: c
 * Block ID: 930fefc0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// TPM2 PCR_Extend 操作伪代码
void pcr_extend(TPM2_HANDLE pcr_handle, TPM2B_DIGEST *data) {
    // 1. 读取当前PCR值
    TPM2B_DIGEST current_pcr = tpm2_pcr_read(pcr_handle);

    // 2. 连接旧值与新数据
    TPM2B_DIGEST concat_data;
    memcpy(concat_data.buffer, current_pcr.buffer, current_pcr.size);
    memcpy(concat_data.buffer + current_pcr.size, data->buffer, data->size);
    concat_data.size = current_pcr.size + data->size;

    // 3. 计算哈希
    TPM2B_DIGEST new_pcr = hash_sha256(concat_data.buffer, concat_data.size);

    // 4. 写入PCR
    tpm2_pcr_write(pcr_handle, &new_pcr);
}
