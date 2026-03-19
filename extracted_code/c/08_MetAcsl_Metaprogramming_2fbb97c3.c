/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\08_MetAcsl_Metaprogramming.md
 * Line: 409
 * Language: c
 * Block ID: 2fbb97c3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// metacsl_case1_array_bounds.c

// 配置规则
/*@
  @meta rule array_access_safety {
    // 自动为所有数组访问添加边界检查
    
    match: {
      // 匹配数组下标访问
      pattern: "array[index]",
      condition: "index is variable"
    },
    
    generate: {
      requires: [
        "array != \null",
        "array_size > 0",
        "0 <= index < array_size"
      ],
      runtime_check: {
        enabled: true,
        action: "abort_on_violation"
      }
    },
    
    // 循环中的数组访问特殊处理
    loop_context: {
      generate_loop_invariant: true,
      invariant_template: "loop_var_bounds"
    }
  }
*/

// 原始代码 (无规范)
void process_buffer(char* buffer, int size, int offset) {
    char result = buffer[offset];  // 潜在越界!
    
    for (int i = 0; i < size; i++) {
        buffer[i] = process_byte(buffer[i]);
    }
}

// MetAcsl处理后:
/*@
  @requires buffer != \null;
  @requires size > 0;
  @requires 0 <= offset < size;  // 自动添加
  @requires \valid(buffer + (0..size-1));
  @assigns buffer[0..size-1];
  @ensures \forall integer i; 0 <= i < size ==>
    buffer[i] == process_byte(\old(buffer[i]));
*/
void process_buffer_annotated(char* buffer, int size, int offset) {
    char result = buffer[offset];
    
    /*@
      @loop invariant 0 <= i <= size;
      @loop invariant \forall integer j; 0 <= j < i ==>
        buffer[j] == process_byte(\at(buffer[j], LoopEntry));
      @loop assigns i, buffer[0..size-1];
      @loop variant size - i;
    */
    for (int i = 0; i < size; i++) {
        buffer[i] = process_byte(buffer[i]);
    }
}
