/*
 * Auto-generated from: 07_Modern_Toolchain\08_Design_Patterns\01_Creational_Patterns.md
 * Line: 290
 * Language: c
 * Block ID: 2531948b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 复杂对象建造 */

typedef struct {
    char name[64];
    int age;
    char email[128];
    char phone[32];
    char address[256];
} person_t;

typedef struct {
    person_t* person;
} person_builder_t;

typedef struct {
    person_builder_t* (*with_name)(person_builder_t*, const char*);
    person_builder_t* (*with_age)(person_builder_t*, int);
    person_t* (*build)(person_builder_t*);
} person_builder_ops_t;

person_builder_t* person_builder_new(void) {
    person_builder_t* b = malloc(sizeof(person_builder_t));
    b->person = calloc(1, sizeof(person_t));
    return b;
}

person_t* person_builder_build(person_builder_t* b) {
    person_t* p = b->person;
    free(b);
    return p;
}

person_builder_t* person_builder_name(person_builder_t* b, const char* name) {
    strncpy(b->person->name, name, sizeof(b->person->name) - 1);
    return b;
}

/* 可选步骤 */
person_builder_t* person_builder_optional_email(person_builder_t* b,
                                                  const char* email) {
    if (email) {
        strncpy(b->person->email, email, sizeof(b->person->email) - 1);
    }
    return b;
}
