/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\Formal_Verification\Case_Studies.md
 * Line: 281
 * Language: c
 * Block ID: 881ef56c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 飞行控制律规约示例 */

/*@ requires \valid(pitch_cmd);
  @ requires -MAX_PITCH_RATE <= desired_rate <= MAX_PITCH_RATE;
  @ assigns *pitch_cmd;
  @ ensures -MAX_PITCH_CMD <= *pitch_cmd <= MAX_PITCH_CMD;
  @ ensures \abs(*pitch_cmd - expected_response(desired_rate)) < EPSILON;
  @*/
void compute_pitch_command(
    float desired_rate,
    float current_rate,
    float* pitch_cmd
);
