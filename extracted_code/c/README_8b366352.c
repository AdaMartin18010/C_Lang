/*
 * Auto-generated from: 07_Modern_Toolchain\08_Design_Patterns\README.md
 * Line: 674
 * Language: c
 * Block ID: 8b366352
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// observer.h
#ifndef OBSERVER_H
#define OBSERVER_H

typedef struct Observer Observer;
typedef struct Subject Subject;

struct Observer {
    void (*update)(Observer* self, const char* event, void* data);
    void (*destroy)(Observer* self);
};

struct Subject {
    void (*attach)(Subject* self, Observer* observer);
    void (*detach)(Subject* self, Observer* observer);
    void (*notify)(Subject* self, const char* event, void* data);
};

// 创建具体主题
Subject* event_bus_create(void);

#endif
