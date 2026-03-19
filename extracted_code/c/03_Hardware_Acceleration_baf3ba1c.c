/*
 * Auto-generated from: 03_System_Technology_Domains\04_Video_Codec\03_Hardware_Acceleration.md
 * Line: 658
 * Language: c
 * Block ID: baf3ba1c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 清理解码器资源
 */
void vaapi_decoder_cleanup(decode_session_t *session)
{
    if (!session) return;

    vaapi_context_t *ctx = &session->va_ctx;

    if (ctx->context_id != VA_INVALID_ID) {
        vaDestroyContext(ctx->display, ctx->context_id);
    }

    if (ctx->surfaces) {
        vaDestroySurfaces(ctx->display, ctx->surfaces, ctx->num_surfaces);
        free(ctx->surfaces);
    }

    if (ctx->config_id != VA_INVALID_ID) {
        vaDestroyConfig(ctx->display, ctx->config_id);
    }

    vaTerminate(ctx->display);
    memset(session, 0, sizeof(*session));
}

// 编译: gcc -o vaapi_demo vaapi_demo.c -lva -lva-drm
