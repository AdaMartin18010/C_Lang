/*
 * Auto-generated from: 07_Modern_Toolchain\05_Case_Studies\03_Nginx_Build_System.md
 * Line: 162
 * Language: c
 * Block ID: df01ebf9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct ngx_module_s {
    ngx_uint_t    ctx_index;
    ngx_uint_t    index;
    char         *name;
    ngx_uint_t    version;
    void         *ctx;
    ngx_command_t *commands;
    ngx_uint_t    type;
    ngx_int_t   (*init_master)(ngx_log_t *log);
    ngx_int_t   (*init_module)(ngx_cycle_t *cycle);
    ngx_int_t   (*init_process)(ngx_cycle_t *cycle);
    void        (*exit_process)(ngx_cycle_t *cycle);
} ngx_module_t;
