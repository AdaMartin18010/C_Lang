/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\16_Performance_Analysis_Hardware\01_PMU_Architecture.md
 * Line: 1335
 * Language: c
 * Block ID: 7e12b84e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* pmu_direct.c - 直接通过MSR/指令访问PMU (仅限内核或特权模式) */

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

/* ==================== x86 PMU直接访问 ==================== */

#if defined(__x86_64__) || defined(__i386__)

/* 读取时间戳计数器 (RDTSC) */
static inline uint64_t rdtsc(void) {
    uint32_t lo, hi;
    __asm__ __volatile__ (
        "rdtsc"
        : "=a" (lo), "=d" (hi)
    );
    return ((uint64_t)hi << 32) | lo;
}

/* 序列化RDTSC (RDTSCP) - 更精确 */
static inline uint64_t rdtscp(uint32_t *aux) {
    uint32_t lo, hi;
    __asm__ __volatile__ (
        "rdtscp\n"
        "shl $32, %%rdx\n"
        "or %%rdx, %0\n"
        "lfence"
        : "=a" (lo), "=d" (hi), "=c" (*aux)
        ::
    );
    return ((uint64_t)hi << 32) | lo;
}

/* CPUID序列化指令 */
static inline void cpuid(uint32_t func, uint32_t *eax, uint32_t *ebx,
                         uint32_t *ecx, uint32_t *edx) {
    __asm__ __volatile__ (
        "cpuid"
        : "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx)
        : "a" (func)
    );
}

/* 内存屏障 */
static inline void mfence(void) {
    __asm__ __volatile__ ("mfence" ::: "memory");
}

/* 序列化指令 */
static inline void serialize(void) {
    uint32_t eax, ebx, ecx, edx;
    cpuid(0, &eax, &ebx, &ecx, &edx);
}

/* 读取MSR (需要ring 0权限) */
static inline uint64_t rdmsr(uint32_t msr) {
    uint32_t low, high;
    __asm__ __volatile__ (
        "rdmsr"
        : "=a" (low), "=d" (high)
        : "c" (msr)
    );
    return ((uint64_t)high << 32) | low;
}

/* 写入MSR (需要ring 0权限) */
static inline void wrmsr(uint32_t msr, uint64_t value) {
    uint32_t low = value & 0xFFFFFFFF;
    uint32_t high = value >> 32;
    __asm__ __volatile__ (
        "wrmsr"
        :: "c" (msr), "a" (low), "d" (high)
    );
}

/* 读取性能监控计数器 (RDPMC) */
static inline uint64_t rdpmc(uint32_t counter) {
    uint32_t low, high;
    __asm__ __volatile__ (
        "rdpmc"
        : "=a" (low), "=d" (high)
        : "c" (counter)
    );
    return ((uint64_t)high << 32) | low;
}

/* x86 PMU MSR地址定义 */
#define IA32_PERFEVTSEL0    0x186
#define IA32_PERFEVTSEL1    0x187
#define IA32_PERFEVTSEL2    0x188
#define IA32_PERFEVTSEL3    0x189
#define IA32_PMC0           0xC1
#define IA32_PMC1           0xC2
#define IA32_PMC2           0xC3
#define IA32_PMC3           0xC4
#define IA32_FIXED_CTR0     0x309  /* INST_RETIRED.ANY */
#define IA32_FIXED_CTR1     0x30A  /* CPU_CLK_UNHALTED.CORE */
#define IA32_FIXED_CTR2     0x30B  /* CPU_CLK_UNHALTED.REF */
#define IA32_PERF_GLOBAL_STATUS     0x38E
#define IA32_PERF_GLOBAL_CTRL       0x38F
#define IA32_PERF_GLOBAL_OVF_CTRL   0x390
#define IA32_FIXED_CTR_CTRL         0x38D

