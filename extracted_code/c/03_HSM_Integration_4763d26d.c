/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\03_HSM_Integration.md
 * Line: 774
 * Language: c
 * Block ID: 4763d26d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* hsm_key_mgmt.c - 密钥管理实现 */
#include "hsm_pkcs11.h"

/* 生成AES密钥 */
CK_RV hsm_generate_aes_key(CK_SESSION_HANDLE session,
                            CK_ULONG key_length,
                            CK_OBJECT_HANDLE *key_handle)
{
    CK_MECHANISM mechanism = { CKM_AES_KEY_GEN, NULL_PTR, 0 };

    CK_BBOOL true_val = CK_TRUE;
    CK_BBOOL false_val = CK_FALSE;

    CK_ATTRIBUTE template[] = {
        {CKA_CLASS, &(CK_OBJECT_CLASS){CKO_SECRET_KEY}, sizeof(CK_OBJECT_CLASS)},
        {CKA_KEY_TYPE, &(CK_KEY_TYPE){CKK_AES}, sizeof(CK_KEY_TYPE)},
        {CKA_VALUE_LEN, &key_length, sizeof(key_length)},
        {CKA_ENCRYPT, &true_val, sizeof(true_val)},
        {CKA_DECRYPT, &true_val, sizeof(true_val)},
        {CKA_SIGN, &true_val, sizeof(true_val)},
        {CKA_VERIFY, &true_val, sizeof(true_val)},
        {CKA_WRAP, &true_val, sizeof(true_val)},
        {CKA_UNWRAP, &true_val, sizeof(true_val)},
        {CKA_SENSITIVE, &true_val, sizeof(true_val)},
        {CKA_EXTRACTABLE, &false_val, sizeof(false_val)},
        {CKA_TOKEN, &true_val, sizeof(true_val)},
        {CKA_PRIVATE, &true_val, sizeof(true_val)},
        {CKA_LABEL, "AES Key", 7}
    };

    return p11->C_GenerateKey(session, &mechanism, template,
                              sizeof(template)/sizeof(CK_ATTRIBUTE), key_handle);
}

/* 生成RSA密钥对 */
CK_RV hsm_generate_rsa_keypair(CK_SESSION_HANDLE session,
                                CK_ULONG modulus_bits,
                                CK_OBJECT_HANDLE *pub_key_handle,
                                CK_OBJECT_HANDLE *priv_key_handle)
{
    CK_MECHANISM mechanism = { CKM_RSA_PKCS_KEY_PAIR_GEN, NULL_PTR, 0 };

    CK_BBOOL true_val = CK_TRUE;
    CK_BBOOL false_val = CK_FALSE;
    CK_BYTE public_exponent[] = { 0x01, 0x00, 0x01 }; /* 65537 */

    /* 公钥模板 */
    CK_ATTRIBUTE pub_template[] = {
        {CKA_CLASS, &(CK_OBJECT_CLASS){CKO_PUBLIC_KEY}, sizeof(CK_OBJECT_CLASS)},
        {CKA_KEY_TYPE, &(CK_KEY_TYPE){CKK_RSA}, sizeof(CK_KEY_TYPE)},
        {CKA_MODULUS_BITS, &modulus_bits, sizeof(modulus_bits)},
        {CKA_PUBLIC_EXPONENT, public_exponent, sizeof(public_exponent)},
        {CKA_ENCRYPT, &true_val, sizeof(true_val)},
        {CKA_VERIFY, &true_val, sizeof(true_val)},
        {CKA_WRAP, &true_val, sizeof(true_val)},
        {CKA_TOKEN, &true_val, sizeof(true_val)},
        {CKA_LABEL, "RSA Public Key", 14}
    };

    /* 私钥模板 */
    CK_ATTRIBUTE priv_template[] = {
        {CKA_CLASS, &(CK_OBJECT_CLASS){CKO_PRIVATE_KEY}, sizeof(CK_OBJECT_CLASS)},
        {CKA_KEY_TYPE, &(CK_KEY_TYPE){CKK_RSA}, sizeof(CK_KEY_TYPE)},
        {CKA_DECRYPT, &true_val, sizeof(true_val)},
        {CKA_SIGN, &true_val, sizeof(true_val)},
        {CKA_UNWRAP, &true_val, sizeof(true_val)},
        {CKA_SENSITIVE, &true_val, sizeof(true_val)},
        {CKA_EXTRACTABLE, &false_val, sizeof(false_val)},
        {CKA_TOKEN, &true_val, sizeof(true_val)},
        {CKA_PRIVATE, &true_val, sizeof(true_val)},
        {CKA_LABEL, "RSA Private Key", 15}
    };

    return p11->C_GenerateKeyPair(session, &mechanism,
                                   pub_template, sizeof(pub_template)/sizeof(CK_ATTRIBUTE),
                                   priv_template, sizeof(priv_template)/sizeof(CK_ATTRIBUTE),
                                   pub_key_handle, priv_key_handle);
}

/* 查找密钥 */
CK_RV hsm_find_key(CK_SESSION_HANDLE session,
                    const char *label,
                    CK_OBJECT_CLASS key_class,
                    CK_KEY_TYPE key_type,
                    CK_OBJECT_HANDLE *key_handle)
{
    CK_ATTRIBUTE template[] = {
        {CKA_CLASS, &key_class, sizeof(key_class)},
        {CKA_KEY_TYPE, &key_type, sizeof(key_type)},
        {CKA_LABEL, (void*)label, label ? (CK_ULONG)strlen(label) : 0}
    };

    CK_RV rv = p11->C_FindObjectsInit(session, template,
                                       label ? 3 : 2);
    if (rv != CKR_OK) return rv;

    CK_ULONG count = 0;
    rv = p11->C_FindObjects(session, key_handle, 1, &count);

    p11->C_FindObjectsFinal(session);

    if (rv == CKR_OK && count == 0) {
        return CKR_OBJECT_HANDLE_INVALID;
    }

    return rv;
}

/* 删除密钥 */
CK_RV hsm_destroy_key(CK_SESSION_HANDLE session, CK_OBJECT_HANDLE key_handle)
{
    return p11->C_DestroyObject(session, key_handle);
}

/* 获取密钥属性 */
CK_RV hsm_get_key_attribute(CK_SESSION_HANDLE session,
                             CK_OBJECT_HANDLE key_handle,
                             CK_ATTRIBUTE_TYPE attr_type,
                             void *value,
                             CK_ULONG *value_len)
{
    CK_ATTRIBUTE attr = { attr_type, value, *value_len };
    CK_RV rv = p11->C_GetAttributeValue(session, key_handle, &attr, 1);
    *value_len = attr.ulValueLen;
    return rv;
}
