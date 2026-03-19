/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\README.md
 * Line: 396
 * Language: c
 * Block ID: 2a69299f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* PKCS#11 (Cryptoki) 标准接口详解 */
#include <cryptoki.h>

/* 会话和对象管理 */
typedef CK_SESSION_HANDLE;      /* 会话句柄 */
typedef CK_OBJECT_HANDLE;       /* 对象(密钥/证书)句柄 */
typedef CK_SLOT_ID;             /* 插槽ID */

/* 密钥属性定义 */
#define CKA_TOKEN           0x00000001  /* 持久存储 */
#define CKA_PRIVATE         0x00000002  /* 需认证访问 */
#define CKA_SENSITIVE       0x00000100  /* 不可导出明文 */
#define CKA_EXTRACTABLE     0x00000162  /* 可导出 */
#define CKA_ENCRYPT         0x00000104  /* 可用于加密 */
#define CKA_DECRYPT         0x00000105  /* 可用于解密 */
#define CKA_SIGN            0x00000108  /* 可用于签名 */
#define CKA_VERIFY          0x00000109  /* 可用于验签 */
#define CKA_WRAP            0x00000106  /* 可包装其他密钥 */
#define CKA_UNWRAP          0x00000107  /* 可解包其他密钥 */

/* HSM密钥生成示例 */
CK_RV generate_hsm_key_pair(CK_SESSION_HANDLE session,
                            CK_OBJECT_HANDLE *pub_key_handle,
                            CK_OBJECT_HANDLE *priv_key_handle) {
    CK_MECHANISM mechanism = {
        CKM_RSA_PKCS_KEY_PAIR_GEN, NULL_PTR, 0
    };

    /* 公钥模板 */
    CK_ATTRIBUTE pub_template[] = {
        {CKA_TOKEN,     &false_val, sizeof(CK_BBOOL)},    /* 会话对象 */
        {CKA_ENCRYPT,   &true_val,  sizeof(CK_BBOOL)},
        {CKA_VERIFY,    &true_val,  sizeof(CK_BBOOL)},
        {CKA_WRAP,      &true_val,  sizeof(CK_BBOOL)},
        {CKA_MODULUS_BITS, &modulus_bits, sizeof(CK_ULONG)},
        {CKA_PUBLIC_EXPONENT, pub_exp, sizeof(pub_exp)}
    };

    /* 私钥模板 - 关键安全属性 */
    CK_ATTRIBUTE priv_template[] = {
        {CKA_TOKEN,     &true_val,  sizeof(CK_BBOOL)},    /* 持久存储 */
        {CKA_PRIVATE,   &true_val,  sizeof(CK_BBOOL)},    /* 需认证 */
        {CKA_SENSITIVE, &true_val,  sizeof(CK_BBOOL)},    /* 不可导出明文 */
        {CKA_EXTRACTABLE, &false_val, sizeof(CK_BBOOL)},  /* 不可提取 */
        {CKA_DECRYPT,   &true_val,  sizeof(CK_BBOOL)},
        {CKA_SIGN,      &true_val,  sizeof(CK_BBOOL)},
        {CKA_UNWRAP,    &true_val,  sizeof(CK_BBOOL)}
    };

    CK_RV rv = C_GenerateKeyPair(session, &mechanism,
                                  pub_template, 6,
                                  priv_template, 7,
                                  pub_key_handle, priv_key_handle);
    return rv;
}

/* 使用HSM进行数字签名 */
CK_RV hsm_sign_data(CK_SESSION_HANDLE session,
                    CK_OBJECT_HANDLE priv_key,
                    CK_BYTE_PTR data,
                    CK_ULONG data_len,
                    CK_BYTE_PTR signature,
                    CK_ULONG_PTR sig_len) {
    CK_MECHANISM mechanism = {
        CKM_SHA256_RSA_PKCS,  /* SHA-256 + RSA PKCS#1 v1.5 */
        NULL_PTR, 0
    };

    /* 初始化签名操作 */
    CK_RV rv = C_SignInit(session, &mechanism, priv_key);
    if (rv != CKR_OK) return rv;

    /* 执行签名 - 私钥永不离开HSM */
    rv = C_Sign(session, data, data_len, signature, sig_len);

    return rv;
}

/* HSM密钥包装 - 安全导出 */
CK_RV wrap_key_for_export(CK_SESSION_HANDLE session,
                          CK_OBJECT_HANDLE key_to_wrap,   /* 被包装密钥 */
                          CK_OBJECT_HANDLE wrapping_key,  /* 包装密钥(AES) */
                          CK_BYTE_PTR wrapped_key,
                          CK_ULONG_PTR wrapped_len) {
    CK_MECHANISM mechanism = {
        CKM_AES_GCM,        /* AES-GCM提供认证加密 */
        &gcm_params,
        sizeof(CK_GCM_PARAMS)
    };

    /* 包装操作 - 被包装密钥在HSM内加密后导出 */
    CK_RV rv = C_WrapKey(session, &mechanism,
                         wrapping_key,    /* 包装密钥 */
                         key_to_wrap,     /* 被包装密钥 */
                         wrapped_key,
                         wrapped_len);
    return rv;
}