/* 配置并启动x86 PMU计数器 */
void x86_pmu_setup(void) {
    uint64_t global_ctrl;

    /* 禁用所有计数器 */
    wrmsr(IA32_PERF_GLOBAL_CTRL, 0);

    /* 配置固定计数器控制 */
    /* 使能3个固定计数器，所有特权级别 */
    wrmsr(IA32_FIXED_CTR_CTRL, 0x222);  /* 每个计数器: OS=1, USR=1, ANY=0 */

    /* 清零固定计数器 */
    wrmsr(IA32_FIXED_CTR0, 0);
    wrmsr(IA32_FIXED_CTR1, 0);
    wrmsr(IA32_FIXED_CTR2, 0);

    /* 配置通用计数器0: L1缓存未命中 */
    /* Event: 0x2E, UMask: 0x4F (LLC Reference) */
    uint64_t evt_sel = 0x2E | (0x4F << 8) | (1ULL << 16) | (1ULL << 17) | (1ULL << 22);
    wrmsr(IA32_PERFEVTSEL0, 0);  /* 先禁用 */
    wrmsr(IA32_PMC0, 0);          /* 清零 */
    wrmsr(IA32_PERFEVTSEL0, evt_sel);

    /* 配置通用计数器1: 分支预测失败 */
    /* Event: 0xC5, UMask: 0x00 (BR_MISP_RETIRED.ALL_BRANCHES) */
    evt_sel = 0xC5 | (0x00 << 8) | (1ULL << 16) | (1ULL << 17) | (1ULL << 22);
    wrmsr(IA32_PERFEVTSEL1, 0);
    wrmsr(IA32_PMC1, 0);
    wrmsr(IA32_PERFEVTSEL1, evt_sel);

    /* 全局使能计数器 */
    global_ctrl = (1ULL << 32) | (1ULL << 33) | (1ULL << 34) |  /* 固定计数器 */
                  (1ULL << 0) | (1ULL << 1);                     /* 通用计数器 */
    wrmsr(IA32_PERF_GLOBAL_CTRL, global_ctrl);

    printf("x86 PMU配置完成\n");
}

/* 读取x86 PMU计数器 */
void x86_pmu_read(uint64_t *fixed, uint64_t *pmc0, uint64_t *pmc1) {
    *fixed = rdmsr(IA32_FIXED_CTR0);
    *pmc0 = rdmsr(IA32_PMC0);
    *pmc1 = rdmsr(IA32_PMC1);
}

/* 停止x86 PMU */
void x86_pmu_stop(void) {
    wrmsr(IA32_PERF_GLOBAL_CTRL, 0);
    wrmsr(IA32_PERFEVTSEL0, 0);
    wrmsr(IA32_PERFEVTSEL1, 0);
}

#endif  /* __x86_64__ || __i386__ */

/* ==================== ARM PMU直接访问 ==================== */

#if defined(__aarch64__)

/* 读取PMCR_EL0 */
static inline uint64_t read_pmcr(void) {
    uint64_t val;
    __asm__ __volatile__ ("mrs %0, pmcr_el0" : "=r" (val));
    return val;
}

/* 写入PMCR_EL0 */
static inline void write_pmcr(uint64_t val) {
    __asm__ __volatile__ ("msr pmcr_el0, %0" :: "r" (val));
}

/* 读取PMCNTENSET_EL0 */
static inline uint64_t read_pmcntenset(void) {
    uint64_t val;
    __asm__ __volatile__ ("mrs %0, pmcntenset_el0" : "=r" (val));
    return val;
}

/* 写入PMCNTENSET_EL0 */
static inline void write_pmcntenset(uint64_t val) {
    __asm__ __volatile__ ("msr pmcntenset_el0, %0" :: "r" (val));
}

/* 读取PMCNTENCLR_EL0 */
static inline void write_pmcntenclr(uint64_t val) {
    __asm__ __volatile__ ("msr pmcntenclr_el0, %0" :: "r" (val));
}

/* 读取PMSELR_EL0 */
static inline void write_pmselr(uint64_t val) {
    __asm__ __volatile__ ("msr pmselr_el0, %0" :: "r" (val));
}

/* 读取PMXEVTYPER_EL0 */
static inline void write_pmxevtyper(uint64_t val) {
    __asm__ __volatile__ ("msr pmxevtyper_el0, %0" :: "r" (val));
}

/* 读取PMXEVCNTR_EL0 */
static inline uint64_t read_pmxevcntr(void) {
    uint64_t val;
    __asm__ __volatile__ ("mrs %0, pmxevcntr_el0" : "=r" (val));
    return val;
}

