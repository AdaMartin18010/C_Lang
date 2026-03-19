/*
 * Auto-generated from: 07_Modern_Toolchain\08_Design_Patterns\03_Behavioral_Patterns.md
 * Line: 113
 * Language: c
 * Block ID: bcb10c40
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    void (*execute)(void* cmd);
    void (*undo)(void* cmd);
    void (*destroy)(void* cmd);
} command_ops_t;

typedef struct {
    command_ops_t* ops;
} command_t;

/* 具体命令 */
typedef struct {
    command_ops_t ops;
    int* value;
    int new_val;
    int old_val;
} set_value_cmd_t;

void set_value_execute(void* cmd) {
    set_value_cmd_t* c = cmd;
    c->old_val = *c->value;
    *c->value = c->new_val;
}

void set_value_undo(void* cmd) {
    set_value_cmd_t* c = cmd;
    *c->value = c->old_val;
}

/* 命令队列 */
typedef struct {
    command_t** commands;
    int count;
    int capacity;
} command_queue_t;

void cmd_queue_push(command_queue_t* q, command_t* cmd) {
    if (q->count < q->capacity) {
        q->commands[q->count++] = cmd;
    }
}

void cmd_queue_execute_all(command_queue_t* q) {
    for (int i = 0; i < q->count; i++) {
        q->commands[i]->ops->execute(q->commands[i]);
    }
}
