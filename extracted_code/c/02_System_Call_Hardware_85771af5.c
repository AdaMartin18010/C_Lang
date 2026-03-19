/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\09_Interrupt_Hardware\02_System_Call_Hardware.md
 * Line: 1230
 * Language: c
 * Block ID: 85771af5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 用户程序使用vDSO
 */

#include <sys/time.h>
#include <time.h>

// 方法1: 通过glibc透明使用
void example_glibc(void)
{
    struct timeval tv;
    struct timezone tz;

    // glibc自动使用vDSO (如果可用)
    gettimeofday(&tv, &tz);  // 可能只花费 ~20 cycles

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);  // vDSO优化
}

// 方法2: 直接调用vDSO (高级)
#include <sys/auxv.h>
#include <asm/vsyscall.h>

// 获取vDSO基址
void *get_vdso_base(void)
{
    return (void *)getauxval(AT_SYSINFO_EHDR);
}

// 解析ELF找到vDSO符号
void *vdso_sym(const char *name);

void example_direct_vdso(void)
{
    // 查找__vdso_gettimeofday符号
    int (*vdso_gettimeofday)(struct timeval *, struct timezone *);

    vdso_gettimeofday = vdso_sym("__vdso_gettimeofday");

    struct timeval tv;
    vdso_gettimeofday(&tv, NULL);  // 直接调用，无系统调用开销
}

// 方法3: 使用syscall (传统慢速方式)
#include <unistd.h>
#include <sys/syscall.h>

void example_syscall(void)
{
    struct timeval tv;
    struct timezone tz;

    // 显式系统调用 - 慢!
    syscall(SYS_gettimeofday, &tv, &tz);  // ~100-200 cycles
}