/* 写入PMXEVCNTR_EL0 */
static inline void write_pmxevcntr(uint64_t val) {
    __asm__ __volatile__ ("msr pmxevcntr_el0, %0" :: "r" (val));
}

/* 读取PMCCNTR_EL0 */
static inline uint64_t read_pmccntr(void) {
    uint64_t val;
    __asm__ __volatile__ ("mrs %0, pmccntr_el0" : "=r" (val));
    return val;
}

/* 写入PMCCNTR_EL0 */
static inline void write_pmccntr(uint64_t val) {
    __asm__ __volatile__ ("msr pmccntr_el0, %0" :: "r" (val));
}

/* 读取PMUSERENR_EL0 */
static inline uint64_t read_pmuserenr(void) {
    uint64_t val;
    __asm__ __volatile__ ("mrs %0, pmuserenr_el0" : "=r" (val));
    return val;
}

/* 数据同步屏障 */
static inline void dsb(void) {
    __asm__ __volatile__ ("dsb sy" ::: "memory");
}

/* 指令同步屏障 */
static inline void isb(void) {
    __asm__ __volatile__ ("isb" ::: "memory");
}

/* ARM事件定义 */
#define ARMV8_PMUV3_EVENT_INST_RETIRED  0x08
#define ARMV8_PMUV3_EVENT_CPU_CYCLES    0x11
#define ARMV8_PMUV3_EVENT_BR_MIS_PRED   0x10
#define ARMV8_PMUV3_EVENT_L1D_CACHE_REFILL 0x03
#define ARMV8_PMUV3_EVENT_L2D_CACHE_REFILL 0x17
#define ARMV8_PMUV3_EVENT_STALL_FRONTEND 0x23
#define ARMV8_PMUV3_EVENT_STALL_BACKEND 0x24

/* 配置ARM PMU */
void arm_pmu_setup(void) {
    uint64_t pmcr;

    /* 禁用PMU */
    write_pmcr(0);
    dsb();
    isb();

    /* 复位所有计数器 */
    pmcr = (1ULL << 1) |  /* 复位事件计数器 */
           (1ULL << 2);   /* 复位周期计数器 */
    write_pmcr(pmcr);
    dsb();
    isb();

    /* 配置计数器0: 退役指令 */
    write_pmselr(0);
    write_pmxevtyper(ARMV8_PMUV3_EVENT_INST_RETIRED);
    write_pmxevcntr(0);

    /* 配置计数器1: 分支预测失败 */
    write_pmselr(1);
    write_pmxevtyper(ARMV8_PMUV3_EVENT_BR_MIS_PRED);
    write_pmxevcntr(0);

    /* 配置计数器2: L1D缓存重填 */
    write_pmselr(2);
    write_pmxevtyper(ARMV8_PMUV3_EVENT_L1D_CACHE_REFILL);
    write_pmxevcntr(0);

    /* 清零周期计数器 */
    write_pmccntr(0);

    /* 使能计数器: cycle + counter 0,1,2 */
    write_pmcntenset((1ULL << 31) | 0x7);

    /* 使能PMU */
    pmcr = (1ULL << 0) |   /* E: PMU使能 */
           (1ULL << 3) |   /* D: 周期计数器64分频 */
           (0x7 << 11);    /* N: 事件计数器数量-1 */
    write_pmcr(pmcr);
    dsb();
    isb();

    printf("ARM PMU配置完成\n");
}

/* 读取ARM PMU计数器 */
void arm_pmu_read(uint64_t *cycle, uint64_t *cnt0, uint64_t *cnt1, uint64_t *cnt2) {
    *cycle = read_pmccntr();

    write_pmselr(0);
    *cnt0 = read_pmxevcntr();

    write_pmselr(1);
    *cnt1 = read_pmxevcntr();

    write_pmselr(2);
    *cnt2 = read_pmxevcntr();
}

/* 停止ARM PMU */
void arm_pmu_stop(void) {
    write_pmcr(0);
    write_pmcntenclr(0xFFFFFFFF);
}

#endif  /* __aarch64__ */

/* ==================== RISC-V PMU直接访问 ==================== */

#if defined(__riscv)

/* 读取cycle CSR */
static inline uint64_t read_csr_cycle(void) {
    uint64_t val;
    __asm__ __volatile__ ("rdcycle %0" : "=r" (val));
    return val;
}

