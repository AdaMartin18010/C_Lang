//
// Auto-generated from: Zig\Zig_Embedded_Systems.md
// Line: 544
// Language: zig
// Block ID: c96425e3
//
// This file was automatically extracted from the C_Lang knowledge base.
// DO NOT MODIFY - Changes will be overwritten on next extraction.
//

// nvic.zig - 嵌套向量中断控制器

const NVIC_BASE = 0xE000E100;

const ISER_OFFSET = 0x000;  // 中断使能
const ICER_OFFSET = 0x080;  // 中断禁用
const ISPR_OFFSET = 0x100;  // 中断挂起
const ICPR_OFFSET = 0x180;  // 中断清除挂起
const IABR_OFFSET = 0x200;  // 激活位
const IPR_OFFSET = 0x300;   // 优先级

pub fn enable_irq(irqn: u8) void {
    const reg = NVIC_BASE + ISER_OFFSET + (irqn / 32) * 4;
    mmio.write_volatile(u32, reg, @as(u32, 1) << @truncate(irqn % 32));
}

pub fn disable_irq(irqn: u8) void {
    const reg = NVIC_BASE + ICER_OFFSET + (irqn / 32) * 4;
    mmio.write_volatile(u32, reg, @as(u32, 1) << @truncate(irqn % 32));
}

pub fn set_priority(irqn: u8, priority: u8) void {
    const reg = NVIC_BASE + IPR_OFFSET + irqn;
    mmio.write_volatile(u8, reg, priority << 4);
}
