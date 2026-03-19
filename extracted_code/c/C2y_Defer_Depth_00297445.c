/*
 * Auto-generated from: 01_Core_Knowledge_System\07_Modern_C\C2y_Defer_Depth.md
 * Line: 3489
 * Language: c
 * Block ID: 00297445
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

volatile sig_atomic_t signal_received = 0;

void signal_handler(int sig) {
    signal_received = sig;
}

void main_loop(void) {
    defer cleanup_all();

    while (!signal_received) {
        // 处理...
    }
}
