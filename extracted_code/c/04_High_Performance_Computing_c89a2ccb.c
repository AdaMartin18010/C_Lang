/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\04_High_Performance_Computing.md
 * Line: 290
 * Language: c
 * Block ID: c89a2ccb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 使用性能计数器（Linux）
#include <linux/perf_event.h>
#include <linux/hw_breakpoint.h>
#include <sys/syscall.h>
#include <unistd.h>

struct perf_event_attr pe = {
    .type = PERF_TYPE_HARDWARE,
    .size = sizeof(struct perf_event_attr),
    .config = PERF_COUNT_HW_CACHE_MISSES,
    .disabled = 1,
    .exclude_kernel = 1,
    .exclude_hv = 1
};

int fd = syscall(__NR_perf_event_open, &pe, 0, -1, -1, 0);
ioctl(fd, PERF_EVENT_IOC_RESET, 0);
ioctl(fd, PERF_EVENT_IOC_ENABLE, 0);

// 被测代码
compute_kernel();

ioctl(fd, PERF_EVENT_IOC_DISABLE, 0);
long long count;
read(fd, &count, sizeof(count));
printf("Cache misses: %lld\n", count);
close(fd);
