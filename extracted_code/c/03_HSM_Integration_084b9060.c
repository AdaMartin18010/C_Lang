/*
 * Auto-generated from: 03_System_Technology_Domains\03_HSM_Integration.md
 * Line: 159
 * Language: c
 * Block ID: 084b9060
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* PKCS#11 基础操作示例 */
#include <pkcs11.h>

CK_FUNCTION_LIST_PTR pFunctionList;
CK_SLOT_ID slotId;
CK_SESSION_HANDLE hSession;

/* 1. 初始化Cryptoki库 */
CK_C_GetFunctionList(&pFunctionList);
pFunctionList->C_Initialize(NULL);

/* 2. 获取可用Slot */
CK_SLOT_ID_PTR pSlotList;
CK_ULONG ulSlotCount;
pFunctionList->C_GetSlotList(CK_TRUE, NULL, &ulSlotCount);
pSlotList = (CK_SLOT_ID_PTR)malloc(ulSlotCount * sizeof(CK_SLOT_ID));
pFunctionList->C_GetSlotList(CK_TRUE, pSlotList, &ulSlotCount);
slotId = pSlotList[0];

/* 3. 打开会话 */
pFunctionList->C_OpenSession(slotId,
    CKF_SERIAL_SESSION | CKF_RW_SESSION,
    NULL, NULL, &hSession);

/* 4. 用户登录 */
CK_UTF8CHAR pin[] = {"1234"};
pFunctionList->C_Login(hSession, CKU_USER, pin, sizeof(pin)-1);

/* 5. 生成密钥对 */
CK_MECHANISM mechanism = {CKM_RSA_PKCS_KEY_PAIR_GEN, NULL_PTR, 0};
CK_ATTRIBUTE publicKeyTemplate[] = {
    {CKA_MODULUS_BITS, &modulusBits, sizeof(modulusBits)},
    {CKA_PUBLIC_EXPONENT, publicExponent, sizeof(publicExponent)},
    {CKA_TOKEN, &trueValue, sizeof(trueValue)}
};
CK_ATTRIBUTE privateKeyTemplate[] = {
    {CKA_TOKEN, &trueValue, sizeof(trueValue)},
    {CKA_SENSITIVE, &trueValue, sizeof(trueValue)},
    {CKA_EXTRACTABLE, &falseValue, sizeof(falseValue)}
};
CK_OBJECT_HANDLE hPublicKey, hPrivateKey;
pFunctionList->C_GenerateKeyPair(hSession, &mechanism,
    publicKeyTemplate, 3,
    privateKeyTemplate, 3,
    &hPublicKey, &hPrivateKey);

/* 6. 签名操作 */
CK_MECHANISM signMech = {CKM_SHA256_RSA_PKCS, NULL, 0};
pFunctionList->C_SignInit(hSession, &signMech, hPrivateKey);
pFunctionList->C_Sign(hSession, data, dataLen, signature, &sigLen);

/* 7. 清理 */
pFunctionList->C_Logout(hSession);
pFunctionList->C_CloseSession(hSession);
pFunctionList->C_Finalize(NULL);
