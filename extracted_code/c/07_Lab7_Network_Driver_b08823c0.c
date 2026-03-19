/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\07_Lab7_Network_Driver.md
 * Line: 334
 * Language: c
 * Block ID: b08823c0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/e1000.c

static void
e1000_recv(void)
{
    // 获取当前头指针
    uint32 tail = (e1000_regs[E1000_RDT / 4] + 1) % NRXDESC;

    // 处理所有已完成的描述符
    while (rx_ring[tail].status & RX_STATUS_DD) {
        // 检查是否有错误
        if (rx_ring[tail].errors) {
            printf("e1000_recv: packet error %x\n", rx_ring[tail].errors);
        } else {
            // 将数据包传递给网络栈
            struct mbuf *m = mbufalloc(0);
            if (m) {
                uint16 len = rx_ring[tail].length;
                if (len > MBUF_SIZE)
                    len = MBUF_SIZE;

                memmove(m->head, rx_bufs[tail], len);
                m->len = len;

                // 传递给上层 (IP层)
                net_rx(m);
            }
        }

        // 重置描述符状态
        rx_ring[tail].status = 0;

        // 更新尾指针，释放描述符给硬件
        e1000_regs[E1000_RDT / 4] = tail;

        // 下一个描述符
        tail = (tail + 1) % NRXDESC;
    }
}
