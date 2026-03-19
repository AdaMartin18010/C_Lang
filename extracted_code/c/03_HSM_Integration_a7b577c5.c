/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\03_HSM_Integration.md
 * Line: 573
 * Language: c
 * Block ID: a7b577c5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* hsm_pkcs11.c - PKCS#11实现 */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include "hsm_pkcs11.h"

/* PKCS#11函数指针类型定义 */
typedef CK_RV (*CK_C_GetFunctionList_t)(CK_FUNCTION_LIST_PTR_PTR);

/* 全局变量 */
static void *pkcs11_lib = NULL;
static CK_FUNCTION_LIST_PTR p11 = NULL;

/* 错误代码转字符串 */
const char* pkcs11_error_string(CK_RV rv)
{
    switch (rv) {
        case CKR_OK: return "CKR_OK";
        case CKR_CANCEL: return "CKR_CANCEL";
        case CKR_HOST_MEMORY: return "CKR_HOST_MEMORY";
        case CKR_SLOT_ID_INVALID: return "CKR_SLOT_ID_INVALID";
        case CKR_GENERAL_ERROR: return "CKR_GENERAL_ERROR";
        case CKR_FUNCTION_FAILED: return "CKR_FUNCTION_FAILED";
        case CKR_ARGUMENTS_BAD: return "CKR_ARGUMENTS_BAD";
        case CKR_ATTRIBUTE_READ_ONLY: return "CKR_ATTRIBUTE_READ_ONLY";
        case CKR_ATTRIBUTE_SENSITIVE: return "CKR_ATTRIBUTE_SENSITIVE";
        case CKR_ATTRIBUTE_TYPE_INVALID: return "CKR_ATTRIBUTE_TYPE_INVALID";
        case CKR_ATTRIBUTE_VALUE_INVALID: return "CKR_ATTRIBUTE_VALUE_INVALID";
        case CKR_DATA_INVALID: return "CKR_DATA_INVALID";
        case CKR_DATA_LEN_RANGE: return "CKR_DATA_LEN_RANGE";
        case CKR_DEVICE_ERROR: return "CKR_DEVICE_ERROR";
        case CKR_DEVICE_MEMORY: return "CKR_DEVICE_MEMORY";
        case CKR_DEVICE_REMOVED: return "CKR_DEVICE_REMOVED";
        case CKR_ENCRYPTED_DATA_INVALID: return "CKR_ENCRYPTED_DATA_INVALID";
        case CKR_ENCRYPTED_DATA_LEN_RANGE: return "CKR_ENCRYPTED_DATA_LEN_RANGE";
        case CKR_FUNCTION_CANCELED: return "CKR_FUNCTION_CANCELED";
        case CKR_FUNCTION_NOT_SUPPORTED: return "CKR_FUNCTION_NOT_SUPPORTED";
        case CKR_KEY_HANDLE_INVALID: return "CKR_KEY_HANDLE_INVALID";
        case CKR_KEY_SIZE_RANGE: return "CKR_KEY_SIZE_RANGE";
        case CKR_KEY_TYPE_INCONSISTENT: return "CKR_KEY_TYPE_INCONSISTENT";
        case CKR_KEY_NOT_NEEDED: return "CKR_KEY_NOT_NEEDED";
        case CKR_KEY_CHANGED: return "CKR_KEY_CHANGED";
        case CKR_KEY_NEEDED: return "CKR_KEY_NEEDED";
        case CKR_KEY_INDIGESTIBLE: return "CKR_KEY_INDIGESTIBLE";
        case CKR_KEY_FUNCTION_NOT_PERMITTED: return "CKR_KEY_FUNCTION_NOT_PERMITTED";
        case CKR_KEY_NOT_WRAPPABLE: return "CKR_KEY_NOT_WRAPPABLE";
        case CKR_KEY_UNEXTRACTABLE: return "CKR_KEY_UNEXTRACTABLE";
        case CKR_MECHANISM_INVALID: return "CKR_MECHANISM_INVALID";
        case CKR_MECHANISM_PARAM_INVALID: return "CKR_MECHANISM_PARAM_INVALID";
        case CKR_OBJECT_HANDLE_INVALID: return "CKR_OBJECT_HANDLE_INVALID";
        case CKR_OPERATION_ACTIVE: return "CKR_OPERATION_ACTIVE";
        case CKR_OPERATION_NOT_INITIALIZED: return "CKR_OPERATION_NOT_INITIALIZED";
        case CKR_PIN_INCORRECT: return "CKR_PIN_INCORRECT";
        case CKR_PIN_INVALID: return "CKR_PIN_INVALID";
        case CKR_PIN_LEN_RANGE: return "CKR_PIN_LEN_RANGE";
        case CKR_PIN_EXPIRED: return "CKR_PIN_EXPIRED";
        case CKR_PIN_LOCKED: return "CKR_PIN_LOCKED";
        case CKR_SESSION_CLOSED: return "CKR_SESSION_CLOSED";
        case CKR_SESSION_COUNT: return "CKR_SESSION_COUNT";
        case CKR_SESSION_HANDLE_INVALID: return "CKR_SESSION_HANDLE_INVALID";
        case CKR_SESSION_PARALLEL_NOT_SUPPORTED: return "CKR_SESSION_PARALLEL_NOT_SUPPORTED";
        case CKR_SESSION_READ_ONLY: return "CKR_SESSION_READ_ONLY";
        case CKR_SESSION_EXISTS: return "CKR_SESSION_EXISTS";
        case CKR_SESSION_READ_ONLY_EXISTS: return "CKR_SESSION_READ_ONLY_EXISTS";
        case CKR_SESSION_READ_WRITE_SO_EXISTS: return "CKR_SESSION_READ_WRITE_SO_EXISTS";
        case CKR_SIGNATURE_INVALID: return "CKR_SIGNATURE_INVALID";
        case CKR_SIGNATURE_LEN_RANGE: return "CKR_SIGNATURE_LEN_RANGE";
        case CKR_TEMPLATE_INCOMPLETE: return "CKR_TEMPLATE_INCOMPLETE";
        case CKR_TEMPLATE_INCONSISTENT: return "CKR_TEMPLATE_INCONSISTENT";
        case CKR_TOKEN_NOT_PRESENT: return "CKR_TOKEN_NOT_PRESENT";
        case CKR_TOKEN_NOT_RECOGNIZED: return "CKR_TOKEN_NOT_RECOGNIZED";
        case CKR_TOKEN_WRITE_PROTECTED: return "CKR_TOKEN_WRITE_PROTECTED";
        case CKR_UNWRAPPING_KEY_HANDLE_INVALID: return "CKR_UNWRAPPING_KEY_HANDLE_INVALID";
        case CKR_UNWRAPPING_KEY_SIZE_RANGE: return "CKR_UNWRAPPING_KEY_SIZE_RANGE";
        case CKR_UNWRAPPING_KEY_TYPE_INCONSISTENT: return "CKR_UNWRAPPING_KEY_TYPE_INCONSISTENT";
        case CKR_USER_ALREADY_LOGGED_IN: return "CKR_USER_ALREADY_LOGGED_IN";
        case CKR_USER_NOT_LOGGED_IN: return "CKR_USER_NOT_LOGGED_IN";
        case CKR_USER_PIN_NOT_INITIALIZED: return "CKR_USER_PIN_NOT_INITIALIZED";
        case CKR_USER_TYPE_INVALID: return "CKR_USER_TYPE_INVALID";
        case CKR_USER_ANOTHER_ALREADY_LOGGED_IN: return "CKR_USER_ANOTHER_ALREADY_LOGGED_IN";
        case CKR_USER_TOO_MANY_TYPES: return "CKR_USER_TOO_MANY_TYPES";
        case CKR_BUFFER_TOO_SMALL: return "CKR_BUFFER_TOO_SMALL";
        case CKR_SAVED_STATE_INVALID: return "CKR_SAVED_STATE_INVALID";
        case CKR_INFORMATION_SENSITIVE: return "CKR_INFORMATION_SENSITIVE";
        case CKR_STATE_UNSAVEABLE: return "CKR_STATE_UNSAVEABLE";
        case CKR_CRYPTOKI_NOT_INITIALIZED: return "CKR_CRYPTOKI_NOT_INITIALIZED";
        case CKR_CRYPTOKI_ALREADY_INITIALIZED: return "CKR_CRYPTOKI_ALREADY_INITIALIZED";
        default: return "Unknown Error";
    }
}

