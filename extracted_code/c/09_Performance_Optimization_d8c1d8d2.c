/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\09_Performance_Optimization.md
 * Line: 915
 * Language: c
 * Block ID: d8c1d8d2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 读多写少: 读写锁
pthread_rwlock_t rwlock;

void read_data() {
    pthread_rwlock_rdlock(&rwlock);  // 多个读者
    // 读取
    pthread_rwlock_unlock(&rwlock);
}

void write_data() {
    pthread_rwlock_wrlock(&rwlock);  // 独占
    // 写入
    pthread_rwlock_unlock(&rwlock);
}

// 或者使用RCU (Linux内核风格)
rcu_read_lock();
p = rcu_dereference(global_ptr);  // 获取指针
// 使用p (只读)
rcu_read_unlock();

// 写入端
new_ptr = kmalloc(...);
*new_ptr = new_data;
rcu_assign_pointer(global_ptr, new_ptr);
synchronize_rcu();  // 等待读者退出
kfree(old_ptr);
