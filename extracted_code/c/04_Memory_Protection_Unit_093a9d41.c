/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\05_Memory_Management_Hardware\04_Memory_Protection_Unit.md
 * Line: 482
 * Language: c
 * Block ID: 093a9d41
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * FreeRTOS与MPU集成示例
 * 实现任务级别的内存隔离
 */

#include "FreeRTOS.h"
#include "task.h"
#include "mpu_wrappers.h"

/*
 * 受MPU保护的任务创建
 */
void create_protected_task(void) {
    /*
     * 定义任务的内存区域
     */
    static const xMemoryRegion xRegions[] = {
        // 区域1: 任务的栈
        {
            .pvBaseAddress = (void *)0x20010000,
            .ulLengthInBytes = 8192,
            .ulParameters = (portMPU_REGION_READ_WRITE |
                           portMPU_REGION_CACHEABLE_BUFFERABLE)
        },
        // 区域2: 任务私有的数据区
        {
            .pvBaseAddress = (void *)0x20012000,
            .ulLengthInBytes = 4096,
            .ulParameters = (portMPU_REGION_READ_WRITE |
                           portMPU_REGION_CACHEABLE_BUFFERABLE)
        },
        // 区域3: 与其他任务共享的数据区 (只读)
        {
            .pvBaseAddress = (void *)0x20014000,
            .ulLengthInBytes = 4096,
            .ulParameters = (portMPU_REGION_READ_ONLY |
                           portMPU_REGION_CACHEABLE_BUFFERABLE)
        }
    };

    TaskParameters_t xTaskParameters = {
        .pvTaskCode = protected_task_function,
        .pcName = "ProtectedTask",
        .usStackDepth = 256,  // 以字为单位
        .pvParameters = NULL,
        .uxPriority = 1,
        .puxStackBuffer = (StackType_t *)0x20010000,  // 预分配栈
        .xRegions = xRegions,
        .mpuParameters = {
            .privileges = tskMPU_READ_WRITE  // 任务运行在特权模式
        }
    };

    TaskHandle_t xHandle;
    xTaskCreateRestricted(&xTaskParameters, &xHandle);
}

/*
 * 受保护的任务函数
 * 只能访问其被授权的区域
 */
void protected_task_function(void *pvParameters) {
    /*
     * 可以访问:
     * - 自己的栈
     * - 自己的数据区
     * - 共享数据区 (只读)
     *
     * 不能访问:
     * - 其他任务的栈或数据
     * - 系统关键区域
     */

    volatile uint32_t *my_data = (uint32_t *)0x20012000;
    volatile const uint32_t *shared_data = (uint32_t *)0x20014000;

    for (;;) {
        // 允许: 访问自己的数据
        *my_data = 0x12345678;

        // 允许: 读取共享数据
        uint32_t value = *shared_data;

        // 会触发MemManage Fault: 写入共享数据区 (只读)
        // *shared_data = 0xDEADBEEF;  // 禁止!

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

/*
 * MemManage Fault处理程序
 */
void MemManage_Handler(void) {
    /*
     * 读取MPU错误状态寄存器
     */
    uint32_t mmfsr = *(volatile uint32_t *)0xE000ED28;
    uint32_t mmar = *(volatile uint32_t *)0xE000ED34;

    if (mmfsr & 0x01) {
        // IACCVIOL: 指令访问违规
        printf("Instruction access violation at 0x%08X\n", mmar);
    }
    if (mmfsr & 0x02) {
        // DACCVIOL: 数据访问违规
        printf("Data access violation at 0x%08X\n", mmar);
    }
    if (mmfsr & 0x08) {
        // MUNSTKERR: 异常退出时堆栈错误
        printf("Unstacking error\n");
    }
    if (mmfsr & 0x10) {
        // MSTKERR: 异常进入时堆栈错误
        printf("Stacking error\n");
    }

    /*
     * 可以选择:
     * 1. 终止违规任务 (如果支持)
     * 2. 重启系统
     * 3. 记录错误并继续
     */

    // 清除错误标志
    *(volatile uint32_t *)0xE000ED28 = mmfsr;

    // 挂起或删除违规任务
    // vTaskDelete(xTaskGetCurrentTaskHandle());
}
