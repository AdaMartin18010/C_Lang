/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\08_MetAcsl_Metaprogramming.md
 * Line: 566
 * Language: c
 * Block ID: 2ef6fdb0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// metacsl_case3_concurrent_safety.c
#include <pthread.h>

/*@
  @meta rule thread_safety {
    // 自动识别并发模式
    
    patterns: {
      "mutex_protected": {
        match: "pthread_mutex_lock(&m); ... pthread_mutex_unlock(&m)",
        generate: [
          "@requires \valid(&m);",
          "@ensures mutex_held(&m);",
          "critical_section: true"
        ]
      },
      
      "shared_variable": {
        match: "global_var = expr",
        condition: "global_var is shared",
        generate: [
          "@requires mutex_held_for(&global_var);",
          "@assigns global_var;"
        ]
      },
      
      "race_condition_check": {
        detect: "unsynchronized_access_to(shared_var)",
        warning: "Potential data race on shared_var"
      }
    }
  }
*/

static pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;
static int g_counter = 0;

// MetAcsl自动生成线程安全规范
/*@
  @requires \valid(&g_mutex);
  @assigns g_counter;
  @ensures g_counter == \old(g_counter) + 1;
  @behavior thread_safe:
    @ensures mutex_held_during(&g_mutex);
*/
void increment_counter(void) {
    pthread_mutex_lock(&g_mutex);
    g_counter++;
    pthread_mutex_unlock(&g_mutex);
}
