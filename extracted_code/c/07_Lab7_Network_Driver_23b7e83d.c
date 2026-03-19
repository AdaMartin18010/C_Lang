/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\07_Lab7_Network_Driver.md
 * Line: 291
 * Language: c
 * Block ID: 23b7e83d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/e1000.c

int
e1000_transmit(struct mbuf *m)
{
    acquire(&e1000_lock);

    // 获取当前尾指针
    uint32 tail = e1000_regs[E1000_TDT / 4];
    uint32 next_tail = (tail + 1) % NTXDESC;

    // 检查是否有空闲描述符
    if (!(tx_ring[tail].status & TX_STATUS_DD)) {
        release(&e1000_lock);
        printf("e1000_transmit: no free descriptors\n");
        return -1;
    }

    // 如果有之前的数据包，释放它
    if (tx_mbuf[tail]) {
        mbuffree(tx_mbuf[tail]);
    }

    // 填充描述符
    tx_ring[tail].addr = (uint64)m->head;
    tx_ring[tail].length = m->len;
    tx_ring[tail].cmd = TX_CMD_EOP | TX_CMD_IFCS | TX_CMD_RS;
    tx_ring[tail].status = 0;

    // 保存mbuf指针以便稍后释放
    tx_mbuf[tail] = m;

    // 更新尾指针，通知硬件
    e1000_regs[E1000_TDT / 4] = next_tail;

    release(&e1000_lock);
    return 0;
}
