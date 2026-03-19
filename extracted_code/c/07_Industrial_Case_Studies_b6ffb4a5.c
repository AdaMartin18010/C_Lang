/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\07_Industrial_Case_Studies.md
 * Line: 430
 * Language: c
 * Block ID: b6ffb4a5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// autosar_asil_d.c
// ASIL-D级别验证示例

/*@
  @requires brake_pressure >= 0 && brake_pressure <= 10000;  // 0-100 bar
  @requires wheel_speeds != \null;
  @requires \valid_read(wheel_speeds + (0..3));
  @assigns \nothing;

  // ASIL-D: 必须证明防抱死逻辑正确
  @ensures \result ==>
    \forall integer i; 0 <= i < 4 ==>
      wheel_speeds[i] > 0;  // 不抱死

  @ensures !\result ==>
    (\exists integer i; 0 <= i < 4 && wheel_speeds[i] < 5);  // 可能抱死
*/
bool ABS_should_activate(uint16_t brake_pressure,
                         const uint16_t wheel_speeds[4]);
