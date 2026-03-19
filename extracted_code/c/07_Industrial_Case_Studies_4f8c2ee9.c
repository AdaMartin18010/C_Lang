/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\07_Industrial_Case_Studies.md
 * Line: 739
 * Language: c
 * Block ID: 4f8c2ee9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// industrial_plc.c
// PLC功能块验证

#define MAX_TEMPERATURE  1500   // 0.1°C
#define MAX_PRESSURE     10000  // 0.01 bar
#define SAFETY_TEMP      1200   // 安全温度

/*@
  @predicate safe_operating_conditions(int16_t temp, int16_t pressure) =
    temp <= SAFETY_TEMP && pressure <= MAX_PRESSURE;
*/

/*@
  @requires \valid(command);
  @assigns *command;
  @ensures !safe_operating_conditions(temp, pressure) ==>
    *command == 0;  // 关闭命令
  @ensures safe_operating_conditions(temp, pressure) ==>
    *command == old_command || *command == 0;
*/
void safety_interlock(int16_t temp, int16_t pressure,
                      uint8_t old_command, uint8_t* command) {
    if (temp > SAFETY_TEMP || pressure > MAX_PRESSURE) {
        *command = 0;  // 紧急停止
    } else {
        *command = old_command;  // 保持当前命令
    }
}
