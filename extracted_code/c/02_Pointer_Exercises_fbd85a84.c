/*
 * Auto-generated from: 12_Practice_Exercises\02_Pointer_Exercises.md
 * Line: 161
 * Language: c
 * Block ID: fbd85a84
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 实现简单的事件系统
typedef void (*EventHandler)(void *event_data);

void register_handler(const char *event_type, EventHandler handler);
void trigger_event(const char *event_type, void *event_data);
