/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\11_Lab11_Optional_Challenges.md
 * Line: 171
 * Language: c
 * Block ID: 7f6227ff
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 简单的内存分配跟踪
struct alloc_record {
    void *addr;
    uint64_t size;
    const char *file;
    int line;
    struct alloc_record *next;
};

void* tracked_malloc(uint64_t size, const char *file, int line) {
    void *p = kalloc(size);
    if (p) {
        struct alloc_record *r = kmalloc(sizeof(*r));
        r->addr = p;
        r->size = size;
        r->file = file;
        r->line = line;
        r->next = alloc_list;
        alloc_list = r;
    }
    return p;
}

void print_leaks() {
    struct alloc_record *r = alloc_list;
    while (r) {
        printf("Leak: %p, %lu bytes at %s:%d\n",
               r->addr, r->size, r->file, r->line);
        r = r->next;
    }
}

#define malloc(size) tracked_malloc(size, __FILE__, __LINE__)
