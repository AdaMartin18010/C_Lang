/*
 * Auto-generated from: 01_Core_Knowledge_System\08_Application_Domains\01_OS_Kernel.md
 * Line: 435
 * Language: c
 * Block ID: f29cd5dc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 在中断上下文中睡眠
void interrupt_handler(void) {
    mutex_lock(&lock);  // 可能睡眠！在中断上下文崩溃
    // ...
}

// ✅ 使用自旋锁（不睡眠）
void interrupt_handler_safe(void) {
    spin_lock(&lock);  // 忙等待，不睡眠
    // ...
    spin_unlock(&lock);
}

// ✅ 工作队列延后处理
void interrupt_handler_defer(void) {
    // 快速ack中断
    schedule_work(&my_work);  // 延后到进程上下文处理
}
