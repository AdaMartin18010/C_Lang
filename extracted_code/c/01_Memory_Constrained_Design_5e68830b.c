/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Memory_Constrained\01_Memory_Constrained_Design.md
 * Line: 1319
 * Language: c
 * Block ID: 5e68830b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ============================================================================
 * 堆使用风险 - 为什么内存受限系统应避免堆
 * ============================================================================ */

/* 风险1: 内存碎片 */
void fragmentation_demo(void)
{
    /* 初始: 空闲内存 100字节 [                  ]
     */

    void *a = malloc(30);  /* [AAAAAA            ] 分配30字节 */
    void *b = malloc(30);  /* [AAAAAABBBBBB      ] 分配30字节 */
    void *c = malloc(30);  /* [AAAAAABBBBBBCCCCCC] 分配30字节 */

    free(b);               /* [AAAAAA      CCCCCC] 释放B */
    /* 现在30字节空闲，但如果在A和C之间 */

    void *d = malloc(40);  /* 失败！虽然有30+30字节，但不连续 */

    free(a);
    free(c);
    free(d);
}

/* 风险2: 分配失败处理复杂 */
void* risky_malloc(size_t size)
{
    void *ptr = malloc(size);
    /* 错误! 分配失败返回NULL，调用者必须检查 */
    return ptr;
}

/* 风险3: 内存泄漏 */
void memory_leak_example(void)
{
    for (int i = 0; i < 100; i++) {
        uint8_t *buffer = malloc(64);
        /* 处理... */
        if (some_error_condition()) {
            return;  /* 错误! 内存泄漏 */
        }
        free(buffer);  /* 可能永远不会执行 */
    }
}

/* 风险4: 堆栈碰撞 */
/*
 * 堆向上增长，栈向下增长
 * 如果没有边界检查，可能相互覆盖
 */

/* ============================================================================
 * 安全的内存分配模式
 * ============================================================================ */

/* 模式1: 完全避免堆 - 静态分配 */
/* 编译时确定所有内存需求 */
static uint8_t uart_rx_buffer[64];
static uint8_t uart_tx_buffer[64];
static sensor_data_t sensor_buffers[4];
static message_t message_pool[8];

/* 模式2: 一次性分配 */
/* 启动时分配，运行时不释放 */
typedef struct {
    uint8_t *config_data;
    uint8_t *working_buffer;
    sensor_data_t *sensors;
} system_buffers_t;

static system_buffers_t sys_bufs;

bool system_init_buffers(void)
{
    /* 启动时一次性分配 */
    static uint8_t memory_arena[2048];  /* 静态内存区 */
    uint16_t offset = 0;

    sys_bufs.config_data = memory_arena + offset;
    offset += 256;

    sys_bufs.working_buffer = memory_arena + offset;
    offset += 512;

    sys_bufs.sensors = (sensor_data_t *)(memory_arena + offset);
    offset += 4 * sizeof(sensor_data_t);

    if (offset > sizeof(memory_arena)) {
        return false;  /* 内存不足 */
    }

    return true;
}

/* 模式3: 内存池 (见第3节) */
/* 使用固定块大小的内存池避免碎片 */

/* 模式4: 栈分配 */
/* 临时数据使用栈 */
void process_temp_data(void)
{
    uint8_t temp[128];  /* 栈分配 */

    read_data(temp, sizeof(temp));
    process(temp, sizeof(temp));
    /* 自动释放 */
}

/* 如果需要"持久"数据，复制到静态区 */
static uint8_t persistent_data[128];

void save_persistent_data(void)
{
    uint8_t temp[128];

    read_data(temp, sizeof(temp));
    memcpy(persistent_data, temp, sizeof(temp));
}

/* 模式5: 双缓冲 */
/* 两个缓冲区交替使用 */
static uint8_t buffer_a[256];
static uint8_t buffer_b[256];
static volatile uint8_t active_buffer = 0;

uint8_t* get_working_buffer(void)
{
    return active_buffer ? buffer_b : buffer_a;
}

uint8_t* get_backup_buffer(void)
{
    return active_buffer ? buffer_a : buffer_b;
}

void swap_buffers(void)
{
    active_buffer = !active_buffer;
}
