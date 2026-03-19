/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\02_Key_Sealing.md
 * Line: 458
 * Language: c
 * Block ID: 463f55e3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 解封数据 - 需要满足密封时的策略条件 */
int tpm_unseal_data(uint32_t parent_handle,
                    const SealedDataPackage *sealed,
                    const char *auth_password,        /* 可选密码授权 */
                    uint8_t *data,                     /* 输出缓冲区 */
                    uint16_t *data_len) {

    /* 1. 加载密封对象 */
    uint32_t object_handle;
    int ret = tpm_load_key(parent_handle,
                           sealed->private.buffer, sealed->private.size,
                           sealed->public.buffer, sealed->public.size,
                           &object_handle);
    if (ret != 0) return ret;

    /* 2. 建立策略会话（如果需要） */
    uint32_t policy_session = 0;

    if (sealed->policy.type == POLICY_PCR) {
        /* 启动策略会话 */
        TPM2B_DIGEST nonce_tpm;
        ret = tpm_start_auth_session_policy(&policy_session, &nonce_tpm);
        if (ret != 0) goto cleanup;

        /* 设置PCR策略 */
        ret = tpm_policy_pcr(policy_session,
                            &sealed->policy.pcr.pcr_select,
                            &sealed->policy.pcr.pcr_digest);
        if (ret != 0) goto cleanup;

        /* 验证策略摘要 */
        TPM2B_DIGEST current_policy;
        ret = tpm_policy_get_digest(policy_session, &current_policy);
        /* 与sealed对象中的authPolicy比较... */
    }

    /* 3. 调用TPM2_Unseal */
    TPM_CMD_BUFFER cmd = {0};
    tpm_build_header(&cmd, TPM_ST_SESSIONS, 0, TPM_CC_Unseal);

    /* itemHandle */
    tpm_cmd_write32(&cmd, object_handle);

    /* 授权区域 */
    tpm_cmd_write32(&cmd, 1);

    if (policy_session) {
        /* 使用策略会话 */
        tpm_cmd_write32(&cmd, policy_session);
        tpm_cmd_write16(&cmd, nonce_tpm.size);
        memcpy(cmd.buffer + cmd.offset, nonce_tpm.buffer, nonce_tpm.size);
        cmd.offset += nonce_tpm.size;
        cmd.buffer[cmd.offset++] = 0x00;  /* sessionAttributes */
        tpm_cmd_write16(&cmd, 0);  /* hmac */
    } else {
        /* 使用密码授权 */
        tpm_cmd_write32(&cmd, TPM_RS_PW);
        tpm_cmd_write16(&cmd, 0);
        cmd.buffer[cmd.offset++] = 0x00;
        if (auth_password) {
            uint16_t pwd_len = strlen(auth_password);
            tpm_cmd_write16(&cmd, pwd_len);
            memcpy(cmd.buffer + cmd.offset, auth_password, pwd_len);
            cmd.offset += pwd_len;
        } else {
            tpm_cmd_write16(&cmd, 0);
        }
    }

    cmd.size = cmd.offset;

    /* 4. 发送命令 */
    uint8_t rsp[512];
    size_t rsp_len = sizeof(rsp);
    ret = tcti_transmit_receive(cmd.buffer, cmd.size, rsp, &rsp_len);

    if (ret == 0) {
        /* 解析解封的数据... */
        TPM_RSP_BUFFER rp = {rsp, rsp_len, 0};
        /* 提取outData... */
    }

cleanup:
    /* 5. 清理 */
    if (policy_session) {
        tpm_flush_context(policy_session);
    }
    tpm_flush_context(object_handle);

    return ret;
}
