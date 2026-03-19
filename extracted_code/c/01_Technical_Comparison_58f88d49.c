/*
 * Auto-generated from: 15_Memory_Safe_Languages\01_Rust_vs_C\01_Technical_Comparison.md
 * Line: 31
 * Language: c
 * Block ID: 58f88d49
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C代码 - 需要程序员管理所有内存
typedef struct {
    char* name;
    int age;
} Person;

Person* person_create(const char* name, int age) {
    Person* p = malloc(sizeof(Person));
    if (!p) return NULL;

    p->name = malloc(strlen(name) + 1);
    if (!p->name) {
        free(p);  // 容易遗漏!
        return NULL;
    }

    strcpy(p->name, name);
    p->age = age;
    return p;
}
