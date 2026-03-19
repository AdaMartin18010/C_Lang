/*
 * Auto-generated from: 03_System_Technology_Domains\07_Hardware_Security\05_Secure_Boot_Deep_Dive.md
 * Line: 2510
 * Language: c
 * Block ID: 9d145641
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * RISC-V TEE（Trusted Execution Environment）集成
 * Keystone/PMP-based TEE实现
 */

/* Keystone Enclave */
typedef struct {
    uintptr_t eid;              /* Enclave ID */
    uintptr_t runtime_paddr;    /* 运行时物理地址 */
    size_t runtime_size;
    uintptr_t user_paddr;       /* 用户代码物理地址 */
    size_t user_size;
    uintptr_t free_paddr;       /* 可用内存 */
    size_t free_size;
    uint8_t measurement[64];    /* 远程证明测量值 */
} keystone_enclave_t;

/* 创建Enclave */
int keystone_create_enclave(const char *runtime_path,
                            const char *user_path,
                            keystone_enclave_t *enclave) {
    /* 1. 加载运行时 */
    size_t runtime_size;
    void *runtime = load_file(runtime_path, &runtime_size);

    /* 2. 分配安全物理内存 */
    enclave->runtime_paddr = alloc_secure_pages(runtime_size);
    enclave->runtime_size = runtime_size;

    /* 3. 复制运行时 */
    memcpy((void *)enclave->runtime_paddr, runtime, runtime_size);

    /* 4. 加载用户代码 */
    size_t user_size;
    void *user = load_file(user_path, &user_size);

    enclave->user_paddr = alloc_secure_pages(user_size);
    enclave->user_size = user_size;

    memcpy((void *)enclave->user_paddr, user, user_size);

    /* 5. 分配自由内存 */
    enclave->free_paddr = alloc_secure_pages(FREE_MEMORY_SIZE);
    enclave->free_size = FREE_MEMORY_SIZE;

    /* 6. 计算测量值 */
    compute_enclave_measurement(enclave);

    /* 7. 配置PMP条目 */
    keystone_setup_pmp(enclave);

    /* 8. 分配EID */
    enclave->eid = allocate_eid();

    return 0;
}

/* 运行Enclave */
int keystone_run_enclave(keystone_enclave_t *enclave,
                         uint64_t *ret_val) {
    /* 保存主机上下文 */
    save_host_context();

    /* 设置Enclave入口 */
    write_csr(CSR_MEPC, enclave->runtime_paddr);

    /* 设置参数 */
    /* ... */

    /* 进入Enclave */
    mret();

    /* Enclave返回后 */
    restore_host_context();

    *ret_val = read_csr(CSR_MCAUSE);

    return 0;
}

/* 处理Enclave退出 */
void keystone_handle_exit(uintptr_t cause) {
    switch (cause) {
        case CAUSE_ENCLAVE_ECALL:
            /* Enclave系统调用 */
            handle_enclave_syscall();
            break;

        case CAUSE_ENCLAVE_FAULT:
            /* Enclave页面错误 */
            handle_enclave_page_fault();
            break;

        case CAUSE_ENCLAVE_INTERRUPT:
            /* Enclave中断 */
            handle_enclave_interrupt();
            break;

        default:
            panic("Unknown enclave exit cause: %lx", cause);
    }
}
