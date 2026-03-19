/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\03_HSM_Integration.md
 * Line: 141
 * Language: c
 * Block ID: 51970861
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* pkcs11_types.h - PKCS#11常用数据类型 */
#ifndef PKCS11_TYPES_H
#define PKCS11_TYPES_H

#include <stdint.h>
#include <stddef.h>

/* 基本类型定义 */
typedef unsigned char     CK_BYTE;
typedef CK_BYTE           CK_CHAR;
typedef CK_BYTE           CK_UTF8CHAR;
typedef CK_BYTE           CK_BBOOL;
typedef unsigned long     CK_ULONG;
typedef long              CK_LONG;
typedef CK_ULONG          CK_FLAGS;
typedef CK_BYTE*          CK_BYTE_PTR;
typedef CK_CHAR*          CK_CHAR_PTR;
typedef CK_UTF8CHAR*      CK_UTF8CHAR_PTR;
typedef CK_ULONG*         CK_ULONG_PTR;
typedef void*             CK_VOID_PTR;

/* Cryptoki类型 */
typedef CK_VOID_PTR       CK_SESSION_HANDLE;
typedef CK_ULONG          CK_OBJECT_HANDLE;
typedef CK_ULONG          CK_SLOT_ID;
typedef CK_ULONG          CK_MECHANISM_TYPE;
typedef CK_ULONG          CK_RV;
typedef CK_ULONG          CK_ATTRIBUTE_TYPE;
typedef CK_ULONG          CK_KEY_TYPE;
typedef CK_ULONG          CK_OBJECT_CLASS;

/* 属性结构 */
typedef struct CK_ATTRIBUTE {
    CK_ATTRIBUTE_TYPE type;
    CK_VOID_PTR       pValue;
    CK_ULONG          ulValueLen;
} CK_ATTRIBUTE;

/* 机制结构 */
typedef struct CK_MECHANISM {
    CK_MECHANISM_TYPE mechanism;
    CK_VOID_PTR       pParameter;
    CK_ULONG          ulParameterLen;
} CK_MECHANISM;

