/*
 * Auto-generated from: 06_Thinking_Representation\05_Concept_Mappings\README.md
 * Line: 413
 * Language: c
 * Block ID: 5afaabaa
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 完整的虚函数表示例
// animal.h
typedef struct animal_vtable animal_vtable_t;
typedef struct animal animal_t;

struct animal {
    const animal_vtable_t* vtable;
    char name[32];
    int age;
};

struct animal_vtable {
    void (*speak)(const animal_t* self);
    void (*move)(animal_t* self, float x, float y);
    const char* (*get_species)(const animal_t* self);
};

// 内联多态调用
static inline void animal_speak(const animal_t* a) {
    a->vtable->speak(a);
}

// dog.c - 具体实现
static void dog_speak(const animal_t* self) {
    printf("%s says: Woof!\n", self->name);
}

static const animal_vtable_t dog_vtable = {
    .speak = dog_speak,
    .move = animal_default_move,
    .get_species = dog_get_species
};

animal_t* dog_create(const char* name, int age) {
    animal_t* dog = malloc(sizeof(animal_t));
    dog->vtable = &dog_vtable;
    strncpy(dog->name, name, 31);
    dog->age = age;
    return dog;
}
