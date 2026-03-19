/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\08_MetAcsl_Metaprogramming.md
 * Line: 325
 * Language: c
 * Block ID: d3eb25f4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*@
  @meta context_sensitive {
    // 根据调用者生成不同规范
    caller_context: {
      if (caller_in_module("crypto")) {
        add_requires("constant_time_execution")
      },
      if (caller_in_module("safety_critical")) {
        add_requires("no_interrupts_during")
      }
    },
    
    // 根据数据流分析
    dataflow_context: {
      if (param_is_tainted(x)) {
        add_requires("sanitize_input(x)")
      }
    }
  }
*/