/* 常用返回值 */
#define CKR_OK                    0x00000000
#define CKR_CANCEL                0x00000001
#define CKR_HOST_MEMORY           0x00000002
#define CKR_SLOT_ID_INVALID       0x00000003
#define CKR_GENERAL_ERROR         0x00000005
#define CKR_FUNCTION_FAILED       0x00000006
#define CKR_ARGUMENTS_BAD         0x00000007
#define CKR_NO_EVENT              0x00000008
#define CKR_NEED_TO_CREATE_THREADS 0x00000009
#define CKR_CANT_LOCK             0x0000000A
#define CKR_ATTRIBUTE_READ_ONLY   0x00000010
#define CKR_ATTRIBUTE_SENSITIVE   0x00000011
#define CKR_ATTRIBUTE_TYPE_INVALID 0x00000012
#define CKR_ATTRIBUTE_VALUE_INVALID 0x00000013
#define CKR_DATA_INVALID          0x00000020
#define CKR_DATA_LEN_RANGE        0x00000021
#define CKR_DEVICE_ERROR          0x00000030
#define CKR_DEVICE_MEMORY         0x00000031
#define CKR_DEVICE_REMOVED        0x00000032
#define CKR_ENCRYPTED_DATA_INVALID 0x00000040
#define CKR_ENCRYPTED_DATA_LEN_RANGE 0x00000041
#define CKR_FUNCTION_CANCELED     0x00000050
#define CKR_FUNCTION_NOT_PARALLEL 0x00000051
#define CKR_FUNCTION_NOT_SUPPORTED 0x00000054
#define CKR_KEY_HANDLE_INVALID    0x00000060
#define CKR_KEY_SIZE_RANGE        0x00000062
#define CKR_KEY_TYPE_INCONSISTENT 0x00000063
#define CKR_KEY_NOT_NEEDED        0x00000064
#define CKR_KEY_CHANGED           0x00000065
#define CKR_KEY_NEEDED            0x00000066
#define CKR_KEY_INDIGESTIBLE      0x00000067
#define CKR_KEY_FUNCTION_NOT_PERMITTED 0x00000068
#define CKR_KEY_NOT_WRAPPABLE     0x00000069
#define CKR_KEY_UNEXTRACTABLE     0x0000006A
#define CKR_MECHANISM_INVALID     0x00000070
#define CKR_MECHANISM_PARAM_INVALID 0x00000071
#define CKR_OBJECT_HANDLE_INVALID 0x00000082
#define CKR_OPERATION_ACTIVE      0x00000090
#define CKR_OPERATION_NOT_INITIALIZED 0x00000091
#define CKR_PIN_INCORRECT         0x000000A0
#define CKR_PIN_INVALID           0x000000A1
#define CKR_PIN_LEN_RANGE         0x000000A2
#define CKR_PIN_EXPIRED           0x000000A3
#define CKR_PIN_LOCKED            0x000000A4
#define CKR_SESSION_CLOSED        0x000000B0
#define CKR_SESSION_COUNT         0x000000B1
#define CKR_SESSION_HANDLE_INVALID 0x000000B3
#define CKR_SESSION_PARALLEL_NOT_SUPPORTED 0x000000B4
#define CKR_SESSION_READ_ONLY     0x000000B5
#define CKR_SESSION_EXISTS        0x000000B6
#define CKR_SESSION_READ_ONLY_EXISTS 0x000000B7
#define CKR_SESSION_READ_WRITE_SO_EXISTS 0x000000B8
#define CKR_SIGNATURE_INVALID     0x000000C0
#define CKR_SIGNATURE_LEN_RANGE   0x000000C1
#define CKR_TEMPLATE_INCOMPLETE   0x000000D0
#define CKR_TEMPLATE_INCONSISTENT 0x000000D1
#define CKR_TOKEN_NOT_PRESENT     0x000000E0
#define CKR_TOKEN_NOT_RECOGNIZED  0x000000E1
#define CKR_TOKEN_WRITE_PROTECTED 0x000000E2
#define CKR_UNWRAPPING_KEY_HANDLE_INVALID 0x000000F0
#define CKR_UNWRAPPING_KEY_SIZE_RANGE 0x000000F1
#define CKR_UNWRAPPING_KEY_TYPE_INCONSISTENT 0x000000F2
#define CKR_USER_ALREADY_LOGGED_IN 0x00000100
#define CKR_USER_NOT_LOGGED_IN    0x00000101
#define CKR_USER_PIN_NOT_INITIALIZED 0x00000102
#define CKR_USER_TYPE_INVALID     0x00000103
#define CKR_USER_ANOTHER_ALREADY_LOGGED_IN 0x00000104
#define CKR_USER_TOO_MANY_TYPES   0x00000105
#define CKR_BUFFER_TOO_SMALL      0x00000150
#define CKR_SAVED_STATE_INVALID   0x00000160
#define CKR_INFORMATION_SENSITIVE 0x00000170
#define CKR_STATE_UNSAVEABLE      0x00000180
#define CKR_CRYPTOKI_NOT_INITIALIZED 0x00000190
#define CKR_CRYPTOKI_ALREADY_INITIALIZED 0x00000191
#define CKR_VENDOR_DEFINED        0x80000000

/* 对象类 */
#define CKO_DATA                  0x00000000
#define CKO_CERTIFICATE           0x00000001
#define CKO_PUBLIC_KEY            0x00000002
#define CKO_PRIVATE_KEY           0x00000003
#define CKO_SECRET_KEY            0x00000004
#define CKO_HW_FEATURE            0x00000005
#define CKO_DOMAIN_PARAMETERS     0x00000006
#define CKO_MECHANISM             0x00000007

/* 密钥类型 */
#define CKK_RSA                   0x00000000
#define CKK_DSA                   0x00000001
#define CKK_DH                    0x00000002
#define CKK_EC                    0x00000003
#define CKK_X9_42_DH              0x00000004
#define CKK_KEA                   0x00000005
#define CKK_GENERIC_SECRET        0x00000010
#define CKK_RC2                   0x00000011
#define CKK_RC4                   0x00000012
#define CKK_DES                   0x00000013
#define CKK_DES2                  0x00000014
#define CKK_DES3                  0x00000015
#define CKK_CAST                  0x00000016
#define CKK_CAST3                 0x00000017
#define CKK_CAST128               0x00000018
#define CKK_RC5                   0x00000019
#define CKK_IDEA                  0x0000001A
#define CKK_SKIPJACK              0x0000001B
#define CKK_BATON                 0x0000001C
#define CKK_JUNIPER               0x0000001D
#define CKK_CDMF                  0x0000001E
#define CKK_AES                   0x0000001F
#define CKK_BLOWFISH              0x00000020
#define CKK_TWOFISH               0x00000021

