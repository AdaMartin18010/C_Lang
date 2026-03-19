/*
 * Auto-generated from: 07_Modern_Toolchain\09_System_Architecture\README.md
 * Line: 773
 * Language: c
 * Block ID: d27c0cdd
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 前向声明与opaque pointer - 实现信息隐藏 */
typedef struct internal_system system_t;

/* 公开API */
system_t *system_create(const system_config_t *config);
void system_destroy(system_t *sys);
int system_start(system_t *sys);

/* 实现细节仅在.c文件中可见 */
struct internal_system {
    module_a_t *mod_a;
    module_b_t *mod_b;
    state_machine_t *fsm;
    /* ... */
};
