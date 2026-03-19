/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\16_Performance_Analysis_Hardware\04_Microarchitecture_Benchmarking.md
 * Line: 1669
 * Language: c
 * Block ID: 9237f16a
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* mypmu_user.c - PMU驱动用户空间程序 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdint.h>

/* IOCTL命令 */
#define MYPMU_IOC_MAGIC  'P'
#define MYPMU_IOC_CONFIG  _IOW(MYPMU_IOC_MAGIC, 0, uint64_t)
#define MYPMU_IOC_START   _IO(MYPMU_IOC_MAGIC, 1)
#define MYPMU_IOC_STOP    _IO(MYPMU_IOC_MAGIC, 2)
#define MYPMU_IOC_READ    _IOR(MYPMU_IOC_MAGIC, 3, uint64_t)
#define MYPMU_IOC_RESET   _IO(MYPMU_IOC_MAGIC, 4)

/* perf事件类型 */
#define PERF_COUNT_HW_CPU_CYCLES        0
#define PERF_COUNT_HW_INSTRUCTIONS      1
#define PERF_COUNT_HW_CACHE_REFERENCES  2
#define PERF_COUNT_HW_CACHE_MISSES      3
#define PERF_COUNT_HW_BRANCH_INSTRUCTIONS 4
#define PERF_COUNT_HW_BRANCH_MISSES     5

int main(int argc, char *argv[]) {
    int fd = open("/dev/mypmu", O_RDWR);
    if (fd < 0) {
        perror("open /dev/mypmu");
        printf("请确保驱动已加载: sudo insmod my_pmu_driver.ko\n");
        return 1;
    }

    printf("自定义PMU驱动测试程序\n");
    printf("=====================\n\n");

    /* 测试1: CPU周期计数 */
    printf("测试1: CPU周期计数\n");

    uint64_t config = PERF_COUNT_HW_CPU_CYCLES;
    if (ioctl(fd, MYPMU_IOC_CONFIG, &config) < 0) {
        perror("ioctl CONFIG");
        close(fd);
        return 1;
    }

    ioctl(fd, MYPMU_IOC_RESET);
    ioctl(fd, MYPMU_IOC_START);

    /* 执行一些工作 */
    volatile long sum = 0;
    for (int i = 0; i < 1000000; i++) {
        sum += i;
    }
    (void)sum;

    ioctl(fd, MYPMU_IOC_STOP);

    uint64_t cycles;
    if (ioctl(fd, MYPMU_IOC_READ, &cycles) < 0) {
        perror("ioctl READ");
    } else {
        printf("  周期数: %lu\n", cycles);
    }

    /* 测试2: 指令计数 */
    printf("\n测试2: 指令计数\n");

    config = PERF_COUNT_HW_INSTRUCTIONS;
    ioctl(fd, MYPMU_IOC_CONFIG, &config);
    ioctl(fd, MYPMU_IOC_RESET);
    ioctl(fd, MYPMU_IOC_START);

    /* 执行一些工作 */
    sum = 0;
    for (int i = 0; i < 1000000; i++) {
        sum += i;
    }

    ioctl(fd, MYPMU_IOC_STOP);

    uint64_t instructions;
    ioctl(fd, MYPMU_IOC_READ, &instructions);
    printf("  指令数: %lu\n", instructions);
    printf("  CPI: %.2f\n", (double)cycles / instructions);

    /* 测试3: 缓存未命中 */
    printf("\n测试3: 缓存未命中测试\n");

    config = PERF_COUNT_HW_CACHE_MISSES;
    ioctl(fd, MYPMU_IOC_CONFIG, &config);
    ioctl(fd, MYPMU_IOC_RESET);
    ioctl(fd, MYPMU_IOC_START);

    /* 随机访问大数组 */
    int size = 100 * 1024 * 1024;  /* 100MB */
    char *data = malloc(size);
    for (int i = 0; i < 100000; i++) {
        int idx = rand() % size;
        data[idx] = i;
    }
    free(data);

    ioctl(fd, MYPMU_IOC_STOP);

    uint64_t cache_misses;
    ioctl(fd, MYPMU_IOC_READ, &cache_misses);
    printf("  缓存未命中: %lu\n", cache_misses);

    close(fd);

    printf("\n测试完成!\n");
    return 0;
}
