//
// Auto-generated from: Zig\Zig_Embedded_Systems.md
// Line: 620
// Language: zig
// Block ID: f917e88f
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// 导入 CMSIS
const cmsis = @cImport({
    @cInclude("stm32f4xx.h");
});

// 使用 C 定义
pub fn use_cmsis() void {
    // 直接访问 C 结构体
    cmsis.RCC->AHB1ENR |= cmsis.RCC_AHB1ENR_GPIOAEN;

    // 配置 GPIO
    cmsis.GPIOA->MODER |= cmsis.GPIO_MODER_MODER5_0;
}

// 链接 C 库
// build.zig:
// lib.addCSourceFile("stm32f4xx_gpio.c", &.{});
// lib.addIncludePath("CMSIS/Include");
