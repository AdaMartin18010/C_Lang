/*
 * Auto-generated from: 00_VERSION_TRACKING\C2y_New_Proposals_2026.md
 * Line: 62
 * Language: c
 * Block ID: 5bfa47c9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C23及之前
void* ptr = get_ptr();
void* safe_ptr = ptr ? ptr : default_ptr;

// C2y 提案 (N3753)
void* safe_ptr = ptr ?: default_ptr;  // 如果ptr为真，返回ptr，否则返回default_ptr
