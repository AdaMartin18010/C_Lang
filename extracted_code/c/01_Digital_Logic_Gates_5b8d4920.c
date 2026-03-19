/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\09_Physical_Machine_Layer\01_Digital_Logic_Gates.md
 * Line: 157
 * Language: c
 * Block ID: 5b8d4920
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// NAND实现所有基本运算
#define NAND(a, b) (!(a) || !(b))

#define NOT_NAND(a) NAND(a, a)
#define AND_NAND(a, b) NAND(NAND(a, b), NAND(a, b))
#define OR_NAND(a, b) NAND(NAND(a, a), NAND(b, b))
