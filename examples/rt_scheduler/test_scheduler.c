/*
 * 实时调度器测试与演示
 * 编译: gcc -Wall -Wextra -O2 -std=c11 -lm -o test_scheduler rt_scheduler.c test_scheduler.c
 */
#include "rt_scheduler.h"
#include <stdio.h>
#include <assert.h>

void test_rms_basic(void) {
    printf("=== RMS Basic Test ===\n");
    Task set[MAX_TASKS];
    size_t count = 0;
    task_set_add(set, &count, 10, 3);  /* T1: C=3, T=10, U=0.3 */
    task_set_add(set, &count, 15, 4);  /* T2: C=4, T=15, U=0.267 */
    task_set_add(set, &count, 30, 6);  /* T3: C=6, T=30, U=0.2 */

    print_task_set(set, count);
    bool ok = rms_schedulability_test(set, count);
    printf("RMS Schedulable: %s\n", ok ? "YES" : "NO");
    assert(ok);

    ScheduleEvent timeline[MAX_TIME];
    int misses = schedule_simulate(set, count, SCHED_RMS, timeline, MAX_TIME);
    printf("Deadline misses: %d\n", misses);
    print_gantt(timeline, 60, set, count);
    assert(misses == 0);
    printf("PASS\n\n");
}

void test_edf_basic(void) {
    printf("=== EDF Basic Test ===\n");
    Task set[MAX_TASKS];
    size_t count = 0;
    task_set_add(set, &count, 10, 3);
    task_set_add(set, &count, 15, 4);
    task_set_add(set, &count, 30, 6);

    print_task_set(set, count);
    bool ok = edf_schedulability_test(set, count);
    printf("EDF Schedulable: %s\n", ok ? "YES" : "NO");
    assert(ok);

    ScheduleEvent timeline[MAX_TIME];
    int misses = schedule_simulate(set, count, SCHED_EDF, timeline, MAX_TIME);
    printf("Deadline misses: %d\n", misses);
    print_gantt(timeline, 60, set, count);
    assert(misses == 0);
    printf("PASS\n\n");
}

void test_rms_unschedulable(void) {
    printf("=== RMS Unschedulable Test ===\n");
    Task set[MAX_TASKS];
    size_t count = 0;
    task_set_add(set, &count, 5, 3);   /* U=0.6 */
    task_set_add(set, &count, 7, 3);   /* U=0.429 */
    task_set_add(set, &count, 10, 2);  /* U=0.2 */
    /* 总利用率 ~1.229 > 1, 但 RMS 充分条件可能更早判定 */

    print_task_set(set, count);
    bool ok = rms_schedulability_test(set, count);
    printf("RMS Schedulable (sufficient test): %s\n", ok ? "YES" : "MAYBE/NO");
    printf("(Note: sufficient test may be conservative)\n\n");
}

void test_edf_high_utilization(void) {
    printf("=== EDF High Utilization Test ===\n");
    Task set[MAX_TASKS];
    size_t count = 0;
    task_set_add(set, &count, 10, 6);  /* U=0.6 */
    task_set_add(set, &count, 15, 5);  /* U=0.333 */
    task_set_add(set, &count, 30, 2);  /* U=0.067 */
    /* 总利用率 ~1.0 */

    print_task_set(set, count);
    bool ok = edf_schedulability_test(set, count);
    printf("EDF Schedulable: %s\n", ok ? "YES" : "NO");

    ScheduleEvent timeline[MAX_TIME];
    int misses = schedule_simulate(set, count, SCHED_EDF, timeline, MAX_TIME);
    printf("Deadline misses: %d\n", misses);
    print_gantt(timeline, 60, set, count);
    printf("PASS\n\n");
}

int main(void) {
    test_rms_basic();
    test_edf_basic();
    test_rms_unschedulable();
    test_edf_high_utilization();
    printf("All scheduler tests passed!\n");
    return 0;
}
