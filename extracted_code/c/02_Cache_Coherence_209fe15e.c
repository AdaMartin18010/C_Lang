/*
 * Auto-generated from: 04_Industrial_Scenarios\13_Linux_Kernel\02_Cache_Coherence.md
 * Line: 838
 * Language: c
 * Block ID: 209fe15e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误: RCU读者侧睡眠
void bad_rcu_reader(void) {
    rcu_read_lock();
    struct data *p = rcu_dereference(global_ptr);

    schedule();  // 错误! RCU读侧临界区不能睡眠

    use(p);
    rcu_read_unlock();
}

// ✅ 正确: RCU临界区快速完成
void good_rcu_reader(void) {
    rcu_read_lock();
    struct data *p = rcu_dereference(global_ptr);
    int value = p->value;  // 快速读取
    rcu_read_unlock();

    // 耗时操作在临界区外
    process(value);
}
