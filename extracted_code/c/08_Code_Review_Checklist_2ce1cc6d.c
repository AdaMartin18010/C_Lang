/*
 * Auto-generated from: 01_Core_Knowledge_System\05_Engineering\08_Code_Review_Checklist.md
 * Line: 509
 * Language: c
 * Block ID: 2ce1cc6d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 错误示例：非原子检查- then-动作
struct connection *conn = NULL;

void send_data(const void *data, size_t len) {
    if (conn != NULL) {  // 检查
        // 上下文切换，conn可能被其他线程释放
        conn_send(conn, data, len);  // 动作 - UAF风险！
    }
}

// ✅ 正确示例：原子检查-then-动作
void send_data_safe(const void *data, size_t len) {
    pthread_mutex_lock(&conn_mutex);
    if (conn != NULL) {
        conn_send(conn, data, len);  // 在锁保护下执行
    }
    pthread_mutex_unlock(&conn_mutex);
}

// ✅ 更好示例：引用计数
struct connection_ref {
    struct connection *ptr;
    atomic_int ref_count;
    pthread_mutex_t mutex;
};

struct connection *conn_acquire(struct connection_ref *ref) {
    pthread_mutex_lock(&ref->mutex);
    if (ref->ptr != NULL) {
        atomic_fetch_add(&ref->ref_count, 1);
    }
    struct connection *result = ref->ptr;
    pthread_mutex_unlock(&ref->mutex);
    return result;  // NULL 或有效引用
}

void conn_release(struct connection_ref *ref, struct connection *conn) {
    if (atomic_fetch_sub(&ref->ref_count, 1) == 1) {
        // 最后一个引用，可以安全释放
        conn_close(conn);
    }
}
