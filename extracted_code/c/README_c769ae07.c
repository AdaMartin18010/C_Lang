/*
 * Auto-generated from: 06_Thinking_Representation\04_Case_Studies\README.md
 * Line: 147
 * Language: c
 * Block ID: c769ae07
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Redis 事件循环核心 - ae.c

typedef struct aeEventLoop {
    int maxfd;
    int setsize;
    long long timeEventNextId;
    aeFileEvent *events;  // 文件事件数组
    aeFiredEvent *fired;  // 已触发事件
    aeTimeEvent *timeEventHead;
    int stop;
    void *apidata;  // 平台特定数据 (epoll/kqueue/select)
    aeBeforeSleepProc *beforesleep;
} aeEventLoop;

// 事件处理主循环
void aeMain(aeEventLoop *eventLoop) {
    eventLoop->stop = 0;
    while (!eventLoop->stop) {
        if (eventLoop->beforesleep)
            eventLoop->beforesleep(eventLoop);
        aeProcessEvents(eventLoop, AE_ALL_EVENTS|AE_CALL_AFTER_SLEEP);
    }
}
