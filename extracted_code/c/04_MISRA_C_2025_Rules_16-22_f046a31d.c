/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\04_MISRA_C_2025_Rules_16-22.md
 * Line: 264
 * Language: c
 * Block ID: f046a31d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 不符合规范 - 潜在死锁 */
void transfer(Account* from, Account* to, int amount) {
    pthread_mutex_lock(&from->lock);
    pthread_mutex_lock(&to->lock);  /* 如果反向调用则死锁 */
    /* 转账操作 */
    pthread_mutex_unlock(&to->lock);
    pthread_mutex_unlock(&from->lock);
}

/* 符合规范 - 锁排序 */
void transfer(Account* a, Account* b, int amount) {
    Account* first = (a < b) ? a : b;  /* 确定顺序 */
    Account* second = (a < b) ? b : a;

    pthread_mutex_lock(&first->lock);
    pthread_mutex_lock(&second->lock);
    /* 转账操作 */
    pthread_mutex_unlock(&second->lock);
    pthread_mutex_unlock(&first->lock);
}
