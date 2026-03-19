/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\03_HSM_Integration.md
 * Line: 905
 * Language: c
 * Block ID: 8a9c2a2d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* hsm_crypto.c - 加解密和签名实现 */
#include "hsm_pkcs11.h"

/* AES加密 */
CK_RV hsm_aes_encrypt(CK_SESSION_HANDLE session,
                       CK_OBJECT_HANDLE key_handle,
                       const CK_BYTE *iv,
                       CK_ULONG iv_len,
                       const CK_BYTE *plaintext,
                       CK_ULONG plaintext_len,
                       CK_BYTE *ciphertext,
                       CK_ULONG *ciphertext_len)
{
    CK_MECHANISM mechanism;

    if (iv && iv_len > 0) {
        mechanism.mechanism = CKM_AES_CBC_PAD;
        mechanism.pParameter = (void*)iv;
        mechanism.ulParameterLen = iv_len;
    } else {
        mechanism.mechanism = CKM_AES_ECB;
        mechanism.pParameter = NULL_PTR;
        mechanism.ulParameterLen = 0;
    }

    CK_RV rv = p11->C_EncryptInit(session, &mechanism, key_handle);
    if (rv != CKR_OK) return rv;

    rv = p11->C_Encrypt(session, (CK_BYTE_PTR)plaintext, plaintext_len,
                        ciphertext, ciphertext_len);

    return rv;
}

/* AES解密 */
CK_RV hsm_aes_decrypt(CK_SESSION_HANDLE session,
                       CK_OBJECT_HANDLE key_handle,
                       const CK_BYTE *iv,
                       CK_ULONG iv_len,
                       const CK_BYTE *ciphertext,
                       CK_ULONG ciphertext_len,
                       CK_BYTE *plaintext,
                       CK_ULONG *plaintext_len)
{
    CK_MECHANISM mechanism;

    if (iv && iv_len > 0) {
        mechanism.mechanism = CKM_AES_CBC_PAD;
        mechanism.pParameter = (void*)iv;
        mechanism.ulParameterLen = iv_len;
    } else {
        mechanism.mechanism = CKM_AES_ECB;
        mechanism.pParameter = NULL_PTR;
        mechanism.ulParameterLen = 0;
    }

    CK_RV rv = p11->C_DecryptInit(session, &mechanism, key_handle);
    if (rv != CKR_OK) return rv;

    rv = p11->C_Decrypt(session, (CK_BYTE_PTR)ciphertext, ciphertext_len,
                        plaintext, plaintext_len);

    return rv;
}

/* RSA签名 */
CK_RV hsm_rsa_sign(CK_SESSION_HANDLE session,
                    CK_OBJECT_HANDLE priv_key_handle,
                    CK_MECHANISM_TYPE mechanism_type,
                    const CK_BYTE *data,
                    CK_ULONG data_len,
                    CK_BYTE *signature,
                    CK_ULONG *signature_len)
{
    CK_MECHANISM mechanism = { mechanism_type, NULL_PTR, 0 };

    CK_RV rv = p11->C_SignInit(session, &mechanism, priv_key_handle);
    if (rv != CKR_OK) return rv;

    rv = p11->C_Sign(session, (CK_BYTE_PTR)data, data_len,
                     signature, signature_len);

    return rv;
}

/* RSA验签 */
CK_RV hsm_rsa_verify(CK_SESSION_HANDLE session,
                      CK_OBJECT_HANDLE pub_key_handle,
                      CK_MECHANISM_TYPE mechanism_type,
                      const CK_BYTE *data,
                      CK_ULONG data_len,
                      const CK_BYTE *signature,
                      CK_ULONG signature_len)
{
    CK_MECHANISM mechanism = { mechanism_type, NULL_PTR, 0 };

    CK_RV rv = p11->C_VerifyInit(session, &mechanism, pub_key_handle);
    if (rv != CKR_OK) return rv;

    rv = p11->C_Verify(session, (CK_BYTE_PTR)data, data_len,
                       (CK_BYTE_PTR)signature, signature_len);

    return rv;
}

/* RSA加密 */
CK_RV hsm_rsa_encrypt(CK_SESSION_HANDLE session,
                       CK_OBJECT_HANDLE pub_key_handle,
                       CK_MECHANISM_TYPE mechanism_type,
                       const CK_BYTE *plaintext,
                       CK_ULONG plaintext_len,
                       CK_BYTE *ciphertext,
                       CK_ULONG *ciphertext_len)
{
    CK_MECHANISM mechanism = { mechanism_type, NULL_PTR, 0 };

    CK_RV rv = p11->C_EncryptInit(session, &mechanism, pub_key_handle);
    if (rv != CKR_OK) return rv;

    rv = p11->C_Encrypt(session, (CK_BYTE_PTR)plaintext, plaintext_len,
                        ciphertext, ciphertext_len);

    return rv;
}

/* RSA解密 */
CK_RV hsm_rsa_decrypt(CK_SESSION_HANDLE session,
                       CK_OBJECT_HANDLE priv_key_handle,
                       CK_MECHANISM_TYPE mechanism_type,
                       const CK_BYTE *ciphertext,
                       CK_ULONG ciphertext_len,
                       CK_BYTE *plaintext,
                       CK_ULONG *plaintext_len)
{
    CK_MECHANISM mechanism = { mechanism_type, NULL_PTR, 0 };

    CK_RV rv = p11->C_DecryptInit(session, &mechanism, priv_key_handle);
    if (rv != CKR_OK) return rv;

    rv = p11->C_Decrypt(session, (CK_BYTE_PTR)ciphertext, ciphertext_len,
                        plaintext, plaintext_len);

    return rv;
}

/* 哈希运算 */
CK_RV hsm_digest(CK_SESSION_HANDLE session,
                  CK_MECHANISM_TYPE mechanism_type,
                  const CK_BYTE *data,
                  CK_ULONG data_len,
                  CK_BYTE *digest,
                  CK_ULONG *digest_len)
{
    CK_MECHANISM mechanism = { mechanism_type, NULL_PTR, 0 };

    CK_RV rv = p11->C_DigestInit(session, &mechanism);
    if (rv != CKR_OK) return rv;

    rv = p11->C_Digest(session, (CK_BYTE_PTR)data, data_len,
                       digest, digest_len);

    return rv;
}
