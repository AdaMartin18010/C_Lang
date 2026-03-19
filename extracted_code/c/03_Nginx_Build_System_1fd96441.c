/*
 * Auto-generated from: 07_Modern_Toolchain\05_Case_Studies\03_Nginx_Build_System.md
 * Line: 268
 * Language: c
 * Block ID: 1fd96441
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 清晰的模块接口
// 每个模块自包含
// 支持静态和动态链接

ngx_module_t ngx_http_my_module = {
    NGX_MODULE_V1,
    &ngx_http_my_module_ctx,
    ngx_http_my_commands,
    NGX_HTTP_MODULE,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NGX_MODULE_V1_PADDING
};
