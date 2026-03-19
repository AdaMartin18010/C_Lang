/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Memory_Constrained\01_Memory_Constrained_Design.md
 * Line: 1193
 * Language: c
 * Block ID: 40b7dbd2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ============================================================================
 * 栈使用指南 - 嵌入式系统的首选分配方式
 * ============================================================================ */

/* 栈的优点：
 * 1. 自动管理 - 函数返回自动释放
 * 2. 无碎片 - 后进先出保证连续性
 * 3. 确定性 - 编译时可计算最大用量
 * 4. 高性能 - 仅需调整SP寄存器
 */

/* 推荐：小数组和结构体使用栈 */
void process_packet_good(uint8_t packet_type)
{
    /* 小缓冲区放栈上 */
    uint8_t header[8];
    uint16_t crc;

    /* 处理数据 */
    header[0] = packet_type;
    header[1] = 0x01;
    crc = calculate_crc(header, 8);

    /* 自动释放 - 无需管理 */
}

/* 不推荐：大数组使用栈 */
void process_packet_bad(void)
{
    /* 危险！可能栈溢出 */
    uint8_t large_buffer[2048];  /* 2KB在栈上 */
    /* ... */
}

/* 栈大小估算 */
/* 每个函数调用需要：
 * - 返回地址: 4字节 (ARM)
 * - 保存的寄存器: 4-32字节
 * - 局部变量: 视情况而定
 * - 对齐填充: 0-3字节
 */

/* 中断栈使用计算示例 */
void calculate_stack_usage(void)
{
    /* 主循环最大调用深度:
     * main() -> loop() -> process() -> handle() = 4层
     * 每层 ~32字节 = 128字节
     */

    /* 中断最坏情况:
     * - 中断发生时主循环在深层调用
     * - 高优先级中断打断低优先级中断
     *
     * 假设3级中断嵌套:
     * - 保存上下文: 16-64字节/级
     * - 中断处理: 32-128字节/级
     * 总计: ~500字节
     */

    /* 建议栈大小 = 正常用量 * 2 + 中断用量
     * = 128 * 2 + 500 = 756字节最小
     * 实际建议分配 2-4KB
     */
}

/* 递归的替代方案 */
/* 不推荐：递归消耗不确定栈空间 */
int factorial_recursive(int n)
{
    if (n <= 1) return 1;
    return n * factorial_recursive(n - 1);
}

/* 推荐：迭代使用固定栈空间 */
int factorial_iterative(int n)
{
    int result = 1;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}

/* 变长数组的替代 */
/* 不推荐：VLA使用不确定栈空间 */
void process_vla_bad(int size)
{
    uint8_t buffer[size];  /* VLA - 避免使用 */
    /* ... */
}

/* 推荐：使用最大固定大小 */
#define MAX_BUFFER_SIZE 128
void process_fixed_good(uint8_t size)
{
    uint8_t buffer[MAX_BUFFER_SIZE];

    if (size > MAX_BUFFER_SIZE) {
        size = MAX_BUFFER_SIZE;  /* 限制大小 */
    }
    /* ... */
}

/* 或分段处理 */
void process_in_chunks(const uint8_t *data, uint16_t total_size)
{
    uint8_t chunk[32];
    uint16_t processed = 0;

    while (processed < total_size) {
        uint16_t chunk_size = total_size - processed;
        if (chunk_size > sizeof(chunk)) {
            chunk_size = sizeof(chunk);
        }

        memcpy(chunk, data + processed, chunk_size);
        process_chunk(chunk, chunk_size);
        processed += chunk_size;
    }
}
