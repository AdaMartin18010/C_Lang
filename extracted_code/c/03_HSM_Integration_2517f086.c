/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\03_HSM_Integration.md
 * Line: 542
 * Language: c
 * Block ID: 2517f086
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* hsm_pkcs11.h - HSM PKCS#11接口 */
#ifndef HSM_PKCS11_H
#define HSM_PKCS11_H

#include "pkcs11_types.h"

/* 错误处理宏 */
#define CHECK_RV(rv, msg) \
    do { \
        if ((rv) != CKR_OK) { \
            fprintf(stderr, "%s: 0x%08lx - %s\n", msg, (unsigned long)(rv), pkcs11_error_string(rv)); \
            goto cleanup; \
        } \
    } while(0)

/* 函数声明 */
CK_RV hsm_load_library(const char *library_path);
CK_RV hsm_unload_library(void);
CK_RV hsm_initialize(void);
CK_RV hsm_finalize(void);
CK_RV hsm_get_slot_list(CK_BBOOL token_present, CK_SLOT_ID *slots, CK_ULONG *count);
CK_RV hsm_open_session(CK_SLOT_ID slot_id, CK_FLAGS flags, CK_SESSION_HANDLE *session);
CK_RV hsm_close_session(CK_SESSION_HANDLE session);
CK_RV hsm_login(CK_SESSION_HANDLE session, CK_USER_TYPE user_type, const char *pin);
CK_RV hsm_logout(CK_SESSION_HANDLE session);
const char* pkcs11_error_string(CK_RV rv);

#endif /* HSM_PKCS11_H */
