/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 2557
 * Language: c
 * Block ID: 61a8c90f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ✅ 好的实践
FILE *fp = fopen("data.txt", "r");
if (!fp) return -1;
defer fclose(fp);
