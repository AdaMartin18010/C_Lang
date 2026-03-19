/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\03_HSM_Integration.md
 * Line: 1077
 * Language: c
 * Block ID: 696ea524
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* hsm_engine.c - OpenSSL HSM引擎实现 */
#include <openssl/engine.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/bn.h>
#include "hsm_pkcs11.h"

#define HSM_ENGINE_ID "hsm_pkcs11"
#define HSM_ENGINE_NAME "HSM PKCS#11 Engine"

/* 引擎私有数据结构 */
typedef struct {
    CK_SESSION_HANDLE session;
    CK_OBJECT_HANDLE private_key;
    CK_OBJECT_HANDLE public_key;
} HSM_KEY_CTX;

static const char *hsm_library_path = NULL;
static CK_SESSION_HANDLE hsm_session = 0;

/* 引擎初始化 */
static int hsm_engine_init(ENGINE *e)
{
    (void)e;

    if (!hsm_library_path) {
        ERR_put_error(ERR_LIB_ENGINE, 0, ENGINE_R_INVALID_ARGUMENT,
                      __FILE__, __LINE__);
        return 0;
    }

    CK_RV rv = hsm_load_library(hsm_library_path);
    if (rv != CKR_OK) return 0;

    rv = hsm_initialize();
    if (rv != CKR_OK) {
        hsm_unload_library();
        return 0;
    }

    return 1;
}

/* 引擎销毁 */
static int hsm_engine_finish(ENGINE *e)
{
    (void)e;

    if (hsm_session) {
        hsm_logout(hsm_session);
        hsm_close_session(hsm_session);
        hsm_session = 0;
    }

    hsm_finalize();
    hsm_unload_library();

    return 1;
}

/* RSA私钥签名 (使用HSM) */
static int hsm_rsa_sign(int type, const unsigned char *m, unsigned int m_length,
                         unsigned char *sigret, unsigned int *siglen,
                         const RSA *rsa)
{
    HSM_KEY_CTX *ctx = RSA_get_ex_data(rsa, 0);
    if (!ctx) return 0;

    CK_MECHANISM_TYPE mech;
    switch (type) {
        case NID_sha256: mech = CKM_SHA256_RSA_PKCS; break;
        case NID_sha384: mech = CKM_SHA384_RSA_PKCS; break;
        case NID_sha512: mech = CKM_SHA512_RSA_PKCS; break;
        default: mech = CKM_RSA_PKCS; break;
    }

    CK_ULONG sig_len = *siglen;
    CK_RV rv = hsm_rsa_sign(ctx->session, ctx->private_key, mech,
                            m, m_length, sigret, &sig_len);
    if (rv != CKR_OK) return 0;

    *siglen = (unsigned int)sig_len;
    return 1;
}

/* RSA公钥解密 (使用HSM) */
static int hsm_rsa_decrypt(int flen, const unsigned char *from,
                            unsigned char *to, RSA *rsa, int padding)
{
    HSM_KEY_CTX *ctx = RSA_get_ex_data(rsa, 0);
    if (!ctx) return -1;

    if (padding != RSA_PKCS1_PADDING) {
        ERR_put_error(ERR_LIB_RSA, 0, RSA_R_UNKNOWN_PADDING_TYPE,
                      __FILE__, __LINE__);
        return -1;
    }

    CK_ULONG out_len = RSA_size(rsa);
    CK_RV rv = hsm_rsa_decrypt(ctx->session, ctx->private_key,
                                CKM_RSA_PKCS, from, flen, to, &out_len);
    if (rv != CKR_OK) return -1;

    return (int)out_len;
}

/* 设置HSM库路径 */
static int hsm_ctrl(ENGINE *e, int cmd, long i, void *p, void (*f)(void))
{
    (void)e; (void)i; (void)f;

    switch (cmd) {
        case 1: /* 设置库路径 */
            if (hsm_library_path) {
                free((void*)hsm_library_path);
            }
            hsm_library_path = strdup((const char*)p);
            return 1;
        default:
            return 0;
    }
}

/* 绑定引擎 */
static int bind_engine(ENGINE *e)
{
    if (!ENGINE_set_id(e, HSM_ENGINE_ID) ||
        !ENGINE_set_name(e, HSM_ENGINE_NAME) ||
        !ENGINE_set_init_function(e, hsm_engine_init) ||
        !ENGINE_set_finish_function(e, hsm_engine_finish) ||
        !ENGINE_set_ctrl_function(e, hsm_ctrl)) {
        return 0;
    }

    return 1;
}

IMPLEMENT_DYNAMIC_CHECK_FN()
IMPLEMENT_DYNAMIC_BIND_FN(bind_engine)
