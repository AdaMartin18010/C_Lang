/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\03_HSM_Integration.md
 * Line: 1222
 * Language: c
 * Block ID: 10a1c5ee
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* hsm_example.c - HSM使用示例 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include "hsm_pkcs11.h"

int main(int argc, char *argv[])
{
    CK_RV rv;

    printf("=== HSM PKCS#11 演示 ===\n\n");

    /* 加载PKCS#11库 */
    const char *library_path = (argc > 1) ? argv[1] : "/usr/lib/libCryptoki2.so";
    printf("加载库: %s\n", library_path);

    rv = hsm_load_library(library_path);
    CHECK_RV(rv, "加载库失败");

    /* 初始化 */
    rv = hsm_initialize();
    CHECK_RV(rv, "初始化失败");

    /* 获取插槽列表 */
    CK_SLOT_ID slots[10];
    CK_ULONG slot_count = 10;
    rv = hsm_get_slot_list(CK_TRUE, slots, &slot_count);
    CHECK_RV(rv, "获取插槽失败");

    printf("发现 %lu 个带令牌的插槽\n", slot_count);
    if (slot_count == 0) {
        fprintf(stderr, "未找到HSM设备\n");
        goto cleanup;
    }

    /* 使用第一个插槽 */
    CK_SLOT_ID slot = slots[0];

    /* 打开读写会话 */
    CK_SESSION_HANDLE session;
    rv = hsm_open_session(slot, CKF_RW_SESSION | CKF_SERIAL_SESSION, &session);
    CHECK_RV(rv, "打开会话失败");

    printf("会话已打开\n");

    /* 用户登录 */
    const char *pin = (argc > 2) ? argv[2] : "1234";
    rv = hsm_login(session, CKU_USER, pin);
    CHECK_RV(rv, "登录失败");

    printf("登录成功\n\n");

    /* 生成AES密钥 */
    printf("生成AES-256密钥...\n");
    CK_OBJECT_HANDLE aes_key;
    rv = hsm_generate_aes_key(session, 32, &aes_key);
    CHECK_RV(rv, "生成AES密钥失败");
    printf("AES密钥句柄: %lu\n", (unsigned long)aes_key);

    /* AES加密测试 */
    CK_BYTE plaintext[] = "Hello, HSM! This is a secret message.";
    CK_BYTE ciphertext[256] = {0};
    CK_BYTE iv[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                       0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10};
    CK_ULONG ciphertext_len = sizeof(ciphertext);

    rv = hsm_aes_encrypt(session, aes_key, iv, sizeof(iv),
                         plaintext, sizeof(plaintext)-1,
                         ciphertext, &ciphertext_len);
    CHECK_RV(rv, "AES加密失败");
    printf("加密完成，密文长度: %lu\n", ciphertext_len);

    /* AES解密测试 */
    CK_BYTE decrypted[256] = {0};
    CK_ULONG decrypted_len = sizeof(decrypted);

    rv = hsm_aes_decrypt(session, aes_key, iv, sizeof(iv),
                         ciphertext, ciphertext_len,
                         decrypted, &decrypted_len);
    CHECK_RV(rv, "AES解密失败");
    decrypted[decrypted_len] = '\0';
    printf("解密完成: %s\n\n", decrypted);

    /* 生成RSA密钥对 */
    printf("生成RSA-2048密钥对...\n");
    CK_OBJECT_HANDLE rsa_pub, rsa_priv;
    rv = hsm_generate_rsa_keypair(session, 2048, &rsa_pub, &rsa_priv);
    CHECK_RV(rv, "生成RSA密钥对失败");
    printf("RSA公钥句柄: %lu, 私钥句柄: %lu\n",
           (unsigned long)rsa_pub, (unsigned long)rsa_priv);

    /* RSA签名测试 */
    CK_BYTE data[] = "Data to be signed";
    CK_BYTE signature[256] = {0};
    CK_ULONG signature_len = sizeof(signature);

    rv = hsm_rsa_sign(session, rsa_priv, CKM_SHA256_RSA_PKCS,
                      data, sizeof(data)-1, signature, &signature_len);
    CHECK_RV(rv, "RSA签名失败");
    printf("签名完成，签名长度: %lu\n", signature_len);

    /* RSA验签测试 */
    rv = hsm_rsa_verify(session, rsa_pub, CKM_SHA256_RSA_PKCS,
                        data, sizeof(data)-1, signature, signature_len);
    if (rv == CKR_OK) {
        printf("签名验证成功!\n\n");
    } else {
        printf("签名验证失败: %s\n\n", pkcs11_error_string(rv));
    }

    /* 清理 */
    hsm_destroy_key(session, aes_key);
    hsm_destroy_key(session, rsa_pub);
    hsm_destroy_key(session, rsa_priv);

    hsm_logout(session);
    hsm_close_session(session);

cleanup:
    hsm_finalize();
    hsm_unload_library();

    printf("演示完成\n");
    return (rv == CKR_OK) ? 0 : 1;
}
