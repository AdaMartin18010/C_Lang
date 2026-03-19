/*
 * Auto-generated from: 06_Thinking_Representation\05_Concept_Mappings\README.md
 * Line: 54
 * Language: c
 * Block ID: b1ffb9c1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C: 完全手动的内存管理
typedef struct {
    char* name;
    int age;
    float* scores;
    size_t score_count;
} student_t;

student_t* student_create(const char* name, int age) {
    student_t* s = malloc(sizeof(student_t));
    if (!s) return NULL;

    s->name = strdup(name);
    s->age = age;
    s->scores = NULL;
    s->score_count = 0;
    return s;
}

void student_destroy(student_t* s) {
    if (s) {
        free(s->name);
        free(s->scores);
        free(s);
    }
}
