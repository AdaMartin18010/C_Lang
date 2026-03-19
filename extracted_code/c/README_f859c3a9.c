/*
 * Auto-generated from: 03_System_Technology_Domains\06_Security_Boot\README.md
 * Line: 101
 * Language: c
 * Block ID: f859c3a9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* UEFI Secure Boot 变量结构 */
typedef struct {
    EFI_GUID signature_owner;       /* 签名者GUID */
    UINT16 signature_size;          /* 签名数据大小 */
    UINT8 signature_data[];         /* X.509证书或SHA-256哈希 */
} EFI_SIGNATURE_DATA;

typedef struct {
    EFI_GUID signature_type;        /* EFI_CERT_TYPE_RSA2048/SHA256 */
    UINT32 signature_list_size;     /* 列表总大小 */
    UINT32 signature_header_size;   /* 头部大小 */
    UINT32 signature_size;          /* 单个签名大小 */
    // UINT8 signature_header[signature_header_size]; /* 可变头部 */
    // EFI_SIGNATURE_DATA signatures[];              /* 签名数组 */
} EFI_SIGNATURE_LIST;

/* 安全启动验签流程 */
EFI_STATUS verify_image_signature(
    VOID *image_buffer,
    UINTN image_size,
    EFI_SIGNATURE_LIST *db,         /* 允许列表 */
    EFI_SIGNATURE_LIST *dbx         /* 禁止列表 */
) {
    /* 1. 检查镜像哈希是否在DBX中 */
    UINT8 image_hash[SHA256_DIGEST_SIZE];
    sha256_hash(image_buffer, image_size, image_hash);

    if (find_hash_in_list(dbx, image_hash) == EFI_SUCCESS) {
        return EFI_SECURITY_VIOLATION;  /* 禁止的镜像 */
    }

    /* 2. 验证签名 */
    WIN_CERTIFICATE_EFI_PKCS *pkcs7_sig;
    extract_pkcs7_signature(image_buffer, &pkcs7_sig);

    EFI_SIGNATURE_DATA *cert = find_matching_cert(db, pkcs7_sig);
    if (cert == NULL) {
        return EFI_SECURITY_VIOLATION;  /* 未授权的签名者 */
    }

    /* 3. RSA-2048 + SHA-256 验证 */
    return rsa_verify_sha256(
        cert->signature_data,
        pkcs7_sig->signature,
        image_hash
    );
}
