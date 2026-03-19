/*
 * Auto-generated from: 06_Thinking_Representation\05_Concept_Mappings\04_Concurrent_Programming_Map.md
 * Line: 106
 * Language: c
 * Block ID: 2a162e4e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 按地址排序获取
void lock_ordered(mtx_t *a, mtx_t *b) {
    if (a < b) { mtx_lock(a); mtx_lock(b); }
    else { mtx_lock(b); mtx_lock(a); }
}
