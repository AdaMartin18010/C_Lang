/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\07_Lab7_Network_Driver.md
 * Line: 216
 * Language: c
 * Block ID: 9b678425
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/e1000.c

int
e1000_init(void)
{
    // 1. 查找PCI设备
    // 扫描PCI总线找到E1000
    // ... (PCI枚举代码)

    // 2. 映射BAR0寄存器
    // e1000_regs = (uint32 *)map_bar0();

    // 3. 复位设备
    uint32 ctrl = e1000_regs[E1000_CTL / 4];
    e1000_regs[E1000_CTL / 4] = ctrl | E1000_CTL_RST;

    // 等待复位完成
    while (e1000_regs[E1000_CTL / 4] & E1000_CTL_RST)
        ;

    // 4. 初始化MAC地址
    // 从EEPROM读取或使用硬编码
    e1000_read_mac();

    // 5. 初始化接收描述符环
    memset(rx_ring, 0, sizeof(rx_ring));
    for (int i = 0; i < NRXDESC; i++) {
        rx_ring[i].addr = (uint64)rx_bufs[i];
        rx_ring[i].status = 0;  // 硬件可写入
    }

    // 设置接收描述符环基地址
    e1000_regs[E1000_RDBAL / 4] = (uint64)rx_ring & 0xFFFFFFFF;
    e1000_regs[E1000_RDBAH / 4] = (uint64)rx_ring >> 32;
    e1000_regs[E1000_RDLEN / 4] = sizeof(rx_ring);
    e1000_regs[E1000_RDH / 4] = 0;
    e1000_regs[E1000_RDT / 4] = NRXDESC - 1;  // 初始时所有描述符可用

    // 6. 初始化发送描述符环
    memset(tx_ring, 0, sizeof(tx_ring));
    for (int i = 0; i < NTXDESC; i++) {
        tx_ring[i].status = TX_STATUS_DD;  // 初始时所有描述符空闲
    }

    // 设置发送描述符环基地址
    e1000_regs[E1000_TDBAL / 4] = (uint64)tx_ring & 0xFFFFFFFF;
    e1000_regs[E1000_TDBAH / 4] = (uint64)tx_ring >> 32;
    e1000_regs[E1000_TDLEN / 4] = sizeof(tx_ring);
    e1000_regs[E1000_TDH / 4] = 0;
    e1000_regs[E1000_TDT / 4] = 0;

    // 7. 配置接收控制寄存器
    e1000_regs[E1000_RCTL / 4] = E1000_RCTL_EN |      // 使能接收
                                  E1000_RCTL_BAM |     // 广播使能
                                  E1000_RCTL_SZ_2048 | // 2048字节缓冲区
                                  E1000_RCTL_SECRC;    // 剥离CRC

    // 8. 配置发送控制寄存器
    e1000_regs[E1000_TCTL / 4] = E1000_TCTL_EN |      // 使能发送
                                  E1000_TCTL_PSP;      // Pad短包

    // 9. 配置中断
    e1000_regs[E1000_IMS / 4] = 0x1;  // 使能接收中断

    printf("e1000: initialized, mac=%x:%x:%x:%x:%x:%x\n",
           mac_addr[0], mac_addr[1], mac_addr[2],
           mac_addr[3], mac_addr[4], mac_addr[5]);

    return 0;
}
