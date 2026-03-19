/*
 * Auto-generated from: 17_Ada_SPARK\03_Ada_C_Interoperability\01_Calling_C_from_Ada.md
 * Line: 224
 * Language: c
 * Block ID: c42e3001
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef void (*Callback)(int value, void* user_data);

void register_callback(Callback cb, void* user_data);
void trigger_event(int value);
