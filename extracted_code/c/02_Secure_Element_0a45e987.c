/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\02_Secure_Element.md
 * Line: 620
 * Language: c
 * Block ID: 0a45e987
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* se_crypto.c - 密钥操作示例 */
#include "se_pcsc.h"

/* 在安全元件内生成AES密钥 */
SEError se_generate_aes_key(SEContext *ctx, uint8_t keyId, uint8_t keySize)
{
    /* 构造GenerateKey APDU (GP标准) */
    APDUCommand cmd = {
        .cla = 0x80,  /* 安全报文或专有CLA */
        .ins = 0xD4,  /* GENERATE KEY */
        .p1 = keyId,
        .p2 = 0x80 | keySize,  /* AES密钥类型 */
        .lc = 0
    };

    APDUResponse resp;
    return se_transmit(ctx, &cmd, &resp);
}

/* 使用SE内的AES密钥加密 */
SEError se_aes_encrypt(SEContext *ctx, uint8_t keyId,
                        const uint8_t *plaintext, uint8_t *ciphertext,
                        size_t length)
{
    /* 选择密钥文件 */
    APDUCommand selectCmd = {
        .cla = 0x00,
        .ins = 0xA4,  /* SELECT */
        .p1 = 0x02,   /* 选择EF */
        .p2 = 0x0C,   /* 返回FCI */
        .lc = 0x02,
        .data = {0x00, keyId}
    };

    APDUResponse resp;
    se_transmit(ctx, &selectCmd, &resp);

    /* 执行加密 */
    APDUCommand encCmd = {
        .cla = 0x00,
        .ins = 0x2A,  /* PERFORM SECURITY OPERATION */
        .p1 = 0x80,   /* 加密 */
        .p2 = 0x86,
        .lc = (uint8_t)length
    };
    memcpy(encCmd.data, plaintext, length);
    encCmd.le = (uint8_t)length;

    SEError err = se_transmit(ctx, &encCmd, &resp);
    if (err == SE_SUCCESS && resp.sw1 == 0x90) {
        memcpy(ciphertext, resp.data, resp.dataLen);
    }

    return err;
}

/* 数字签名示例 */
SEError se_sign_data(SEContext *ctx, uint8_t keyId,
                      const uint8_t *hash, uint8_t hashLen,
                      uint8_t *signature, size_t *sigLen)
{
    /* 设置哈希值 */
    APDUCommand setHash = {
        .cla = 0x00,
        .ins = 0x22,  /* MANAGE SECURITY ENVIRONMENT */
        .p1 = 0x41,   /* 计算签名模板 */
        .p2 = 0xB6,
        .lc = hashLen + 2,
        .data = {0x90, hashLen}
    };
    memcpy(setHash.data + 2, hash, hashLen);

    APDUResponse resp;
    se_transmit(ctx, &setHash, &resp);

    /* 执行签名 */
    APDUCommand signCmd = {
        .cla = 0x00,
        .ins = 0x2A,  /* PERFORM SECURITY OPERATION */
        .p1 = 0x9E,   /* 签名 */
        .p2 = 0x9A
    };

    SEError err = se_transmit(ctx, &signCmd, &resp);
    if (err == SE_SUCCESS && resp.sw1 == 0x90) {
        memcpy(signature, resp.data, resp.dataLen);
        *sigLen = resp.dataLen;
    }

    return err;
}
