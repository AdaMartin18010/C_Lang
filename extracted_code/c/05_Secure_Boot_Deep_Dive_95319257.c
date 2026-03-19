/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\05_Secure_Boot_Deep_Dive.md
 * Line: 401
 * Language: c
 * Block ID: 95319257
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * BL2: Trusted Boot Firmware
 * 负责加载并验证所有后续启动阶段
 */

/* 启动镜像描述符 */
typedef struct {
    uint32_t image_id;
    const char *name;
    uint64_t load_addr;
    uint64_t entry_point;
    uint32_t flags;
    auth_info_t auth_info;
} bl_mem_params_node_t;

/* 镜像ID定义 */
enum {
    BL31_IMAGE_ID = 0,
    BL32_IMAGE_ID,      /* OP-TEE */
    BL33_IMAGE_ID,      /* U-Boot/UEFI */
    SCP_BL2_IMAGE_ID,   /* SCP固件 */
    NUM_IMAGES,
};

void bl2_main(void) {
    /* 1. 初始化控制台 */
    console_init();
    INFO("BL2: Trusted Boot Firmware\n");

    /* 2. 平台初始化 */
    bl2_plat_arch_setup();
    bl2_platform_setup();

    /* 3. 初始化TPM（如果存在） */
    tpm_init();

    /* 4. 加载并验证所有镜像 */
    bl_mem_params_node_t mem_params[NUM_IMAGES] = {0};

    /* 加载BL31（必须） */
    int ret = load_auth_image(BL31_IMAGE_ID, &mem_params[BL31_IMAGE_ID]);
    if (ret != 0) {
        ERROR("BL31 load/auth failed: %d\n", ret);
        panic();
    }
    extend_pcr(PCR_BL31, mem_params[BL31_IMAGE_ID].hash, 32);

    /* 加载BL32（可选，如OP-TEE） */
    if (is_optee_required()) {
        ret = load_auth_image(BL32_IMAGE_ID, &mem_params[BL32_IMAGE_ID]);
        if (ret != 0) {
            ERROR("BL32 load/auth failed\n");
            panic();
        }
        extend_pcr(PCR_BL32, mem_params[BL32_IMAGE_ID].hash, 32);
    }

    /* 加载BL33（非安全世界bootloader） */
    ret = load_auth_image(BL33_IMAGE_ID, &mem_params[BL33_IMAGE_ID]);
    if (ret != 0) {
        ERROR("BL33 load/auth failed\n");
        panic();
    }
    extend_pcr(PCR_BL33, mem_params[BL33_IMAGE_ID].hash, 32);

    /* 5. 填充启动信息 */
    bl_params_t *next_params = prepare_next_boot_params(mem_params);

    /* 6. 进入BL31 */
    INFO("Handing off to BL31...\n");
    bl2_plat_flush_mem();

    bl31_entry_t bl31_entry = (bl31_entry_t)mem_params[BL31_IMAGE_ID].entry_point;
    bl31_entry(next_params);
}

/* 加载并认证镜像 */
int load_auth_image(uint32_t image_id, bl_mem_params_node_t *params) {
    /* 1. 从FIP (Firmware Image Package) 读取镜像 */
    fip_toc_entry_t toc_entry;
    if (fip_get_image_info(image_id, &toc_entry) != 0) {
        return -ENOENT;
    }

    /* 2. 分配内存 */
    void *image_buf = allocate_load_mem(params->load_addr, toc_entry.size);
    if (!image_buf) {
        return -ENOMEM;
    }

    /* 3. 加载镜像 */
    fip_load_image(&toc_entry, image_buf);

    /* 4. 解析认证信息 */
    auth_info_t auth_info;
    if (parse_auth_info(image_buf, &auth_info) != 0) {
        return ERR_INVALID_MAGIC;
    }

    /* 5. 验证证书链 */
    if (verify_cert_chain(&auth_info.cert_chain) != 0) {
        log_security_event(EVENT_CERT_INVALID, image_id);
        return ERR_CERT_CHAIN_BROKEN;
    }

    /* 6. 验证签名 */
    uint8_t image_hash[64];
    hash_memory(auth_info.hash_alg,
                image_buf + auth_info.payload_offset,
                auth_info.payload_size,
                image_hash, NULL);

    if (verify_signature(&auth_info, image_hash) != 0) {
        return ERR_SIGNATURE_INVALID;
    }

    /* 7. 反滚动保护检查 */
    if (check_rollback(&auth_info) != 0) {
        return ERR_ROLLBACK_DETECTED;
    }

    /* 8. 记录安全版本号 */
    update_security_version(image_id, auth_info.security_version);

    params->entry_point = auth_info.entry_point;
    memcpy(params->hash, image_hash, 32);

    return 0;
}
