/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\08_Lab8_Lock.md
 * Line: 693
 * Language: c
 * Block ID: aaac5bc1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#ifdef DEBUG_LOCK

#define LOCK_TRACE(lk, op) \
    printf("[%s] CPU%d %s %s at %s:%d\n", \
           ticks ? "K" : "B", cpuid(), op, (lk)->name, __FILE__, __LINE__)

#define ACQUIRE(lk) do { \
    LOCK_TRACE(lk, "acquiring"); \
    acquire(lk); \
    LOCK_TRACE(lk, "acquired"); \
} while(0)

#define RELEASE(lk) do { \
    LOCK_TRACE(lk, "releasing"); \
    release(lk); \
    LOCK_TRACE(lk, "released"); \
} while(0)

#else
#define ACQUIRE(lk) acquire(lk)
#define RELEASE(lk) release(lk)
#endif
