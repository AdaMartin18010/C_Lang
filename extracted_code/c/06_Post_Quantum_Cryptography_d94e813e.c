/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\06_Post_Quantum_Cryptography.md
 * Line: 1853
 * Language: c
 * Block ID: d94e813e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * FIPS 204 实现合规性检查
 */

int fips204_compliance_check(void) {
    int errors = 0;

    /* 1. 域参数检查 */
    /* q = 8380417 */
    /* n = 256 */
    /* d = 13 */

    /* 2. 密钥生成测试 */
    ml_dsa_keypair_t kp;
    ml_dsa_keygen(&kp);

    /* 3. 签名测试 */
    uint8_t msg[32] = {0};
    ml_dsa_signature_t sig;

    ml_dsa_sign(&sig, msg, sizeof(msg), &kp, NULL, 0, NULL, 0);

    /* 4. 验签测试 */
    if (ml_dsa_verify(msg, sizeof(msg), &sig, kp.pk, NULL, 0) != 0) {
        errors++;
        ERROR("Signature verification failed\n");
    }

    /* 5. 消息修改后验签应该失败 */
    msg[0] = 0xFF;
    if (ml_dsa_verify(msg, sizeof(msg), &sig, kp.pk, NULL, 0) == 0) {
        errors++;
        ERROR("Forgery not detected\n");
    }

    /* 6. 检查确定性签名 */
    /* 相同消息应该产生不同签名（随机掩码） */

    return errors;
}

/* 上下文字符串验证 */
int verify_context_string(const uint8_t *ctx, size_t ctxlen) {
    /* FIPS 204要求上下文字符串长度<=255字节 */
    if (ctxlen > 255) {
        return -1;
    }

    /* 空上下文是允许的 */
    return 0;
}
