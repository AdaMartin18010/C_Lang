/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\20_Industrial_Case_Studies\03_Baremetal_Embedded.md
 * Line: 977
 * Language: c
 * Block ID: a84ec085
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// syscalls.c - newlib系统调用实现

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

// UART输出
extern void uart_putc(char c);
extern char uart_getc(void);

// _exit - 程序退出
void _exit(int status) {
    // 通知调试器
    __asm__ __volatile__("ebreak");

    // 无限循环
    while (1) {
        __asm__ __volatile__("wfi");
    }
}

// _sbrk - 堆内存扩展
extern char _heap_start[];
extern char _heap_end[];
static char *heap_ptr = NULL;

caddr_t _sbrk(int incr) {
    if (heap_ptr == NULL) {
        heap_ptr = _heap_start;
    }

    char *prev_heap_ptr = heap_ptr;

    if (heap_ptr + incr > _heap_end) {
        errno = ENOMEM;
        return (caddr_t)-1;
    }

    heap_ptr += incr;
    return (caddr_t)prev_heap_ptr;
}

// _write - 写文件
ssize_t _write(int fd, const void *buf, size_t count) {
    if (fd == 1 || fd == 2) {  // stdout或stderr
        const char *ptr = buf;
        for (size_t i = 0; i < count; i++) {
            uart_putc(ptr[i]);
            if (ptr[i] == '\n') {
                uart_putc('\r');
            }
        }
        return count;
    }
    errno = EBADF;
    return -1;
}

// _read - 读文件
ssize_t _read(int fd, void *buf, size_t count) {
    if (fd == 0) {  // stdin
        char *ptr = buf;
        for (size_t i = 0; i < count; i++) {
            ptr[i] = uart_getc();
        }
        return count;
    }
    errno = EBADF;
    return -1;
}

// _close - 关闭文件
int _close(int fd) {
    return 0;
}

// _fstat - 获取文件状态
int _fstat(int fd, struct stat *st) {
    st->st_mode = S_IFCHR;
    return 0;
}

// _isatty - 检查是否是终端
int _isatty(int fd) {
    return 1;
}

// _lseek - 文件定位
_off_t _lseek(int fd, _off_t offset, int whence) {
    return 0;
}

// _gettimeofday - 获取时间 (可选)
int _gettimeofday(struct timeval *tv, void *tz) {
    uint64_t cycles = read_csr(cycle);
    tv->tv_sec = cycles / CPU_FREQ;
    tv->tv_usec = (cycles % CPU_FREQ) * 1000000 / CPU_FREQ;
    return 0;
}

// _kill - 发送信号
int _kill(int pid, int sig) {
    errno = EINVAL;
    return -1;
}

// _getpid - 获取进程ID
int _getpid(void) {
    return 1;
}
