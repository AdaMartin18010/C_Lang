/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 2574
 * Language: c
 * Block ID: 875fe2a2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ✅ 好的实践
char *buffer = malloc(size);
if (!buffer) return NULL;
defer free(buffer);
