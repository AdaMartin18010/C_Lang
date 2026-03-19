/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\09_MISRA_C_2023_Rules_9.md
 * Line: 369
 * Language: c
 * Block ID: f0ae5249
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 违反 - 死锁风险 */
void deadlock_risk(void)
{
    pthread_mutex_lock(&mutex_a);
    pthread_mutex_lock(&mutex_b);  /* 如果另一线程顺序相反，死锁！ */
    /* ... */
    pthread_mutex_unlock(&mutex_b);
    pthread_mutex_unlock(&mutex_a);
}

/* ✅ 合规 - 统一的锁顺序 */
#define LOCK_ORDER_A 1
#define LOCK_ORDER_B 2

void safe_lock_order(void)
{
    /* 始终按相同顺序获取 */
    pthread_mutex_lock(&mutex_a);  /* 顺序A */
    pthread_mutex_lock(&mutex_b);  /* 顺序B */
    /* ... */
    pthread_mutex_unlock(&mutex_b);
    pthread_mutex_unlock(&mutex_a);
}

/* ✅ 合规 - 尝试锁 */
void try_lock_pattern(void)
{
    pthread_mutex_lock(&mutex_a);
    if (pthread_mutex_trylock(&mutex_b) != 0) {
        pthread_mutex_unlock(&mutex_a);
        return;  /* 稍后重试 */
    }
    /* 成功获取两个锁 */
    /* ... */
    pthread_mutex_unlock(&mutex_b);
    pthread_mutex_unlock(&mutex_a);
}
