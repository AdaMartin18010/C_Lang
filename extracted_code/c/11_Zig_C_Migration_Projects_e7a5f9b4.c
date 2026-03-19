/*
 * Auto-generated from: 12_Practice_Exercises\11_Zig_C_Migration_Projects.md
 * Line: 263
 * Language: c
 * Block ID: e7a5f9b4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// clib.h
#ifndef CLIB_H
#define CLIB_H

typedef struct {
    char name[64];
    int age;
} Person;

Person* person_create(const char *name, int age);
void person_free(Person *p);
void person_greet(const Person *p);

#endif
