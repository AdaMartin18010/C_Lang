/*
 * Auto-generated from: 03_System_Technology_Domains\06_Security_Boot\README.md
 * Line: 192
 * Language: c
 * Block ID: 74ebf266
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ARM TF-A 镜像认证描述符 */
typedef struct auth_img_desc {
    const char *img_id;                    /* 镜像标识 */
    auth_method_type_t auth_method;        /* 认证方法 */

    union {
        /* 哈希认证 */
        struct {
            const uint8_t *hash;
            size_t hash_len;
        } hash_auth;

        /* 签名认证 */
        struct {
            const uint8_t *pk_ptr;         /* 公钥指针 */
            const uint8_t *sig_ptr;        /* 签名指针 */
            auth_alg_t alg;                /* RSA/ECDSA */
        } sig_auth;
    } auth_data;

    /* 父镜像引用（形成信任链） */
    const struct auth_img_desc *parent;
} auth_img_desc_t;

/* 认证镜像头部（ARM固件镜像格式） */
typedef struct auth_img_header {
    uint32_t magic;                        /* 0x5F424E41 "ANB_" */
    uint16_t version;                      /* 头部版本 */
    uint16_t flags;                        /* 镜像标志 */
    uint32_t size;                         /* 镜像大小 */

    /* 签名区域 */
    uint32_t sig_offset;                   /* 签名偏移 */
    uint32_t sig_size;                     /* 签名大小 */

    /* 哈希区域 */
    uint32_t hash_offset;                  /* 哈希偏移 */
    uint32_t hash_size;                    /* 哈希大小 */

    /* 回滚保护 */
    uint32_t fw_version;                   /* 固件版本 */
    uint32_t min_fw_version;               /* 最低兼容版本 */
} __packed auth_img_header_t;

/* 安全启动验证函数 */
int auth_verify_image(const auth_img_desc_t *desc,
                      const void *image_ptr,
                      size_t image_size) {
    const auth_img_header_t *hdr = image_ptr;

    /* 1. 验证头部魔数 */
    if (hdr->magic != AUTH_IMG_MAGIC) {
        ERROR("Invalid image magic: 0x%x\n", hdr->magic);
        return -EINVAL;
    }

    /* 2. 回滚保护检查 */
    if (hdr->fw_version < desc->min_fw_version) {
        ERROR("Rollback detected: current=%u, minimum=%u\n",
              hdr->fw_version, desc->min_fw_version);
        return -EPERM;
    }

    /* 3. 根据认证方法验证 */
    switch (desc->auth_method) {
    case AUTH_METHOD_HASH:
        /* 计算并比较哈希 */
        uint8_t computed_hash[SHA256_DIGEST_SIZE];
        sha256_calc(image_ptr + hdr->hash_offset,
                    hdr->hash_size, computed_hash);

        if (memcmp(computed_hash, desc->auth_data.hash_auth.hash,
                   SHA256_DIGEST_SIZE) != 0) {
            ERROR("Hash verification failed\n");
            return -EAUTH;
        }
        break;

    case AUTH_METHOD_SIG:
        /* RSA签名验证 */
        if (rsa_verify_pkcs1_v15(
                desc->auth_data.sig_auth.pk_ptr,
                image_ptr + hdr->sig_offset,
                hdr->sig_size,
                image_ptr + sizeof(*hdr),
                image_size - sizeof(*hdr)) != 0) {
            ERROR("Signature verification failed\n");
            return -EAUTH;
        }
        break;

    default:
        return -ENOTSUP;
    }

    /* 4. 更新回滚计数器（原子操作） */
    update_rollback_counter(desc->img_id, hdr->fw_version);

    return 0;
}
