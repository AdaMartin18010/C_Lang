/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\02_Embedded_Multiplatform_Protocols.md
 * Line: 145
 * Language: c
 * Block ID: de8d6f33
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ============================================================
// 启动流程差异分析
// ============================================================

// ========== STM32启动流程 ==========
// 1. 上电后从0x08000000(Flash)或0x1FFF0000(系统存储器)启动
// 2. 读取栈顶指针(0x08000000)和复位向量(0x08000004)
// 3. 执行启动文件(assembly)初始化栈、复制.data、清零.bss
// 4. 调用SystemInit()配置时钟
// 5. 调用main()

// startup_stm32f4xx.s (简化)
__isr_vector:
    .word _estack          // Top of stack
    .word Reset_Handler    // Reset Handler
    .word NMI_Handler      // NMI Handler
    // ...

Reset_Handler:
    // 复制.data从Flash到RAM
    ldr r0, =_sidata
    ldr r1, =_sdata
    ldr r2, =_edata
    b LoopCopyDataInit
CopyDataInit:
    ldr r3, [r0, #0]
    str r3, [r1, #0]
    adds r0, r0, #4
    adds r1, r1, #4
LoopCopyDataInit:
    cmp r1, r2
    bcc CopyDataInit

    // 清零.bss
    // ...

    // 调用系统初始化
    bl SystemInit

    // 调用main
    bl main
    bx lr

// ========== ESP32启动流程 ==========
// ESP32使用二级引导程序(Bootloader)
// 1. ROM中的第一级引导(不可修改)加载Flash 0x1000的Bootloader
// 2. Bootloader读取分区表，加载应用程序
// 3. 初始化DRAM、配置Cache、设置CPU频率
// 4. 跳转到应用程序入口(app_main)

// ESP32的特殊性: FreeRTOS是默认OS
void app_main(void) {
    // ESP-IDF框架下，main变成了app_main
    // 系统已经初始化完成，包括WiFi/BLE栈

    // 创建任务
    xTaskCreate(sensor_task, "sensor", 4096, NULL, 5, NULL);
    xTaskCreate(wifi_task, "wifi", 8192, NULL, 5, NULL);
}

// ========== RP2040启动流程 ==========
// RP2040从片外QSPI Flash启动
// 1. Bootrom(ROM)执行，检测启动模式(Flash/USB/UART)
// 2. 从Flash 0x10000000加载第二级引导
// 3. 初始化PLL、配置时钟、初始化外设
// 4. 启动第二个核心
// 5. 跳转到main()

// RP2040特有的多核启动
void main(void) {
    // 核心0执行这里

    // 启动核心1
    multicore_launch_core1(core1_entry);

    // 核心0继续执行
    core0_main();
}

void core1_entry(void) {
    // 核心1从这里开始执行
    core1_main();
}
