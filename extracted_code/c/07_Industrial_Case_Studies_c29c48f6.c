/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\11_CompCert_Verification\07_Industrial_Case_Studies.md
 * Line: 772
 * Language: c
 * Block ID: c29c48f6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// industrial_interlock.c

#define NUM_ZONES       4
#define ZONE_SAFE       0
#define ZONE_WARNING    1
#define ZONE_DANGER     2

typedef struct {
    uint8_t status[NUM_ZONES];
    bool emergency_stop;
    uint8_t master_state;
} SafetySystem;

/*@
  @requires \valid(system);
  @assigns system->master_state;
  @ensures system->emergency_stop ==> system->master_state == 0;
  @ensures (\forall integer i; 0 <= i < NUM_ZONES ==>
    system->status[i] == ZONE_SAFE) ==>
    system->master_state == 1;
  @ensures (\exists integer i; 0 <= i < NUM_ZONES &&
    system->status[i] == ZONE_DANGER) ==>
    system->master_state == 0;
*/
void update_safety_state(SafetySystem* system) {
    if (system->emergency_stop) {
        system->master_state = 0;  // 紧急停止状态
        return;
    }

    // 检查所有区域
    bool all_safe = true;
    bool any_danger = false;

    /*@
      @loop invariant 0 <= i <= NUM_ZONES;
      @loop assigns i, all_safe, any_danger;
    */
    for (int i = 0; i < NUM_ZONES; i++) {
        if (system->status[i] != ZONE_SAFE) {
            all_safe = false;
        }
        if (system->status[i] == ZONE_DANGER) {
            any_danger = true;
        }
    }

    if (any_danger) {
        system->master_state = 0;  // 危险,停止
    } else if (all_safe) {
        system->master_state = 1;  // 全部安全,运行
    } else {
        system->master_state = 2;  // 警告状态
    }
}
