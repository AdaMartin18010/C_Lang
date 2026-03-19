/*
 * Auto-generated from: 07_Modern_Toolchain\09_System_Architecture\01_Microkernel_Architecture.md
 * Line: 349
 * Language: c
 * Block ID: 19cf65f7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 大块数据传输使用共享内存 */

typedef struct {
    void* shm_addr;       /* 共享内存地址 */
    size_t shm_size;      /* 共享内存大小 */
    uint32_t seq_num;     /* 序列号用于同步 */
} shm_channel_t;

int setup_shm_ipc(shm_channel_t* channel, size_t size) {
    /* 分配物理页面 */
    void* pages = alloc_phys_pages(PAGE_ALIGN(size) / PAGE_SIZE);
    if (!pages) return -ENOMEM;

    /* 映射到当前进程 */
    channel->shm_addr = mmap(NULL, size, PROT_READ | PROT_WRITE,
                             MAP_SHARED, pages);
    channel->shm_size = size;
    channel->seq_num = 0;

    return 0;
}

int shm_send(shm_channel_t* channel, void* data, size_t len) {
    if (len > channel->shm_size) return -EINVAL;

    /* 等待接收者就绪 */
    while (channel->seq_num & 1) {
        cpu_relax();
    }

    /* 写入数据 */
    memcpy(channel->shm_addr, data, len);
    memory_barrier();

    /* 通知接收者 */
    channel->seq_num++;

    return 0;
}

int shm_receive(shm_channel_t* channel, void* buf, size_t len) {
    uint32_t seq = channel->seq_num;

    /* 等待新数据 */
    while (!(seq & 1) || seq == channel->seq_num) {
        seq = channel->seq_num;
        cpu_relax();
    }

    memory_barrier();

    /* 读取数据 */
    memcpy(buf, channel->shm_addr, len);

    /* 确认接收 */
    channel->seq_num++;

    return 0;
}
