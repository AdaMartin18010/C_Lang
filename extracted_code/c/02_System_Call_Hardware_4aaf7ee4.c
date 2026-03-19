/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\09_Interrupt_Hardware\02_System_Call_Hardware.md
 * Line: 1455
 * Language: c
 * Block ID: 4aaf7ee4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * musl libc系统调用实现 (更轻量级)
 * src/internal/x86_64/syscall.s
 */

// musl使用单个汇编入口点处理所有系统调用
.global __syscall
.type __syscall,@function
__syscall:
    movq %rdi,%rax      /* 系统调用号 */
    movq %rsi,%rdi      /* 参数1 */
    movq %rdx,%rsi      /* 参数2 */
    movq %rcx,%rdx      /* 参数3 */
    movq %r8,%r10       /* 参数4 (注意使用r10) */
    movq %r9,%r8        /* 参数5 */
    movq 8(%rsp),%r9    /* 参数6 (从栈获取) */
    syscall
    ret

// C端包装器
static __inline long __syscall0(long n)
{
    unsigned long ret;
    __asm__ __volatile__ ("syscall" : "=a"(ret) : "a"(n) : "rcx","r11","memory");
    return ret;
}

static __inline long __syscall1(long n, long a1)
{
    unsigned long ret;
    __asm__ __volatile__ ("syscall" : "=a"(ret) : "a"(n), "D"(a1) : "rcx","r11","memory");
    return ret;
}

// ... 类似定义直到 __syscall6

// 实际的系统调用函数 (以read为例)
ssize_t read(int fd, void *buf, size_t count)
{
    return syscall_cp(SYS_read, fd, buf, count);
}
