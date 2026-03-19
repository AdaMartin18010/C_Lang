/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\04_Cognitive_Representation\02_Embodied_Cognition.md
 * Line: 282
 * Language: c
 * Block ID: 7238ab1b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 使用物理世界隐喻理解并发

// 互斥锁作为"门锁"
pthread_mutex_t room_lock = PTHREAD_MUTEX_INITIALIZER;

void enter_room(void) {
    // "敲门并等待进入"
    pthread_mutex_lock(&room_lock);
    // "现在在房间内，独占访问"
    do_work();
    // "离开房间，解锁门"
    pthread_mutex_unlock(&room_lock);
}

// 信号量作为"计数令牌"
sem_t parking_spaces;

void park_car(void) {
    // "获取一个停车令牌（如果没有则等待）"
    sem_wait(&parking_spaces);
    // "停车"
    // ...
    // "离开，归还令牌"
    sem_post(&parking_spaces);
}
