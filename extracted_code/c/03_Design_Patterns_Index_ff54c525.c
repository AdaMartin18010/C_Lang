/*
 * Auto-generated from: 06_Thinking_Representation\06_Index\03_Design_Patterns_Index.md
 * Line: 343
 * Language: c
 * Block ID: ff54c525
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 核心实现
typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t read_cond;
    pthread_cond_t write_cond;
    int readers;
    int writers_waiting;
    int writing;
} RWLock;
