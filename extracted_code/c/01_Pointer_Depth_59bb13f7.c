/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\01_Pointer_Depth.md
 * Line: 578
 * Language: c
 * Block ID: 59bb13f7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

float f;
int *pi = (int*)&f;  /* 违反严格别名规则 */
*pi = 0x7FFFFFFF;     /* UB，除非通过union */
