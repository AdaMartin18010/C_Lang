/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\05_Frama_C_LLM_Integration.md
 * Line: 133
 * Language: c
 * Block ID: 36d76c74
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* Frama-C LLM插件核心接口 (frama-c-llm.mli) */

module type LLM_PROVIDER = sig
  type t

  (* LLM配置 *)
  val name : string
  val version : string
  val max_tokens : int

  (* 核心功能 *)
  val generate_acsl :
    Cil_types.kernel_function ->
    string option ->
    (string, string) result

  val suggest_invariant :
    Cil_types.stmt ->
    (string list, string) result

  val analyze_failure :
    Wp.ProverTask.task ->
    (string, string) result

  val complete_proof :
    Wp.Goals.t ->
    (string option, string) result
end
