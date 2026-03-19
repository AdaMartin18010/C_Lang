/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\01_Game_Semantics\02_C11_Memory_Model.md
 * Line: 342
 * Language: c
 * Block ID: d38b9b3c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Happens-Before 图追踪器
#include <stdint.h>
#include <stdbool.h>

#define MAX_THREADS 64
#define MAX_EVENTS 1000000

typedef enum {
    EVENT_READ,
    EVENT_WRITE,
    EVENT_ACQUIRE,
    EVENT_RELEASE,
    EVENT_FORK,
    EVENT_JOIN
} EventType;

typedef struct {
    uint64_t timestamp;
    EventType type;
    void *address;
    int thread_id;
    uint64_t vc[MAX_THREADS];  // Vector Clock
} Event;

// Vector Clock 结构
typedef struct {
    uint64_t clocks[MAX_THREADS];
} VectorClock;

void vc_update(VectorClock *vc, int tid, uint64_t value) {
    if (vc->clocks[tid] < value) {
        vc->clocks[tid] = value;
    }
}

void vc_merge(VectorClock *dst, const VectorClock *src) {
    for (int i = 0; i < MAX_THREADS; i++) {
        if (dst->clocks[i] < src->clocks[i]) {
            dst->clocks[i] = src->clocks[i];
        }
    }
}

bool vc_leq(const VectorClock *a, const VectorClock *b) {
    for (int i = 0; i < MAX_THREADS; i++) {
        if (a->clocks[i] > b->clocks[i]) {
            return false;
        }
    }
    return true;
}

// 检测两个事件是否有happens-before关系
bool happens_before(const Event *e1, const Event *e2) {
    return vc_leq(&e1->vc, &e2->vc);
}

// 数据竞争检测
bool is_data_race(const Event *e1, const Event *e2) {
    // 检查是否是同一地址
    if (e1->address != e2->address) return false;

    // 检查是否至少一个是写
    bool has_write = (e1->type == EVENT_WRITE) || (e2->type == EVENT_WRITE);
    if (!has_write) return false;

    // 检查不同线程
    if (e1->thread_id == e2->thread_id) return false;

    // 检查是否有happens-before关系
    if (happens_before(e1, e2) || happens_before(e2, e1)) {
        return false;
    }

    return true;  // 发现数据竞争！
}
