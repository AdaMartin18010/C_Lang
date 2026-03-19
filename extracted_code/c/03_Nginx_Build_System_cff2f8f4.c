/*
 * Auto-generated from: 07_Modern_Toolchain\05_Case_Studies\03_Nginx_Build_System.md
 * Line: 201
 * Language: c
 * Block ID: cff2f8f4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

static ngx_command_t ngx_http_static_commands[] = {
    { ngx_string("root"),
      NGX_HTTP_LOC_CONF|NGX_CONF_TAKE1,
      ngx_conf_set_str_slot,
      NGX_HTTP_LOC_CONF_OFFSET,
      offsetof(ngx_http_static_loc_conf_t, root),
      NULL },

    ngx_null_command
};