/* 读取instret CSR */
static inline uint64_t read_csr_instret(void) {
    uint64_t val;
    __asm__ __volatile__ ("rdinstret %0" : "=r" (val));
    return val;
}

/* 读取mcycle CSR (机器模式) */
static inline uint64_t read_csr_mcycle(void) {
    uint64_t val;
    __asm__ __volatile__ ("csrr %0, mcycle" : "=r" (val));
    return val;
}

/* 读取minstret CSR (机器模式) */
static inline uint64_t read_csr_minstret(void) {
    uint64_t val;
    __asm__ __volatile__ ("csrr %0, minstret" : "=r" (val));
    return val;
}

/* 读取mcountinhibit CSR */
static inline uint64_t read_csr_mcountinhibit(void) {
    uint64_t val;
    __asm__ __volatile__ ("csrr %0, mcountinhibit" : "=r" (val));
    return val;
}

/* 写入mcountinhibit CSR */
static inline void write_csr_mcountinhibit(uint64_t val) {
    __asm__ __volatile__ ("csrw mcountinhibit, %0" :: "r" (val));
}

/* 读取mhpmcounter3 CSR */
static inline uint64_t read_csr_mhpmcounter3(void) {
    uint64_t val;
    __asm__ __volatile__ ("csrr %0, mhpmcounter3" : "=r" (val));
    return val;
}

/* 写入mhpmcounter3 CSR */
static inline void write_csr_mhpmcounter3(uint64_t val) {
    __asm__ __volatile__ ("csrw mhpmcounter3, %0" :: "r" (val));
}

/* 读取mhpmevent3 CSR */
static inline uint64_t read_csr_mhpmevent3(void) {
    uint64_t val;
    __asm__ __volatile__ ("csrr %0, mhpmevent3" : "=r" (val));
    return val;
}

/* 写入mhpmevent3 CSR */
static inline void write_csr_mhpmevent3(uint64_t val) {
    __asm__ __volatile__ ("csrw mhpmevent3, %0" :: "r" (val));
}

/* 内存屏障 */
static inline void fence(void) {
    __asm__ __volatile__ ("fence" ::: "memory");
}

/* RISC-V Sscofpmf事件 */
#define RISCV_EVENT_L1D_CACHE_ACCESS    0x0102
#define RISCV_EVENT_L1D_CACHE_MISS      0x0103
#define RISCV_EVENT_BRANCH              0x0005
#define RISCV_EVENT_BRANCH_MISPRED      0x0006

/* 配置RISC-V PMU */
void riscv_pmu_setup(void) {
    /* 禁止计数器 */
    write_csr_mcountinhibit(0xFFFFFFFF);
    fence();

    /* 清零mhpmcounter3 */
    write_csr_mhpmcounter3(0);

    /* 配置mhpmevent3: L1D缓存访问 */
    /* 事件编码 + 用户/监督/机器模式使能 */
    uint64_t event = RISCV_EVENT_L1D_CACHE_ACCESS |
                     (1ULL << 16) |  /* U */
                     (1ULL << 17) |  /* S */
                     (1ULL << 18);   /* M */
    write_csr_mhpmevent3(event);
    fence();

    /* 使能计数器: cycle + instret + hpmcounter3 */
    write_csr_mcountinhibit(~(0x1 | 0x2 | 0x8));

    printf("RISC-V PMU配置完成\n");
}

/* 读取RISC-V PMU计数器 */
void riscv_pmu_read(uint64_t *cycle, uint64_t *instret, uint64_t *hpm3) {
    *cycle = read_csr_mcycle();
    *instret = read_csr_minstret();
    *hpm3 = read_csr_mhpmcounter3();
}

/* 停止RISC-V PMU */
void riscv_pmu_stop(void) {
    write_csr_mcountinhibit(0xFFFFFFFF);
}

#endif  /* __riscv */

/* ==================== 通用测试代码 ==================== */

/* 测试工作负载 */
volatile int global_sum = 0;

