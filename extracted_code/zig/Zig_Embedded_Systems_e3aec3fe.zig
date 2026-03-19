//
// Auto-generated from: Zig\Zig_Embedded_Systems.md
// Line: 574
// Language: zig
// Block ID: e3aec3fe
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// interrupts.zig

// 外部中断 0 处理
export fn EXTI0_IRQHandler() void {
    // 清除中断标志
    mmio.write_volatile(u32, 0x40013C14, 0x01);

    // 用户代码
    gpio.Pin.init(.A, 0).toggle();
}

// 定时器中断
export fn TIM2_IRQHandler() void {
    const TIM2_SR = 0x40000010;

    // 检查更新中断
    if (mmio.read_volatile(u32, TIM2_SR) & 0x01 != 0) {
        // 清除标志
        mmio.write_volatile(u32, TIM2_SR, ~@as(u32, 0x01));

        // 时基递增
        system_tick += 1;
    }
}

var system_tick: u32 = 0;

pub fn get_tick() u32 {
    return @atomicLoad(u32, &system_tick, .Monotonic);
}

pub fn delay_ms(ms: u32) void {
    const start = get_tick();
    while (get_tick() - start < ms) {
        asm volatile ("wfi");  // 等待中断
    }
}
