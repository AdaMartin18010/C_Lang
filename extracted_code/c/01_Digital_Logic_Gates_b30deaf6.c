/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\01_Digital_Logic_Gates.md
 * Line: 210
 * Language: c
 * Block ID: b30deaf6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// CMOS逻辑门的晶体管级模拟

// NMOS晶体管模型（高电平导通）
typedef struct {
    Bool gate;      // 栅极
    Bool source;    // 源极
    Bool drain;     // 漏极（输出）
    Bool conducting;// 导通状态
} NMOS;

// PMOS晶体管模型（低电平导通）
typedef struct {
    Bool gate;
    Bool source;
    Bool drain;
    Bool conducting;
} PMOS;

void update_nmos(NMOS *t) {
    t->conducting = t->gate;  // 栅极高电平导通
    if (t->conducting) {
        t->drain = t->source;
    }
}

void update_pmos(PMOS *t) {
    t->conducting = !t->gate;  // 栅极低电平导通
    if (t->conducting) {
        t->drain = t->source;
    }
}

// CMOS反相器（NOT门）
Bool cmos_not(Bool input) {
    // 上拉网络（PMOS）：输入0时导通，输出1
    // 下拉网络（NMOS）：输入1时导通，输出0
    if (input == FALSE) {
        return TRUE;   // PMOS导通，Vdd到输出
    } else {
        return FALSE;  // NMOS导通，输出到地
    }
}

// CMOS NAND门
Bool cmos_nand(Bool a, Bool b) {
    // 下拉网络：两个NMOS串联，都导通时输出0
    // 上拉网络：两个PMOS并联，任一导通时输出1
    if (a == TRUE && b == TRUE) {
        return FALSE;  // 两个NMOS都导通
    } else {
        return TRUE;   // 至少一个PMOS导通
    }
}
