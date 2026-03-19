/*
 * Auto-generated from: 07_Modern_Toolchain\08_Design_Patterns\03_Behavioral_Patterns.md
 * Line: 83
 * Language: c
 * Block ID: 17edd756
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef void (*state_fn_t)(void* sm, int event);

typedef struct {
    state_fn_t current_state;
} state_machine_t;

void state_idle(state_machine_t* sm, int event) {
    if (event == 1) {
        printf("Starting...\n");
        sm->current_state = (state_fn_t)state_running;
    }
}

void state_running(state_machine_t* sm, int event) {
    if (event == 0) {
        printf("Stopping...\n");
        sm->current_state = (state_fn_t)state_idle;
    }
}

void sm_handle(state_machine_t* sm, int event) {
    sm->current_state(sm, event);
}
