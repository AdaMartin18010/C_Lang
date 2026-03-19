/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\04_Lab4_Traps.md
 * Line: 624
 * Language: c
 * Block ID: 22b47603
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/trap.c

int
devintr(void)
{
    uint64 scause = r_scause();

    if ((scause & 0x8000000000000000L) &&
        (scause & 0xff) == 9) {
        //  supervisor外部中断 (PLIC)
        int irq = plic_claim();

        if (irq == UART0_IRQ) {
            uartintr();
        } else if (irq == VIRTIO0_IRQ) {
            virtio_disk_intr();
        } else if (irq) {
            printf("unexpected interrupt irq=%d\n", irq);
        }

        if (irq)
            plic_complete(irq);

        return 1;
    } else if (scause == 0x8000000000000001L) {
        //  supervisor定时器中断
        if (cpuid() == 0) {
            clockintr();
        }
        w_sip(r_sip() & ~2);  // 清除中断
        return 2;
    }

    return 0;
}
