/*
 * Auto-generated from: 14_Video_Tutorials\01_Video_Scripts\10_Formal_Verification.md
 * Line: 829
 * Language: c
 * Block ID: e8965142
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#define HEAP_SIZE 1024

static char heap[HEAP_SIZE];
static int heap_used = 0;

/*@ logic boolean is_allocated(void *p, integer size) =
      (char*)p >= heap && (char*)p + size <= heap + heap_used;
*/

/*@ requires size >= 0;
    requires heap_used + size <= HEAP_SIZE;
    assigns heap_used;
    ensures \result == \null ==> heap_used == \old(heap_used);
    ensures \result != \null ==> is_allocated(\result, size);
    ensures \result != \null ==> \valid((char*)\result + (0..size-1));
*/
void *simple_alloc(int size) {
    if (heap_used + size > HEAP_SIZE) {
        return NULL;
    }

    void *ptr = &heap[heap_used];
    heap_used += size;

    return ptr;
}

/*@ requires is_allocated(ptr, size);
    // 简化: 不实现真正的释放
    assigns \nothing;
*/
void simple_free(void *ptr, int size) {
    // 标记为释放 (简化实现)
}
