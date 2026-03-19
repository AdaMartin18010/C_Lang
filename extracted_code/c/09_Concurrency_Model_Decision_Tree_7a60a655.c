/*
 * Auto-generated from: 06_Thinking_Representation\01_Decision_Trees\09_Concurrency_Model_Decision_Tree.md
 * Line: 277
 * Language: c
 * Block ID: 7a60a655
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ✅ 正确：防止虚假唤醒
pthread_mutex_lock(&mutex);
while (!condition) {  // 使用while而非if
    pthread_cond_wait(&cond, &mutex);
}
// 使用共享数据
pthread_mutex_unlock(&mutex);

// ✅ 正确：避免唤醒丢失
pthread_mutex_lock(&mutex);
condition = true;  // 先修改条件
pthread_cond_signal(&cond);  // 再发送信号
pthread_mutex_unlock(&mutex);