/* 加载PKCS#11库 */
CK_RV hsm_load_library(const char *library_path)
{
    pkcs11_lib = dlopen(library_path, RTLD_NOW);
    if (!pkcs11_lib) {
        fprintf(stderr, "无法加载PKCS#11库: %s\n", dlerror());
        return CKR_GENERAL_ERROR;
    }

    CK_C_GetFunctionList_t get_function_list =
        (CK_C_GetFunctionList_t)dlsym(pkcs11_lib, "C_GetFunctionList");
    if (!get_function_list) {
        fprintf(stderr, "无法获取C_GetFunctionList: %s\n", dlerror());
        dlclose(pkcs11_lib);
        pkcs11_lib = NULL;
        return CKR_GENERAL_ERROR;
    }

    CK_RV rv = get_function_list(&p11);
    if (rv != CKR_OK) {
        fprintf(stderr, "C_GetFunctionList失败: 0x%08lx\n", (unsigned long)rv);
        dlclose(pkcs11_lib);
        pkcs11_lib = NULL;
        p11 = NULL;
    }

    return rv;
}

/* 卸载PKCS#11库 */
CK_RV hsm_unload_library(void)
{
    if (pkcs11_lib) {
        dlclose(pkcs11_lib);
        pkcs11_lib = NULL;
        p11 = NULL;
    }
    return CKR_OK;
}

