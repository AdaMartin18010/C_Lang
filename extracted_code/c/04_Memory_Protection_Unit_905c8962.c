/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\05_Memory_Management_Hardware\04_Memory_Protection_Unit.md
 * Line: 281
 * Language: c
 * Block ID: 905c8962
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * ARM Cortex-M MPU 配置示例
 */

#include <stdint.h>

// MPU寄存器定义 (Cortex-M3/M4)
#define MPU_BASE        0xE000ED90

#define MPU_TYPER       (*(volatile uint32_t *)(MPU_BASE + 0x00))
#define MPU_CTRL        (*(volatile uint32_t *)(MPU_BASE + 0x04))
#define MPU_RNR         (*(volatile uint32_t *)(MPU_BASE + 0x08))
#define MPU_RBAR        (*(volatile uint32_t *)(MPU_BASE + 0x0C))
#define MPU_RASR        (*(volatile uint32_t *)(MPU_BASE + 0x10))

// MPU_CTRL位
#define MPU_CTRL_ENABLE     (1 << 0)
#define MPU_CTRL_HFNMIENA   (1 << 1)
#define MPU_CTRL_PRIVDEFENA (1 << 2)

// RASR位域
#define RASR_XN             (1 << 28)
#define RASR_AP_SHIFT       24
#define RASR_TEX_SHIFT      19
#define RASR_S              (1 << 18)
#define RASR_C              (1 << 17)
#define RASR_B              (1 << 16)
#define RASR_SRD_SHIFT      8
#define RASR_SIZE_SHIFT     1
#define RASR_ENABLE         (1 << 0)

// 访问权限
#define AP_NO_ACCESS        0x0
#define AP_PRIV_RW          0x1
#define AP_PRIV_RW_USER_RO  0x2
#define AP_PRIV_RW_USER_RW  0x3
#define AP_PRIV_RO          0x5
#define AP_PRIV_RO_USER_RO  0x6

/*
 * MPU区域大小编码
 * 实际大小 = 2^(SIZE+1) bytes
 */
typedef enum {
    MPU_SIZE_32B    = 0x04,   // 2^5 = 32B
    MPU_SIZE_64B    = 0x05,   // 2^6 = 64B
    MPU_SIZE_128B   = 0x06,   // 2^7 = 128B
    MPU_SIZE_256B   = 0x07,   // 2^8 = 256B
    MPU_SIZE_512B   = 0x08,   // 2^9 = 512B
    MPU_SIZE_1KB    = 0x09,   // 2^10 = 1KB
    MPU_SIZE_2KB    = 0x0A,   // 2^11 = 2KB
    MPU_SIZE_4KB    = 0x0B,   // 2^12 = 4KB
    MPU_SIZE_8KB    = 0x0C,   // 2^13 = 8KB
    MPU_SIZE_16KB   = 0x0D,   // 2^14 = 16KB
    MPU_SIZE_32KB   = 0x0E,   // 2^15 = 32KB
    MPU_SIZE_64KB   = 0x0F,   // 2^16 = 64KB
    MPU_SIZE_128KB  = 0x10,   // 2^17 = 128KB
    MPU_SIZE_256KB  = 0x11,   // 2^18 = 256KB
    MPU_SIZE_512KB  = 0x12,   // 2^19 = 512KB
    MPU_SIZE_1MB    = 0x13,   // 2^20 = 1MB
    MPU_SIZE_2MB    = 0x14,   // 2^21 = 2MB
    MPU_SIZE_4MB    = 0x15,   // 2^22 = 4MB
    MPU_SIZE_8MB    = 0x16,   // 2^23 = 8MB
    MPU_SIZE_16MB   = 0x17,   // 2^24 = 16MB
    MPU_SIZE_32MB   = 0x18,   // 2^25 = 32MB
    MPU_SIZE_64MB   = 0x19,   // 2^26 = 64MB
    MPU_SIZE_128MB  = 0x1A,   // 2^27 = 128MB
    MPU_SIZE_256MB  = 0x1B,   // 2^28 = 256MB
    MPU_SIZE_512MB  = 0x1C,   // 2^29 = 512MB
    MPU_SIZE_1GB    = 0x1D,   // 2^30 = 1GB
    MPU_SIZE_2GB    = 0x1E,   // 2^31 = 2GB
    MPU_SIZE_4GB    = 0x1F,   // 2^32 = 4GB
} mpu_region_size_t;

