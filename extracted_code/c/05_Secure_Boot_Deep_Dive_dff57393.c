/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\05_Secure_Boot_Deep_Dive.md
 * Line: 644
 * Language: c
 * Block ID: dff57393
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * BL32: OP-TEE (Open Portable Trusted Execution Environment)
 * 可信执行环境实现
 */

/* OP-TEE启动信息 */
typedef struct {
    uint64_t pageable_part;
    uint64_t dtb;
    uint64_t dtb_size;
    uint64_t __reserved;
    uint64_t num_threads;
    uint64_t rpc_args;
    uint64_t rpc_cookies;
} optee_boot_info_t;

/* 初始化OP-TEE */
int optee_init(optee_boot_info_t *info) {
    /* 1. 验证OP-TEE镜像 */
    if (validate_optee_image() != 0) {
        return -EINVAL;
    }

    /* 2. 配置TrustZone内存 */
    configure_tzasc();

    /* 3. 初始化OP-TEE核心 */
    uint64_t entry_point = get_optee_entry();

    /* 设置S-EL1上下文 */
    cpu_context_t ctx = {0};

    /* 设置栈 */
    ctx.sp = OPTEE_STACK_TOP;

    /* 设置参数寄存器 */
    ctx.x0 = OPTEE_MSG_BOOT_OPTEE_MAGIC;
    ctx.x1 = (uintptr_t)info;
    ctx.x2 = OPTEE_MSG_BOOT_API_VERSION;

    /* 设置入口点 */
    ctx.pc = entry_point;

    /* 设置S-EL1安全配置 */
    ctx.sctlr = SCTLR_M_BIT | SCTLR_C_BIT | SCTLR_I_BIT;
    ctx.cpacr = CPACR_FP_EN;

    /* 4. 跳转到OP-TEE */
    INFO("Entering OP-TEE...\n");
    enter_el1_secure(&ctx);

    /* OP-TEE返回后 */
    INFO("OP-TEE initialization complete\n");

    return 0;
}

/* OP-TEE SMC处理 */
uintptr_t optee_smc_handler(uint32_t smc_fid, uintptr_t x1, uintptr_t x2,
                            uintptr_t x3, uintptr_t x4, void *handle) {
    switch (smc_fid) {
        case OPTEE_SMC_CALLS_COUNT:
            return OPTEE_SMC_CALL_COUNT;

        case OPTEE_SMC_CALLS_UID:
            return OPTEE_MSG_UID;

        case OPTEE_SMC_CALLS_REVISION:
            return (OPTEE_MSG_REVISION_MAJOR << 16) |
                   OPTEE_MSG_REVISION_MINOR;

        case OPTEE_SMC_CALL_GET_OS_UUID:
            return optee_get_os_uuid();

        case OPTEE_SMC_OPEN_SESSION:
            return optee_open_session(x1, x2, x3, x4);

        case OPTEE_SMC_INVOKE_COMMAND:
            return optee_invoke_command(x1, x2, x3, x4);

        case OPTEE_SMC_CLOSE_SESSION:
            return optee_close_session(x1);

        default:
            return OPTEE_SMC_RETURN_EBADCMD;
    }
}
