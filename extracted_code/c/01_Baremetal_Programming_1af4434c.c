/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Baremetal\01_Baremetal_Programming.md
 * Line: 4501
 * Language: c
 * Block ID: 1af4434c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @brief 链接器脚本定义的符号声明
 */
extern uint32_t _estack;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;
extern uint32_t __heap_start;
extern uint32_t __heap_end;

/**
 * @brief 获取栈使用情况
 */
uint32_t Stack_GetUnused(void)
{
    /* 栈从高地址向低地址增长 */
    volatile uint32_t *p = &_ebss;
    uint32_t unused = 0;

    /* 查找未使用的栈空间 (填充模式) */
    while (*p == 0xDEADBEEF && p < &_estack) {
        unused += 4;
        p++;
    }

    return unused;
}

/**
 * @brief 获取堆使用情况
 */
typedef struct {
    void *ptr;
    uint32_t size;
    uint8_t used;
} Heap_Block;

#define HEAP_BLOCKS 32
static Heap_Block g_heapBlocks[HEAP_BLOCKS];

void* malloc_simple(uint32_t size)
{
    /* 简单堆分配实现 */
    static uint8_t *heapPtr = (uint8_t *)&__heap_start;

    void *ptr = heapPtr;
    heapPtr += (size + 3) & ~3;  /* 4字节对齐 */

    if ((uint32_t)heapPtr > (uint32_t)&__heap_end) {
        return NULL;  /* 堆溢出 */
    }

    return ptr;
}

void Memory_PrintUsage(void (*print)(const char *fmt, ...))
{
    uint32_t textSize = (uint32_t)&_etext - 0x08000000;
    uint32_t dataSize = (uint32_t)&_edata - (uint32_t)&_sdata;
    uint32_t bssSize = (uint32_t)&_ebss - (uint32_t)&_sbss;
    uint32_t stackSize = (uint32_t)&_estack - (uint32_t)&_ebss;

    print("Memory Usage:\r\n");
    print("  Flash (Text): %d bytes\r\n", textSize);
    print("  RAM (Data):   %d bytes\r\n", dataSize);
    print("  RAM (BSS):    %d bytes\r\n", bssSize);
    print("  Stack Total:  %d bytes\r\n", stackSize);
}
