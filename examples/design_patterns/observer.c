/*
 * 观察者模式（多订阅者回调）示例
 * 编译: gcc -Wall -Wextra -o observer observer.c
 */
#include <stdio.h>
#include <stdlib.h>

#define MAX_OBSERVERS 16

typedef void (*EventCallback)(void *observer, int event_id, const char *data);

typedef struct {
    void *observer;
    EventCallback callback;
} ObserverEntry;

typedef struct {
    ObserverEntry observers[MAX_OBSERVERS];
    int count;
} Subject;

void subject_init(Subject *s) {
    s->count = 0;
}

int subject_subscribe(Subject *s, void *observer, EventCallback cb) {
    if (s->count >= MAX_OBSERVERS) return -1;
    s->observers[s->count].observer = observer;
    s->observers[s->count].callback = cb;
    s->count++;
    return 0;
}

void subject_notify(Subject *s, int event_id, const char *data) {
    for (int i = 0; i < s->count; i++) {
        s->observers[i].callback(s->observers[i].observer, event_id, data);
    }
}

/* 具体观察者 */
typedef struct {
    const char *name;
    int event_count;
} Logger;

void logger_on_event(void *self, int event_id, const char *data) {
    Logger *l = self;
    printf("[%s] Event %d: %s\n", l->name, event_id, data);
    l->event_count++;
}

int main(void) {
    Subject sensor;
    subject_init(&sensor);
    
    Logger console = {"ConsoleLogger", 0};
    Logger file = {"FileLogger", 0};
    
    subject_subscribe(&sensor, &console, logger_on_event);
    subject_subscribe(&sensor, &file, logger_on_event);
    
    /* 触发事件 */
    subject_notify(&sensor, 1, "Temperature: 25C");
    subject_notify(&sensor, 2, "Pressure: 1013hPa");
    
    printf("Console received %d events\n", console.event_count);
    printf("File received %d events\n", file.event_count);
    
    return 0;
}
