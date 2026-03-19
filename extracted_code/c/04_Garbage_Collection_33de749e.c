/*
 * Auto-generated from: 03_System_Technology_Domains\04_Garbage_Collection.md
 * Line: 159
 * Language: c
 * Block ID: 33de749e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

void* gc_alloc(size_t size) { return GC_malloc(size); }
void* manual_alloc(size_t size) { return malloc(size); }

int main() {
    GC_INIT();
    char* temp = (char*)gc_alloc(1024);      // GC管理
    void* critical = manual_alloc(1024);      // 手动管理
    free(critical);   // 必须释放
    return 0;
}