/*
 * 配置MPU区域
 */
void mpu_configure_region(uint32_t region_num,
                          uint32_t base_addr,
                          mpu_region_size_t size,
                          uint32_t attributes) {
    // 选择区域
    MPU_RNR = region_num;

    // 设置基址 (必须对齐到区域大小)
    MPU_RBAR = base_addr & 0xFFFFFFE0;

    // 设置属性
    MPU_RASR = attributes | (size << RASR_SIZE_SHIFT) | RASR_ENABLE;
}

/*
 * 典型的嵌入式系统MPU配置
 */
void mpu_setup_typical(void) {
    /*
     * 配置区域:
     *
     * Region 0: 代码段 (Flash)
     * - 基址: 0x08000000
     * - 大小: 512KB
     * - 属性: 只读执行，可缓存
     */
    mpu_configure_region(0,
                         0x08000000,
                         MPU_SIZE_512KB,
                         (AP_PRIV_RO_USER_RO << RASR_AP_SHIFT) |  // 只读
                         RASR_C | RASR_B);                         // 可缓存

    /*
     * Region 1: 数据段 (SRAM)
     * - 基址: 0x20000000
     * - 大小: 128KB
     * - 属性: 读写，可缓存
     */
    mpu_configure_region(1,
                         0x20000000,
                         MPU_SIZE_128KB,
                         (AP_PRIV_RW_USER_RW << RASR_AP_SHIFT) |  // 读写
                         RASR_XN |                                  // 不可执行
                         RASR_C | RASR_B);                          // 可缓存

    /*
     * Region 2: 栈区 (SRAM的一部分)
     * - 基址: 0x20020000
     * - 大小: 32KB
     * - 属性: 读写，可缓存
     */
    mpu_configure_region(2,
                         0x20020000,
                         MPU_SIZE_32KB,
                         (AP_PRIV_RW << RASR_AP_SHIFT) |  // 特权读写，用户无访问
                         RASR_XN |                          // 不可执行
                         RASR_C | RASR_B);

    /*
     * Region 3: 外设区域
     * - 基址: 0x40000000
     * - 大小: 512MB (通常根据具体外设调整)
     * - 属性: 读写，不可缓存，设备内存
     */
    mpu_configure_region(3,
                         0x40000000,
                         MPU_SIZE_512MB,
                         (AP_PRIV_RW << RASR_AP_SHIFT) |  // 特权读写
                         RASR_XN |                          // 不可执行
                         (2 << RASR_TEX_SHIFT));            // 设备内存

    /*
     * Region 4: 系统控制块 (SCB, NVIC等)
     * - 基址: 0xE0000000
     * - 大小: 1MB
     * - 属性: 读写，强有序 (设备内存)
     */
    mpu_configure_region(4,
                         0xE0000000,
                         MPU_SIZE_1MB,
                         (AP_PRIV_RW << RASR_AP_SHIFT) |
                         RASR_XN |
                         (0 << RASR_TEX_SHIFT));  // 强有序

    /*
     * 启用MPU
     * - 使能MPU
     * - 启用默认内存映射 (未配置区域按默认属性访问)
     * - 允许在HardFault/NMI中使用MPU
     */
    MPU_CTRL = MPU_CTRL_ENABLE | MPU_CTRL_PRIVDEFENA | MPU_CTRL_HFNMIENA;

    /*
     * 屏障指令确保MPU配置生效
     */
    __DSB();  // 数据同步屏障
    __ISB();  // 指令同步屏障
}

/*
 * 保护堆栈溢出检测
 * 在栈底设置一个 guard page
 */
void mpu_setup_stack_guard(void *stack_bottom) {
    /*
     * Region 7: 栈保护页 (guard page)
     * - 大小: 32字节 (最小)
     * - 属性: 无访问权限
     *
     * 当栈溢出时访问此区域会触发MemManage Fault
     */
    mpu_configure_region(7,
                         (uint32_t)stack_bottom - 32,
                         MPU_SIZE_32B,
                         (AP_NO_ACCESS << RASR_AP_SHIFT) |
                         RASR_XN);
}
