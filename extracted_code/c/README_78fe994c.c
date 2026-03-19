/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\01_Standard_IO\README.md
 * Line: 614
 * Language: c
 * Block ID: 78fe994c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 每个线程使用独立的缓冲区 */
_Thread_local char thread_buffer[8192];

void thread_local_io(FILE *fp) {
    /* 为每个线程设置独立缓冲区 */
    setvbuf(fp, thread_buffer, _IOFBF, sizeof(thread_buffer));

    /* 后续 I/O 操作... */
}
