/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 2566
 * Language: c
 * Block ID: 672c6a2e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ✅ 好的实践
pthread_mutex_lock(&mutex);
defer pthread_mutex_unlock(&mutex);
