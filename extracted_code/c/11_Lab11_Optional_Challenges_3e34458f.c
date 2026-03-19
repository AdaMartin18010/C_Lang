/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\11_Lab11_Optional_Challenges.md
 * Line: 148
 * Language: c
 * Block ID: 3e34458f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 简单的性能计数器
struct perf_counter {
    uint64_t syscall_count;
    uint64_t context_switches;
    uint64_t page_faults;
    uint64_t disk_reads;
    uint64_t disk_writes;
};

// 打印统计
void print_perf_stats() {
    printf("System Statistics:\n");
    printf("  Syscalls: %lu\n", perf.syscall_count);
    printf("  Context switches: %lu\n", perf.context_switches);
    printf("  Page faults: %lu\n", perf.page_faults);
    printf("  Disk I/O: %lu reads, %lu writes\n",
           perf.disk_reads, perf.disk_writes);
}
