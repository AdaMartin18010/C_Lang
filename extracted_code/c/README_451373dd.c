/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\README.md
 * Line: 538
 * Language: c
 * Block ID: 451373dd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* OP-TEE 客户端API */
#include <tee_client_api.h>

/* 安全会话建立 */
TEEC_Result open_ta_session(TEEC_Context *ctx,
                            TEEC_Session *sess,
                            const TEEC_UUID *ta_uuid) {
    TEEC_Operation op = {0};
    uint32_t origin;

    /* 打开与TA的会话 */
    TEEC_Result res = TEEC_OpenSession(ctx, sess, ta_uuid,
                                        TEEC_LOGIN_PUBLIC,
                                        NULL, &op, &origin);
    return res;
}

/* 调用安全服务 */
TEEC_Result invoke_secure_storage(TEEC_Session *sess,
                                  uint32_t command_id,
                                  void *data,
                                  size_t data_len) {
    TEEC_Operation op = {0};
    uint32_t origin;

    /* 设置参数 - 共享内存 */
    TEEC_SharedMemory shm = {
        .buffer = data,
        .size = data_len,
        .flags = TEEC_MEM_INPUT | TEEC_MEM_OUTPUT
    };
    TEEC_RegisterSharedMemory(sess->ctx, &shm);

    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_WHOLE,
                                      TEEC_NONE,
                                      TEEC_NONE,
                                      TEEC_NONE);
    op.params[0].memref.parent = &shm;
    op.params[0].memref.offset = 0;
    op.params[0].memref.size = data_len;

    /* 调用TA */
    TEEC_Result res = TEEC_InvokeCommand(sess, command_id, &op, &origin);

    TEEC_ReleaseSharedMemory(&shm);
    return res;
}

/* TA端安全存储实现 */
TEE_Result store_secure_object(uint32_t param_types,
                               TEE_Param params[4]) {
    const uint32_t exp_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT,
                                                TEE_PARAM_TYPE_NONE,
                                                TEE_PARAM_TYPE_NONE,
                                                TEE_PARAM_TYPE_NONE);
    if (param_types != exp_types)
        return TEE_ERROR_BAD_PARAMETERS;

    /* 创建或打开安全对象 */
    TEE_ObjectHandle object;
    TEE_Result res = TEE_CreatePersistentObject(
        TEE_STORAGE_PRIVATE,        /* 安全存储区域 */
        object_id, object_id_len,
        TEE_DATA_FLAG_ACCESS_WRITE, /* 访问权限 */
        TEE_HANDLE_NULL,            /* 不使用密钥 */
        NULL, 0,                    /* 初始数据 */
        &object);

    if (res != TEE_SUCCESS)
        return res;

    /* 写入数据 - 自动加密存储到RPMB */
    res = TEE_WriteObjectData(object,
                              params[0].memref.buffer,
                              params[0].memref.size);

    TEE_CloseObject(object);
    return res;
}
