/*
 * Auto-generated from: 12_Practice_Exercises\11_Zig_C_Migration_Projects.md
 * Line: 280
 * Language: c
 * Block ID: d36cc9e4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// clib.c
#include "clib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Person* person_create(const char *name, int age) {
    Person *p = malloc(sizeof(Person));
    strncpy(p->name, name, sizeof(p->name) - 1);
    p->name[sizeof(p->name) - 1] = '\0';
    p->age = age;
    return p;
}

void person_free(Person *p) {
    free(p);
}

void person_greet(const Person *p) {
    printf("Hello, I'm %s, %d years old.\n", p->name, p->age);
}
