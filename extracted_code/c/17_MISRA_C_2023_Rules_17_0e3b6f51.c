/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\17_MISRA_C_2023_Rules_17.md
 * Line: 317
 * Language: c
 * Block ID: 0e3b6f51
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ✅ 合规 - 不透明类型 */
/* public.h */
struct InternalData;  /* 不透明类型 */
typedef struct InternalData *Handle;

Handle create_handle(void);
void destroy_handle(Handle h);
void use_handle(Handle h);

/* private.c */
#include "public.h"

struct InternalData {
    int secret_data;
    void *private_ptr;
};

Handle create_handle(void)
{
    Handle h = malloc(sizeof(*h));
    if (h) {
        h->secret_data = 0;
    }
    return h;
}
