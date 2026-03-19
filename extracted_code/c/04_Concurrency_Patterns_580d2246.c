/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\04_Concurrency_Patterns.md
 * Line: 143
 * Language: c
 * Block ID: 580d2246
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

pthread_attr_t attr;
pthread_attr_init(&attr);

// 设置分离状态
pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

// 设置栈大小 (默认通常8MB)
size_t stacksize = 1024 * 1024;  // 1MB
pthread_attr_setstacksize(&attr, stacksize);

// 创建线程
pthread_t tid;
pthread_create(&tid, &attr, thread_func, NULL);

pthread_attr_destroy(&attr);
