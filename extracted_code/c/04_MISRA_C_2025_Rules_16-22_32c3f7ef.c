/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\04_MISRA_C_2025_Rules_16-22.md
 * Line: 661
 * Language: c
 * Block ID: 32c3f7ef
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 符合规范 */
[[nodiscard]] int critical_function(void);
[[deprecated("Use new_function() instead")]] int old_function(void);
[[maybe_unused]] int debug_var = 0;
[[noreturn]] void fatal_error(const char* msg);