/* 机制类型 */
#define CKM_RSA_PKCS_KEY_PAIR_GEN 0x00000000
#define CKM_RSA_PKCS              0x00000001
#define CKM_RSA_9796              0x00000002
#define CKM_RSA_X_509             0x00000003
#define CKM_MD2_RSA_PKCS          0x00000004
#define CKM_MD5_RSA_PKCS          0x00000005
#define CKM_SHA1_RSA_PKCS         0x00000006
#define CKM_SHA256_RSA_PKCS       0x00000040
#define CKM_SHA384_RSA_PKCS       0x00000041
#define CKM_SHA512_RSA_PKCS       0x00000042
#define CKM_RSA_PKCS_OAEP         0x00000009
#define CKM_RSA_X9_31_KEY_PAIR_GEN 0x0000000A
#define CKM_RSA_X9_31             0x0000000B
#define CKM_SHA1_RSA_X9_31        0x0000000C
#define CKM_RSA_PKCS_PSS          0x0000000D
#define CKM_SHA1_RSA_PKCS_PSS     0x0000000E
#define CKM_SHA256_RSA_PKCS_PSS   0x00000043
#define CKM_SHA384_RSA_PKCS_PSS   0x00000044
#define CKM_SHA512_RSA_PKCS_PSS   0x00000045
#define CKM_DSA_KEY_PAIR_GEN      0x00000010
#define CKM_DSA                   0x00000011
#define CKM_DSA_SHA1              0x00000012
#define CKM_DH_PKCS_KEY_PAIR_GEN  0x00000020
#define CKM_DH_PKCS_DERIVE        0x00000021
#define CKM_X9_42_DH_KEY_PAIR_GEN 0x00000030
#define CKM_X9_42_DH_DERIVE       0x00000031
#define CKM_X9_42_DH_HYBRID_DERIVE 0x00000032
#define CKM_X9_42_MQV_DERIVE      0x00000033
#define CKM_RC2_KEY_GEN           0x00000100
#define CKM_RC2_ECB               0x00000101
#define CKM_RC2_CBC               0x00000102
#define CKM_RC2_MAC               0x00000103
#define CKM_RC2_MAC_GENERAL       0x00000104
#define CKM_RC2_CBC_PAD           0x00000105
#define CKM_RC4_KEY_GEN           0x00000110
#define CKM_RC4                   0x00000111
#define CKM_DES_KEY_GEN           0x00000120
#define CKM_DES_ECB               0x00000121
#define CKM_DES_CBC               0x00000122
#define CKM_DES_MAC               0x00000123
#define CKM_DES_MAC_GENERAL       0x00000124
#define CKM_DES_CBC_PAD           0x00000125
#define CKM_DES2_KEY_GEN          0x00000130
#define CKM_DES3_KEY_GEN          0x00000131
#define CKM_DES3_ECB              0x00000132
#define CKM_DES3_CBC              0x00000133
#define CKM_DES3_MAC              0x00000134
#define CKM_DES3_MAC_GENERAL      0x00000135
#define CKM_DES3_CBC_PAD          0x00000136
#define CKM_CDMF_KEY_GEN          0x00000140
#define CKM_CDMF_ECB              0x00000141
#define CKM_CDMF_CBC              0x00000142
#define CKM_CDMF_MAC              0x00000143
#define CKM_CDMF_MAC_GENERAL      0x00000144
#define CKM_CDMF_CBC_PAD          0x00000145
#define CKM_MD2                   0x00000200
#define CKM_MD2_HMAC              0x00000201
#define CKM_MD2_HMAC_GENERAL      0x00000202
#define CKM_MD5                   0x00000210
#define CKM_MD5_HMAC              0x00000211
#define CKM_MD5_HMAC_GENERAL      0x00000212
#define CKM_SHA_1                 0x00000220
#define CKM_SHA_1_HMAC            0x00000221
#define CKM_SHA_1_HMAC_GENERAL    0x00000222
#define CKM_SHA_256               0x00000250
#define CKM_SHA_256_HMAC          0x00000251
#define CKM_SHA_256_HMAC_GENERAL  0x00000252
#define CKM_SHA_384               0x00000260
#define CKM_SHA_384_HMAC          0x00000261
#define CKM_SHA_384_HMAC_GENERAL  0x00000262
#define CKM_SHA_512               0x00000270
#define CKM_SHA_512_HMAC          0x00000271
#define CKM_SHA_512_HMAC_GENERAL  0x00000272
#define CKM_GENERIC_SECRET_KEY_GEN 0x00000350
#define CKM_SHA256_KEY_DERIVATION 0x00000396
#define CKM_SHA384_KEY_DERIVATION 0x00000397
#define CKM_SHA512_KEY_DERIVATION 0x00000398
#define CKM_AES_KEY_GEN           0x00001080
#define CKM_AES_ECB               0x00001081
#define CKM_AES_CBC               0x00001082
#define CKM_AES_MAC               0x00001083
#define CKM_AES_MAC_GENERAL       0x00001084
#define CKM_AES_CBC_PAD           0x00001085
#define CKM_AES_CTR               0x00001086
#define CKM_AES_GCM               0x00001087
#define CKM_AES_CCM               0x00001088
#define CKM_AES_CTS               0x00001089
#define CKM_AES_CMAC              0x0000108A
#define CKM_AES_CMAC_GENERAL      0x0000108B

