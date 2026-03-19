/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 3509
 * Language: c
 * Block ID: 9e6a6723
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void thread_safe_function(void) {
    pthread_mutex_lock(&shared_mutex);
    defer pthread_mutex_unlock(&shared_mutex);

    // 访问共享资源...
}
