/*
 * Auto-generated from: 07_Modern_Toolchain\09_System_Architecture\01_Microkernel_Architecture.md
 * Line: 96
 * Language: c
 * Block ID: 936ad209
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 块设备驱动 (用户态) */
void block_driver_main(void) {
    message_t msg;

    while (1) {
        receive(ANY, &msg);

        switch (msg.type) {
            case BLOCK_READ:
                handle_read(msg.data);
                break;
            case BLOCK_WRITE:
                handle_write(msg.data);
                break;
            case BLOCK_IOCTL:
                handle_ioctl(msg.data);
                break;
        }

        /* 回复 */
        msg.dest = msg.source;
        send(msg.source, &msg);
    }
}

/* 内核只处理底层中断 */
void disk_interrupt_handler(void) {
    /* 通知驱动进程 */
    notify(DISK_DRIVER_PID);
}
