/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\08_Lab8_Lock.md
 * Line: 608
 * Language: c
 * Block ID: 0e563ae0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/lockstat.h

struct lock_stat {
    const char *name;
    uint64 acquires;
    uint64 misses;      // 获取失败次数
    uint64 spins;       // 自旋次数
    uint64 sleep;       // 睡眠次数 (对于睡眠锁)
    uint64 max_hold_time;  // 最大持有时间
    uint64 total_hold_time;
};

#define MAX_LOCK_STATS 100
extern struct lock_stat lock_stats[MAX_LOCK_STATS];
extern int lock_stat_count;

void lockstat_init(struct spinlock *lk, const char *name);
void lockstat_acquired(struct spinlock *lk);
void lockstat_released(struct spinlock *lk);
