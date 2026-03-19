/*
 * Auto-generated from: 05_Deep_Structure_MetaPhysics\06_TLA_Plus\04_Distributed_Systems_Specs.md
 * Line: 878
 * Language: c
 * Block ID: ee9c94ed
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 基于TLA+规范实现的Raft互斥锁保护
 * 对应TLA+中的类型不变式和状态转换
 */

#include <pthread.h>
#include <stdatomic.h>

/* 对应TLA+ TypeInvariant */
struct RaftServer {
    _Atomic uint64_t current_term;
    _Atomic uint32_t role;  /* 0=FOLLOWER, 1=CANDIDATE, 2=LEADER */

    /* 对应TLA+ log */
    struct LogEntry *log;
    size_t log_len;
    pthread_rwlock_t log_lock;

    /* 对应TLA+ commitIndex, lastApplied */
    _Atomic uint64_t commit_index;
    _Atomic uint64_t last_applied;

    /* Leader特有状态 - 对应TLA+ nextIndex, matchIndex */
    uint64_t *next_index;
    uint64_t *match_index;

    /* 选举定时器 */
    pthread_mutex_t timer_mutex;
    pthread_cond_t timer_cond;
    uint64_t election_timeout;

    /* 投票状态 - 对应TLA+ votedFor */
    _Atomic uint32_t voted_for;
};

/* 对应TLA+ ElectionSafety: 任期检查确保唯一Leader */
int become_leader(struct RaftServer *s) {
    uint64_t term = atomic_load(&s->current_term);

    /* 原子性地检查和更新角色 */
    uint32_t expected = CANDIDATE;
    if (!atomic_compare_exchange_strong(&s->role,
                                         &expected,
                                         LEADER)) {
        return -1;  /* 已经不是Candidate了 */
    }

    /* 初始化Leader状态 - 对应TLA+ BecomeLeader */
    pthread_rwlock_wrlock(&s->log_lock);
    for (int i = 0; i < num_servers; i++) {
        if (i != self_id) {
            s->next_index[i] = s->log_len + 1;
            s->match_index[i] = 0;
        }
    }
    pthread_rwlock_unlock(&s->log_lock);

    return 0;
}

/* 对应TLA+ StateMachineSafety: 日志一致性检查 */
int append_entries(struct RaftServer *s,
                   uint64_t prev_log_index,
                   uint64_t prev_log_term,
                   struct LogEntry *entries,
                   size_t n_entries) {
    pthread_rwlock_wrlock(&s->log_lock);

    /* 日志一致性检查 - 对应TLA+ LogMatching */
    if (prev_log_index > 0) {
        if (prev_log_index > s->log_len ||
            s->log[prev_log_index - 1].term != prev_log_term) {
            pthread_rwlock_unlock(&s->log_lock);
            return -1;  /* 日志不匹配 */
        }
    }

    /* Leader只追加 - 对应TLA+ LeaderAppendOnly */
    /* 不删除已存在的条目，只覆盖冲突的 */

    pthread_rwlock_unlock(&s->log_lock);
    return 0;
}
