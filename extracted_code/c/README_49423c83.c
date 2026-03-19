/*
 * Auto-generated from: 07_Modern_Toolchain\08_Design_Patterns\README.md
 * Line: 699
 * Language: c
 * Block ID: 49423c83
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// observer.c
#include "observer.h"
#include <stdlib.h>
#include <string.h>

#define MAX_OBSERVERS 32

typedef struct {
    Subject interface;
    Observer* observers[MAX_OBSERVERS];
    int count;
} EventBus;

static void event_bus_attach(Subject* self, Observer* observer) {
    EventBus* bus = (EventBus*)self;
    if (bus->count < MAX_OBSERVERS) {
        bus->observers[bus->count++] = observer;
    }
}

static void event_bus_detach(Subject* self, Observer* observer) {
    EventBus* bus = (EventBus*)self;
    for (int i = 0; i < bus->count; i++) {
        if (bus->observers[i] == observer) {
            // 移动后续元素
            memmove(&bus->observers[i], &bus->observers[i+1],
                   (bus->count - i - 1) * sizeof(Observer*));
            bus->count--;
            break;
        }
    }
}

static void event_bus_notify(Subject* self, const char* event, void* data) {
    EventBus* bus = (EventBus*)self;
    for (int i = 0; i < bus->count; i++) {
        bus->observers[i]->update(bus->observers[i], event, data);
    }
}

Subject* event_bus_create(void) {
    EventBus* bus = calloc(1, sizeof(EventBus));
    bus->interface.attach = event_bus_attach;
    bus->interface.detach = event_bus_detach;
    bus->interface.notify = event_bus_notify;
    return (Subject*)bus;
}
