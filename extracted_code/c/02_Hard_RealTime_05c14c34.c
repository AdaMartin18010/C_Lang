/*
 * Auto-generated from: 04_Industrial_Scenarios\01_Automotive_ABS\02_Hard_RealTime.md
 * Line: 677
 * Language: c
 * Block ID: 05c14c34
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 问题: 低优先级任务持有高优先级任务需要的资源
// 导致中等优先级任务抢占低优先级任务，高优先级任务被延迟

// ✅ 解决方案1: 优先级继承
void mutex_lock(PriorityMutex *m) {
    disable_interrupts();
    if (m->owner == NULL) {
        m->owner = current_task;
    } else {
        // 将资源持有者的优先级提升
        if (m->owner->priority > current_task->priority) {
            m->owner->priority = current_task->priority;
            m->priority_raised = true;
        }
        // 将当前任务加入等待队列
        add_to_wait_queue(current_task, m);
        block_task();
    }
    enable_interrupts();
}

void mutex_unlock(PriorityMutex *m) {
    disable_interrupts();
    if (m->priority_raised) {
        // 恢复原始优先级
        m->owner->priority = m->owner->base_priority;
        m->priority_raised = false;
    }
    // 唤醒等待队列中的任务
    wake_highest_waiter(m);
    m->owner = NULL;
    enable_interrupts();
}

// ✅ 解决方案2: 优先级天花板
void mutex_lock_ceiling(PriorityMutex *m) {
    disable_interrupts();
    // 直接提升到天花板优先级
    uint8_t old_priority = current_task->priority;
    current_task->priority = m->ceiling_priority;

    // 获取资源
    // ...

    current_task->saved_priority = old_priority;
    enable_interrupts();
}
