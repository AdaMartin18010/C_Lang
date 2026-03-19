/*
 * Auto-generated from: 07_Modern_Toolchain\09_System_Architecture\02_Event_Driven_Architecture.md
 * Line: 273
 * Language: c
 * Block ID: 9a602def
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 类型安全的事件总线 */

typedef void (*event_handler_t)(void* event_data, void* context);

typedef struct event_subscription {
    int event_type;
    event_handler_t handler;
    void* context;
    struct event_subscription* next;
} event_subscription_t;

typedef struct {
    event_subscription_t* subs;
    mtx_t lock;
    /* 事件队列 */
    void** event_queue;
    int queue_head;
    int queue_tail;
    int queue_size;
    cnd_t has_event;
} event_bus_t;

event_bus_t* event_bus_create(int queue_size) {
    event_bus_t* bus = calloc(1, sizeof(event_bus_t));
    mtx_init(&bus->lock, mtx_plain);
    cnd_init(&bus->has_event);
    bus->event_queue = calloc(queue_size, sizeof(void*));
    bus->queue_size = queue_size;
    return bus;
}

/* 订阅 */
void event_bus_subscribe(event_bus_t* bus, int event_type,
                         event_handler_t handler, void* context) {
    event_subscription_t* sub = malloc(sizeof(event_subscription_t));
    sub->event_type = event_type;
    sub->handler = handler;
    sub->context = context;

    mtx_lock(&bus->lock);
    sub->next = bus->subs;
    bus->subs = sub;
    mtx_unlock(&bus->lock);
}

/* 发布 */
void event_bus_publish(event_bus_t* bus, int event_type, void* event_data) {
    mtx_lock(&bus->lock);

    /* 查找订阅者并直接调用 (同步) */
    event_subscription_t* sub = bus->subs;
    while (sub) {
        if (sub->event_type == event_type) {
            sub->handler(event_data, sub->context);
        }
        sub = sub->next;
    }

    mtx_unlock(&bus->lock);
}

/* 异步发布 */
void event_bus_publish_async(event_bus_t* bus, int event_type, void* event_data) {
    mtx_lock(&bus->lock);

    int next = (bus->queue_tail + 1) % bus->queue_size;
    if (next != bus->queue_head) {
        bus->event_queue[bus->queue_tail] = event_data;
        bus->queue_tail = next;
        cnd_signal(&bus->has_event);
    }

    mtx_unlock(&bus->lock);
}

/* 分发线程 */
int event_dispatch_thread(void* arg) {
    event_bus_t* bus = arg;

    while (1) {
        mtx_lock(&bus->lock);
        while (bus->queue_head == bus->queue_tail) {
            cnd_wait(&bus->has_event, &bus->lock);
        }

        void* event = bus->event_queue[bus->queue_head];
        bus->queue_head = (bus->queue_head + 1) % bus->queue_size;
        mtx_unlock(&bus->lock);

        /* 分发 */
        /* ... */
    }
    return 0;
}
