/*
 * Auto-generated from: 07_Modern_Toolchain\11_Workflow_StateMachines\README.md
 * Line: 830
 * Language: c
 * Block ID: 6834a9d1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 事件总线核心组件

typedef struct event {
    char type[128];
    char id[64];
    void* payload;
    size_t payload_size;
    time_t timestamp;
    char source[128];
} event_t;

typedef void (*event_handler_t)(event_t* event, void* context);

typedef struct {
    char event_type[128];
    event_handler_t handler;
    void* context;
    int priority;
} subscription_t;

typedef struct {
    subscription_t* subscriptions;
    size_t subscription_count;
    pthread_mutex_t lock;

    // 异步处理
    queue_t* event_queue;
    pthread_t* worker_threads;
    size_t worker_count;
    bool running;
} event_bus_t;

// 创建事件总线
event_bus_t* event_bus_create(size_t worker_count) {
    event_bus_t* bus = calloc(1, sizeof(event_bus_t));
    bus->subscriptions = calloc(64, sizeof(subscription_t));
    pthread_mutex_init(&bus->lock, NULL);

    // 创建事件队列
    bus->event_queue = queue_create(1024);
    bus->worker_count = worker_count;
    bus->worker_threads = calloc(worker_count, sizeof(pthread_t));
    bus->running = true;

    // 启动工作线程
    for (size_t i = 0; i < worker_count; i++) {
        pthread_create(&bus->worker_threads[i], NULL,
                       event_worker_thread, bus);
    }

    return bus;
}

// 订阅事件
void event_bus_subscribe(event_bus_t* bus, const char* event_type,
                         event_handler_t handler, void* context,
                         int priority) {
    pthread_mutex_lock(&bus->lock);

    subscription_t* sub = &bus->subscriptions[bus->subscription_count++];
    strncpy(sub->event_type, event_type, 127);
    sub->handler = handler;
    sub->context = context;
    sub->priority = priority;

    pthread_mutex_unlock(&bus->lock);
}

// 发布事件
void event_bus_publish(event_bus_t* bus, event_t* event) {
    // 复制事件到队列
    event_t* event_copy = copy_event(event);
    queue_enqueue(bus->event_queue, event_copy);
}

// 工作线程
void* event_worker_thread(void* arg) {
    event_bus_t* bus = (event_bus_t*)arg;

    while (bus->running) {
        event_t* event = queue_dequeue_timeout(bus->event_queue, 100);
        if (!event) continue;

        // 查找匹配的订阅者
        pthread_mutex_lock(&bus->lock);

        // 按优先级排序处理
        subscription_t matching[64];
        size_t match_count = 0;

        for (size_t i = 0; i < bus->subscription_count; i++) {
            if (strcmp(bus->subscriptions[i].event_type, event->type) == 0 ||
                strcmp(bus->subscriptions[i].event_type, "*") == 0) {
                matching[match_count++] = bus->subscriptions[i];
            }
        }

        pthread_mutex_unlock(&bus->lock);

        // 按优先级排序
        qsort(matching, match_count, sizeof(subscription_t),
              compare_subscription_priority);

        // 调用处理器
        for (size_t i = 0; i < match_count; i++) {
            matching[i].handler(event, matching[i].context);
        }

        free_event(event);
    }

    return NULL;
}