/* 属性类型 */
#define CKA_CLASS                 0x00000000
#define CKA_TOKEN                 0x00000001
#define CKA_PRIVATE               0x00000002
#define CKA_LABEL                 0x00000003
#define CKA_APPLICATION           0x00000010
#define CKA_VALUE                 0x00000011
#define CKA_OBJECT_ID             0x00000012
#define CKA_CERTIFICATE_TYPE      0x00000080
#define CKA_ISSUER                0x00000081
#define CKA_SERIAL_NUMBER         0x00000082
#define CKA_AC_ISSUER             0x00000083
#define CKA_OWNER                 0x00000084
#define CKA_ATTR_TYPES            0x00000085
#define CKA_TRUSTED               0x00000086
#define CKA_CERTIFICATE_CATEGORY  0x00000087
#define CKA_JAVA_MIDP_SECURITY_DOMAIN 0x00000088
#define CKA_URL                   0x00000089
#define CKA_HASH_OF_SUBJECT_PUBLIC_KEY 0x0000008A
#define CKA_HASH_OF_ISSUER_PUBLIC_KEY 0x0000008B
#define CKA_CHECK_VALUE           0x0000008C
#define CKA_KEY_TYPE              0x00000100
#define CKA_SUBJECT               0x00000101
#define CKA_ID                    0x00000102
#define CKA_SENSITIVE             0x00000103
#define CKA_ENCRYPT               0x00000104
#define CKA_DECRYPT               0x00000105
#define CKA_WRAP                  0x00000106
#define CKA_UNWRAP                0x00000107
#define CKA_SIGN                  0x00000108
#define CKA_SIGN_RECOVER          0x00000109
#define CKA_VERIFY                0x0000010A
#define CKA_VERIFY_RECOVER        0x0000010B
#define CKA_DERIVE                0x0000010C
#define CKA_START_DATE            0x00000110
#define CKA_END_DATE              0x00000111
#define CKA_MODULUS               0x00000120
#define CKA_MODULUS_BITS          0x00000121
#define CKA_PUBLIC_EXPONENT       0x00000122
#define CKA_PRIVATE_EXPONENT      0x00000123
#define CKA_PRIME_1               0x00000124
#define CKA_PRIME_2               0x00000125
#define CKA_EXPONENT_1            0x00000126
#define CKA_EXPONENT_2            0x00000127
#define CKA_COEFFICIENT           0x00000128
#define CKA_PUBLIC_KEY_INFO       0x00000129
#define CKA_PRIME                 0x00000130
#define CKA_SUBPRIME              0x00000131
#define CKA_BASE                  0x00000132
#define CKA_PRIME_BITS            0x00000133
#define CKA_SUBPRIME_BITS         0x00000134
#define CKA_VALUE_BITS            0x00000160
#define CKA_VALUE_LEN             0x00000161
#define CKA_EXTRACTABLE           0x00000162
#define CKA_LOCAL                 0x00000163
#define CKA_NEVER_EXTRACTABLE     0x00000164
#define CKA_ALWAYS_SENSITIVE      0x00000165
#define CKA_KEY_GEN_MECHANISM     0x00000166
#define CKA_MODIFIABLE            0x00000170
#define CKA_COPYABLE              0x00000171
#define CKA_DESTROYABLE           0x00000172
#define CKA_ECDSA_PARAMS          0x00000180
#define CKA_EC_PARAMS             0x00000180
#define CKA_EC_POINT              0x00000181
#define CKA_SECONDARY_AUTH        0x00000200
#define CKA_AUTH_PIN_FLAGS        0x00000201
#define CKA_ALWAYS_AUTHENTICATE   0x00000202
#define CKA_WRAP_WITH_TRUSTED     0x00000210
#define CKA_WRAP_TEMPLATE         (CKF_ARRAY_ATTRIBUTE|0x00000211)
#define CKA_UNWRAP_TEMPLATE       (CKF_ARRAY_ATTRIBUTE|0x00000212)
#define CKA_DERIVE_TEMPLATE       (CKF_ARRAY_ATTRIBUTE|0x00000213)
#define CKA_OTP_FORMAT            0x00000220
#define CKA_OTP_LENGTH            0x00000221
#define CKA_OTP_TIME_INTERVAL     0x00000222
#define CKA_OTP_USER_FRIENDLY_MODE 0x00000223
#define CKA_OTP_CHALLENGE_REQUIREMENT 0x00000224
#define CKA_OTP_TIME_REQUIREMENT  0x00000225
#define CKA_OTP_COUNTER_REQUIREMENT 0x00000226
#define CKA_OTP_PIN_REQUIREMENT   0x00000227
#define CKA_OTP_COUNTER           0x0000022E
#define CKA_OTP_TIME              0x0000022F
#define CKA_OTP_USER_IDENTIFIER   0x0000022A
#define CKA_OTP_SERVICE_IDENTIFIER 0x0000022B
#define CKA_OTP_SERVICE_LOGO      0x0000022C
#define CKA_OTP_SERVICE_LOGO_TYPE 0x0000022D
#define CKA_GOSTR3410_PARAMS      0x00000250
#define CKA_GOSTR3411_PARAMS      0x00000251
#define CKA_GOST28147_PARAMS      0x00000252
#define CKA_HW_FEATURE_TYPE       0x00000300
#define CKA_RESET_ON_INIT         0x00000301
#define CKA_HAS_RESET             0x00000302
#define CKA_PIXEL_X               0x00000400
#define CKA_PIXEL_Y               0x00000401
#define CKA_RESOLUTION            0x00000402
#define CKA_CHAR_ROWS             0x00000403
#define CKA_CHAR_COLUMNS          0x00000404
#define CKA_COLOR                 0x00000405
#define CKA_BITS_PER_PIXEL        0x00000406
#define CKA_CHAR_SETS             0x00000480
#define CKA_ENCODING_METHODS      0x00000481
#define CKA_MIME_TYPES            0x00000482
#define CKA_MECHANISM_TYPE        0x00000500
#define CKA_REQUIRED_CMS_ATTRIBUTES 0x00000501
#define CKA_DEFAULT_CMS_ATTRIBUTES 0x00000502
#define CKA_SUPPORTED_CMS_ATTRIBUTES 0x00000503
#define CKA_ALLOWED_MECHANISMS    (CKF_ARRAY_ATTRIBUTE|0x00000600)
#define CKA_VENDOR_DEFINED        0x80000000

