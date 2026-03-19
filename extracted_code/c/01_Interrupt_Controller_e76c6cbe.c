/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\09_Interrupt_Hardware\01_Interrupt_Controller.md
 * Line: 338
 * Language: c
 * Block ID: e76c6cbe
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * Local APIC 寄存器定义
 * arch/x86/include/asm/apicdef.h
 */

#define APIC_DEFAULT_PHYS_BASE  0xFEE00000

// Local APIC 寄存器偏移
#define APIC_ID         0x020   // Local APIC ID
#define APIC_VER        0x030   // Local APIC Version
#define APIC_TPR        0x080   // Task Priority Register
#define APIC_APR        0x090   // Arbitration Priority Register
#define APIC_PPR        0x0A0   // Processor Priority Register
#define APIC_EOI        0x0B0   // End of Interrupt
#define APIC_RRR        0x0C0   // Remote Read Register
#define APIC_LDR        0x0D0   // Logical Destination Register
#define APIC_DFR        0x0E0   // Destination Format Register
#define APIC_SVR        0x0F0   // Spurious Interrupt Vector
#define APIC_ISR        0x100   // In-Service Register (256-bit)
#define APIC_TMR        0x180   // Trigger Mode Register (256-bit)
#define APIC_IRR        0x200   // Interrupt Request Register (256-bit)
#define APIC_ESR        0x280   // Error Status Register
#define APIC_ICR        0x300   // Interrupt Command Register (64-bit)
#define APIC_ICR2       0x310   // ICR[63:32]
#define APIC_LVTT       0x320   // LVT Timer Register
#define APIC_LVTTHMR    0x330   // LVT Thermal Monitor
#define APIC_LVTPC      0x340   // LVT Performance Counter
#define APIC_LVT0       0x350   // LVT LINT0
#define APIC_LVT1       0x360   // LVT LINT1
#define APIC_LVTERR     0x370   // LVT Error
#define APIC_TMICT      0x380   // Timer Initial Count
#define APIC_TMCCT      0x390   // Timer Current Count
#define APIC_TDCR       0x3E0   // Timer Divide Configuration

// ICR (Interrupt Command Register) 定义
#define APIC_DEST_SELF          0x40000
#define APIC_DEST_ALLINC        0x80000
#define APIC_DEST_ALLBUT        0xC0000
#define APIC_DEST_LOGICAL       0x800
#define APIC_DM_FIXED           0x000
#define APIC_DM_LOWEST          0x100
#define APIC_DM_SMI             0x200
#define APIC_DM_NMI             0x400
#define APIC_DM_INIT            0x500
#define APIC_DM_STARTUP         0x600
#define APIC_DM_EXTINT          0x700
#define APIC_VECTOR_MASK        0x000FF
