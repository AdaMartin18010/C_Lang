/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\09_Interrupt_Hardware\02_System_Call_Hardware.md
 * Line: 1295
 * Language: c
 * Block ID: 398c409c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * glibc系统调用封装层
 * sysdeps/unix/sysv/linux/x86_64/syscall.S
 */

/* 通用系统调用封装 */
long syscall(long sysno, ...)
{
    // 变参函数，根据sysno调用不同的系统调用
    // 实际实现为汇编
}

/* 具体系统调用的内联汇编实现 */

// 3参数系统调用示例 (open)
static __always_inline _syscall3(long, open,
                                 const char *, pathname,
                                 int, flags,
                                 mode_t, mode)
{
    long ret;
    __asm__ volatile (
        "syscall"
        : "=a" (ret)
        : "a" (__NR_open), "D" (pathname), "S" (flags), "d" (mode)
        : "rcx", "r11", "memory"
    );

    // 处理错误 (Linux返回负值作为错误码)
    if (ret < 0) {
        errno = -ret;
        ret = -1;
    }
    return ret;
}

// 6参数系统调用 (mmap)
static __always_inline _syscall6(void *, mmap,
                                 void *, addr,
                                 size_t, length,
                                 int, prot,
                                 int, flags,
                                 int, fd,
                                 off_t, offset)
{
    void *ret;
    register long r10 __asm__("r10") = flags;
    register long r8  __asm__("r8")  = fd;
    register long r9  __asm__("r9")  = offset;

    __asm__ volatile (
        "syscall"
        : "=a" (ret)
        : "a" (__NR_mmap), "D" (addr), "S" (length),
          "d" (prot), "r" (r10), "r" (r8), "r" (r9)
        : "rcx", "r11", "memory"
    );

    return ret;
}

// 错误处理宏
#define INTERNAL_SYSCALL(name, err, nr, args...) \
    internal_syscall##nr(__NR_##name, err, args)

// 包装为POSIX接口
int open(const char *pathname, int flags, ...)
{
    mode_t mode = 0;

    if (__OPEN_NEEDS_MODE(flags)) {
        va_list arg;
        va_start(arg, flags);
        mode = va_arg(arg, mode_t);
        va_end(arg);
    }

    return INLINE_SYSCALL(open, 3, pathname, flags, mode);
}

ssize_t read(int fd, void *buf, size_t count)
{
    return INLINE_SYSCALL(read, 3, fd, buf, count);
}

ssize_t write(int fd, const void *buf, size_t count)
{
    return INLINE_SYSCALL(write, 3, fd, buf, count);
}
