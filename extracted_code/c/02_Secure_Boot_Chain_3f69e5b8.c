/*
 * Auto-generated from: 03_System_Technology_Domains\06_Security_Boot\02_Secure_Boot_Chain.md
 * Line: 186
 * Language: c
 * Block ID: 3f69e5b8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 启动阶段上下文
typedef struct {
    uint32_t stage;           // 当前阶段
    uint8_t  next_hash[32];   // 下一阶段公钥哈希
    void    *load_addr;       // 加载地址
} BootContext;

// 链式验证启动
int chain_load(BootContext *ctx, const char *image_name, uint32_t stage) {
    // 从存储加载镜像
    uint8_t *image = load_from_storage(image_name);
    size_t image_size = get_image_size(image_name);

    // 验证镜像
    if (verify_image(image, image_size, ctx->next_hash) != 0) {
        printf("Stage %u verification failed!\n", stage);
        enter_recovery_mode();
        return -1;
    }

    // 提取下一阶段公钥哈希
    BootImageHeader *hdr = (BootImageHeader*)image;
    memcpy(ctx->next_hash, hdr->next_stage_hash, 32);

    // 解压并执行
    void *exec_addr = decompress_image(image, ctx->load_addr);

    printf("Loading stage %u at %p\n", stage, exec_addr);

    // 跳转到下一阶段
    boot_jump(exec_addr, ctx);

    return 0;  // 不会到达这里
}

// 完整启动链
void secure_boot_chain(void) {
    BootContext ctx = {0};

    // 阶段1: BL1 (Boot ROM加载)
    ctx.stage = 1;
    memcpy(ctx.next_hash, read_otp_bl2_hash(), 32);
    chain_load(&ctx, "bl2.bin", 1);

    // 阶段2: BL2
    ctx.stage = 2;
    // BL2哈希已在chain_load中更新
    chain_load(&ctx, "bl31.bin", 2);

    // 阶段3: BL31/32/33
    ctx.stage = 3;
    chain_load(&ctx, "bl33.bin", 3);
}
