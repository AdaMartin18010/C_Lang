/*
 * Auto-generated from: 01_Core_Knowledge_System\04_Standard_Library_Layer\01_Standard_IO\README.md
 * Line: 190
 * Language: c
 * Block ID: 4e58e6fb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* glibc 中的 _IO_FILE 结构（简化版） */
struct _IO_FILE {
    int _flags;           /* 文件状态标志 */

    /* 缓冲区指针 */
    char* _IO_read_ptr;   /* 当前读取位置 */
    char* _IO_read_end;   /* 可读区域结束 */
    char* _IO_read_base;  /* 读取缓冲区起始 */

    char* _IO_write_base; /* 写入缓冲区起始 */
    char* _IO_write_ptr;  /* 当前写入位置 */
    char* _IO_write_end;  /* 写入缓冲区结束 */

    char* _IO_buf_base;   /* 缓冲区起始 */
    char* _IO_buf_end;    /* 缓冲区结束 */

    /* 底层文件描述符 */
    int _fileno;

    /* 用于多线程同步 */
    int _lock;

    /* 其他字段... */
};
