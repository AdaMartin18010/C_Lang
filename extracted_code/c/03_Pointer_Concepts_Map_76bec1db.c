/*
 * Auto-generated from: 06_Thinking_Representation\05_Concept_Mappings\03_Pointer_Concepts_Map.md
 * Line: 113
 * Language: c
 * Block ID: 76bec1db
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 泛型交换
void swap(void *a, void *b, size_t size) {
    char temp[size];
    memcpy(temp, a, size);
    memcpy(a, b, size);
    memcpy(b, temp, size);
}
