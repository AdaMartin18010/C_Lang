/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\08_MetAcsl_Metaprogramming.md
 * Line: 480
 * Language: c
 * Block ID: d04f542e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// metacsl_case2_smart_pointers.c
#include <stdlib.h>

/*@
  @meta rule smart_pointer_management {
    // 自动跟踪所有权语义
    
    patterns: {
      "allocation": {
        match: "ptr = malloc(size)",
        generate: [
          "@allocates ptr;",
          "@ensures ptr != \null ==> \valid(ptr);",
          "ownership: caller"
        ]
      },
      
      "deallocation": {
        match: "free(ptr)",
        generate: [
          "@frees ptr;",
          "@requires \valid(ptr) || ptr == \null;",
          "ownership: released"
        ]
      },
      
      "null_check": {
        match: "if (ptr != NULL)",
        generate: [
          "// 分支内: ptr is valid",
          "// 分支外: ptr may be null"
        ]
      }
    },
    
    // 内存泄漏检测
    leak_detection: {
      check_at_function_exit: true,
      ensure_all_allocated_freed: true
    }
  }
*/

typedef struct {
    int* data;
    size_t size;
    size_t capacity;
} IntVector;

// MetAcsl自动生成完整规范
/*@
  @requires \valid(v);
  @requires initial_capacity > 0;
  @allocates v->data;
  @assigns v->size, v->capacity, v->data;
  @ensures v->size == 0;
  @ensures v->capacity == initial_capacity;
  @ensures v->data != \null ==> \valid(v->data + (0..initial_capacity-1));
  @ensures v->data != \null ==> \initialized(v->data + (0..initial_capacity-1));
*/
void vector_init(IntVector* v, size_t initial_capacity) {
    v->data = (int*)malloc(initial_capacity * sizeof(int));
    v->size = 0;
    v->capacity = initial_capacity;
}

/*@
  @requires \valid(v);
  @requires \valid(v->data) || v->data == \null;
  @frees v->data;
  @assigns v->data, v->size, v->capacity;
  @ensures v->data == \null;
  @ensures v->size == 0;
  @ensures v->capacity == 0;
*/
void vector_destroy(IntVector* v) {
    free(v->data);
    v->data = NULL;
    v->size = 0;
    v->capacity = 0;
}
