/*
 * Auto-generated from: 07_Modern_Toolchain\05_Case_Studies\03_Nginx_Build_System.md
 * Line: 180
 * Language: c
 * Block ID: 50814f78
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#define NGX_CORE_MODULE      0x45524F43  /* CORE */
#define NGX_EVENT_MODULE     0x544E5645  /* EVNT */
#define NGX_HTTP_MODULE      0x50545448  /* HTTP */
#define NGX_MAIL_MODULE      0x4C49414D  /* MAIL */
#define NGX_STREAM_MODULE    0x4D525453  /* STRM */

/* HTTP 模块上下文 */
typedef struct {
    ngx_int_t   (*preconfiguration)(ngx_conf_t *cf);
    ngx_int_t   (*postconfiguration)(ngx_conf_t *cf);
    void       *(*create_main_conf)(ngx_conf_t *cf);
    void       *(*create_srv_conf)(ngx_conf_t *cf);
    char       *(*merge_srv_conf)(ngx_conf_t *cf, void *prev, void *conf);
    void       *(*create_loc_conf)(ngx_conf_t *cf);
    char       *(*merge_loc_conf)(ngx_conf_t *cf, void *prev, void *conf);
} ngx_http_module_t;
