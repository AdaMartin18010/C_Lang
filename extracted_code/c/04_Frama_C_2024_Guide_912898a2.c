/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\04_Frama_C_2024_Guide.md
 * Line: 996
 * Language: c
 * Block ID: 912898a2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// DO-178C C级软件示例
/*@
  @requires cmd != \null;
  @requires cmd->id >= 0 && cmd->id < MAX_CMD_ID;
  @requires is_valid_command(cmd);
  @assigns command_log[0..];
  @ensures execution_status(cmd->id) == CMD_EXECUTED;
  @behavior high_priority:
    @assumes cmd->priority == PRIORITY_HIGH;
    @ensures response_time_us < MAX_RESPONSE_HIGH_US;
  @behavior normal_priority:
    @assumes cmd->priority != PRIORITY_HIGH;
    @ensures response_time_us < MAX_RESPONSE_NORMAL_US;
  @complete behaviors;
  @disjoint behaviors;
*/
Std_ReturnType execute_flight_command(const Command* cmd);
