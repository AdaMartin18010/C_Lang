/*
 * Auto-generated from: 07_Modern_Toolchain\08_Design_Patterns\README.md
 * Line: 81
 * Language: c
 * Block ID: b5ef5bce
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// singleton.c
#include "singleton.h"
#include <string.h>

static Singleton instance;
static int initialized = 0;

Singleton* singleton_get_instance(void) {
    if (!initialized) {
        memset(&instance, 0, sizeof(instance));
        strcpy(instance.name, "Default");
        initialized = 1;
    }
    return &instance;
}

void singleton_set_data(Singleton* self, int value) {
    if (self == &instance) {
        self->data = value;
    }
}

int singleton_get_data(const Singleton* self) {
    return (self == &instance) ? self->data : 0;
}