void test_workload(void) {
    volatile int sum = 0;
    int arr[1000];

    /* 初始化数组 */
    for (int i = 0; i < 1000; i++) {
        arr[i] = i;
    }

    /* 计算密集型循环 */
    for (int iter = 0; iter < 10000; iter++) {
        for (int i = 0; i < 1000; i++) {
            if (arr[i] % 2 == 0) {
                sum += arr[i];
            } else {
                sum -= arr[i];
            }
        }
    }

    global_sum = sum;
}

int main(void) {
    printf("\n========== PMU直接访问测试 ==========\n\n");

#if defined(__x86_64__) || defined(__i386__)
    printf("架构: x86\n");

    /* 注意: 直接MSR访问需要内核模块或root权限 */
    printf("注意: x86直接MSR访问需要内核态权限\n");
    printf("这里仅演示rdtsc指令的使用:\n\n");

    uint64_t tsc_start, tsc_end;
    uint32_t aux;

    /* RDTSC测试 */
    serialize();
    tsc_start = rdtscp(&aux);

    test_workload();

    tsc_end = rdtscp(&aux);
    serialize();

    printf("RDTSC测量结果:\n");
    printf("  起始TSC: %" PRIu64 "\n", tsc_start);
    printf("  结束TSC: %" PRIu64 "\n", tsc_end);
    printf("  差值: %" PRIu64 " cycles\n", tsc_end - tsc_start);

#elif defined(__aarch64__)
    printf("架构: ARM64\n");

    /* 检查用户态PMU访问权限 */
    uint64_t userenr = read_pmuserenr();
    printf("PMUSERENR_EL0: 0x%" PRIx64 "\n", userenr);

    if (!(userenr & 0x1)) {
        printf("警告: 用户态PMU访问未启用\n");
        printf("请在内核中启用CONFIG_PERF_EVENTS或设置PMU用户访问权限\n");
    } else {
        /* 配置PMU */
        arm_pmu_setup();

        uint64_t cycle_start, cycle_end;
        uint64_t cnt0_start, cnt0_end;
        uint64_t cnt1_start, cnt1_end;
        uint64_t cnt2_start, cnt2_end;

        /* 读取初始值 */
        arm_pmu_read(&cycle_start, &cnt0_start, &cnt1_start, &cnt2_start);

        /* 执行测试 */
        test_workload();

        /* 读取结束值 */
        arm_pmu_read(&cycle_end, &cnt0_end, &cnt1_end, &cnt2_end);

        /* 输出结果 */
        printf("\nPMU测量结果:\n");
        printf("  周期计数: %" PRIu64 "\n", cycle_end - cycle_start);
        printf("  退役指令: %" PRIu64 "\n", cnt0_end - cnt0_start);
        printf("  分支预测失败: %" PRIu64 "\n", cnt1_end - cnt1_start);
        printf("  L1D缓存重填: %" PRIu64 "\n", cnt2_end - cnt2_start);

        if (cnt0_end > cnt0_start) {
            printf("  CPI: %.3f\n",
                   (double)(cycle_end - cycle_start) / (cnt0_end - cnt0_start));
        }

        arm_pmu_stop();
    }

#elif defined(__riscv)
    printf("架构: RISC-V\n");

    /* 配置PMU */
    riscv_pmu_setup();

    uint64_t cycle_start, cycle_end;
    uint64_t instret_start, instret_end;
    uint64_t hpm3_start, hpm3_end;

    /* 读取初始值 */
    riscv_pmu_read(&cycle_start, &instret_start, &hpm3_start);
    fence();

    /* 执行测试 */
    test_workload();

    fence();
    /* 读取结束值 */
    riscv_pmu_read(&cycle_end, &instret_end, &hpm3_end);

    /* 输出结果 */
    printf("\nPMU测量结果:\n");
    printf("  机器周期: %" PRIu64 "\n", cycle_end - cycle_start);
    printf("  退役指令: %" PRIu64 "\n", instret_end - instret_start);
    printf("  HPM3 (L1D访问): %" PRIu64 "\n", hpm3_end - hpm3_start);

    if (instret_end > instret_start) {
        printf("  CPI: %.3f\n",
               (double)(cycle_end - cycle_start) / (instret_end - instret_start));
    }

    riscv_pmu_stop();

#else
    printf("架构: 未知 (不支持直接PMU访问)\n");
#endif

    printf("\n========== 测试完成 ==========\n");
    return 0;
}