/* 初始化Cryptoki */
CK_RV hsm_initialize(void)
{
    if (!p11) return CKR_CRYPTOKI_NOT_INITIALIZED;
    return p11->C_Initialize(NULL);
}

/* 结束Cryptoki */
CK_RV hsm_finalize(void)
{
    if (!p11) return CKR_CRYPTOKI_NOT_INITIALIZED;
    return p11->C_Finalize(NULL);
}

/* 获取插槽列表 */
CK_RV hsm_get_slot_list(CK_BBOOL token_present, CK_SLOT_ID *slots, CK_ULONG *count)
{
    if (!p11) return CKR_CRYPTOKI_NOT_INITIALIZED;
    return p11->C_GetSlotList(token_present, slots, count);
}

/* 获取插槽信息 */
CK_RV hsm_get_slot_info(CK_SLOT_ID slot_id, CK_SLOT_INFO *info)
{
    if (!p11) return CKR_CRYPTOKI_NOT_INITIALIZED;
    return p11->C_GetSlotInfo(slot_id, info);
}

/* 获取令牌信息 */
CK_RV hsm_get_token_info(CK_SLOT_ID slot_id, CK_TOKEN_INFO *info)
{
    if (!p11) return CKR_CRYPTOKI_NOT_INITIALIZED;
    return p11->C_GetTokenInfo(slot_id, info);
}

/* 打开会话 */
CK_RV hsm_open_session(CK_SLOT_ID slot_id, CK_FLAGS flags, CK_SESSION_HANDLE *session)
{
    if (!p11) return CKR_CRYPTOKI_NOT_INITIALIZED;
    return p11->C_OpenSession(slot_id, flags, NULL, NULL, session);
}

/* 关闭会话 */
CK_RV hsm_close_session(CK_SESSION_HANDLE session)
{
    if (!p11) return CKR_CRYPTOKI_NOT_INITIALIZED;
    return p11->C_CloseSession(session);
}

/* 登录 */
CK_RV hsm_login(CK_SESSION_HANDLE session, CK_USER_TYPE user_type, const char *pin)
{
    if (!p11) return CKR_CRYPTOKI_NOT_INITIALIZED;
    return p11->C_Login(session, user_type, (CK_UTF8CHAR_PTR)pin,
                        pin ? (CK_ULONG)strlen(pin) : 0);
}

/* 登出 */
CK_RV hsm_logout(CK_SESSION_HANDLE session)
{
    if (!p11) return CKR_CRYPTOKI_NOT_INITIALIZED;
    return p11->C_Logout(session);
}
