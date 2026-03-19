/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\08_MetAcsl_Metaprogramming.md
 * Line: 217
 * Language: c
 * Block ID: daa4b5a0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*@
  @meta query {
    // 查询函数参数
    parameters: {
      pointer_params: count_of(\valid(arg)),
      array_params: count_of(\valid(arg + (0..size-1))),
      scalar_params: count_of(true)
    },
    
    // 查询函数体
    body: {
      has_loops: exists(stmt is Loop),
      has_allocation: exists(call is malloc),
      has_pointer_write: exists(assign to *ptr)
    },
    
    // 基于查询生成规范
    generate: {
      if (pointer_params > 0) add_requires(pointer_validity),
      if (has_loops) add_loop_invariants(auto_infer),
      if (has_allocation) add_memory_tracking
    }
  }
*/