/* 会话类型 */
#define CKS_RO_PUBLIC_SESSION     0
#define CKS_RO_USER_FUNCTIONS     1
#define CKS_RW_PUBLIC_SESSION     2
#define CKS_RW_USER_FUNCTIONS     3
#define CKS_RW_SO_FUNCTIONS       4

/* 用户类型 */
#define CKU_SO                    0
#define CKU_USER                  1
#define CKU_CONTEXT_SPECIFIC      2

/* 状态标志 */
#define CKF_TOKEN_PRESENT         0x00000001
#define CKF_REMOVABLE_DEVICE      0x00000002
#define CKF_HW_SLOT               0x00000004
#define CKF_RNG                   0x00000001
#define CKF_WRITE_PROTECTED       0x00000002
#define CKF_LOGIN_REQUIRED        0x00000004
#define CKF_USER_PIN_INITIALIZED  0x00000008
#define CKF_RESTORE_KEY_NOT_NEEDED 0x00000020
#define CKF_CLOCK_ON_TOKEN        0x00000040
#define CKF_SECONDARY_AUTHENTICATION 0x00000080
#define CKF_USER_PIN_COUNT_LOW    0x00010000
#define CKF_USER_PIN_FINAL_TRY    0x00020000
#define CKF_USER_PIN_LOCKED       0x00040000
#define CKF_USER_PIN_TO_BE_CHANGED 0x00080000
#define CKF_SO_PIN_COUNT_LOW      0x00100000
#define CKF_SO_PIN_FINAL_TRY      0x00200000
#define CKF_SO_PIN_LOCKED         0x00400000
#define CKF_SO_PIN_TO_BE_CHANGED  0x00800000
#define CKF_RW_SESSION            0x00000002
#define CKF_SERIAL_SESSION        0x00000004

#endif /* PKCS11_TYPES_H */
