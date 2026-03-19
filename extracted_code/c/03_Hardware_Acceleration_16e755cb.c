/*
 * Auto-generated from: 03_System_Technology_Domains\04_Video_Codec\03_Hardware_Acceleration.md
 * Line: 184
 * Language: c
 * Block ID: 16e755cb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * 初始化H.264硬件解码器
 * 创建配置、上下文和表面缓冲区
 */
int vaapi_h264_decoder_init(decode_session_t *session,
                             int width, int height)
{
    if (!session || !session->va_ctx.display) {
        return -1;
    }

    session->width = width;
    session->height = height;
    session->format = VA_RT_FORMAT_YUV420;

    vaapi_context_t *ctx = &session->va_ctx;

    // 查询H.264解码支持的入口点
    VAEntrypoint entrypoints[5];
    int num_entrypoints;
    VAStatus status = vaQueryConfigEntrypoints(
        ctx->display,
        VAProfileH264High,
        entrypoints,
        &num_entrypoints
    );

    if (status != VA_STATUS_SUCCESS) {
        fprintf(stderr, "查询入口点失败\n");
        return -1;
    }

    // 查找VLD (Variable Length Decode) 入口点
    VAEntrypoint selected_entry = -1;
    for (int i = 0; i < num_entrypoints; i++) {
        if (entrypoints[i] == VAEntrypointVLD) {
            selected_entry = VAEntrypointVLD;
            break;
        }
    }

    if (selected_entry == -1) {
        fprintf(stderr, "不支持H.264 VLD解码\n");
        return -1;
    }

    // 创建配置
    VAConfigAttrib attrib;
    attrib.type = VAConfigAttribRTFormat;
    attrib.value = session->format;

    status = vaCreateConfig(
        ctx->display,
        VAProfileH264High,
        selected_entry,
        &attrib, 1,
        &ctx->config_id
    );

    if (status != VA_STATUS_SUCCESS) {
        fprintf(stderr, "创建配置失败: %s\n", vaErrorStr(status));
        return -1;
    }

    // 创建解码表面 (用于参考帧和输出)
    ctx->num_surfaces = 16;  // H.264需要多个参考帧
    ctx->surfaces = calloc(ctx->num_surfaces, sizeof(VASurfaceID));

    unsigned int fourcc = VA_FOURCC('N', 'V', '1', '2');
    VASurfaceAttrib surface_attrib = {
        .type = VASurfaceAttribPixelFormat,
        .flags = VA_SURFACE_ATTRIB_SETTABLE,
        .value = {.type = VAGenericValueTypeInteger, .value = {.i = fourcc}}
    };

    status = vaCreateSurfaces(
        ctx->display,
        session->format,
        width, height,
        ctx->surfaces,
        ctx->num_surfaces,
        &surface_attrib, 1
    );

    if (status != VA_STATUS_SUCCESS) {
        fprintf(stderr, "创建表面失败: %s\n", vaErrorStr(status));
        return -1;
    }

    // 创建解码上下文
    status = vaCreateContext(
        ctx->display,
        ctx->config_id,
        width, height,
        VA_PROGRESSIVE,  // 逐行扫描
        ctx->surfaces,
        ctx->num_surfaces,
        &ctx->context_id
    );

    if (status != VA_STATUS_SUCCESS) {
        fprintf(stderr, "创建上下文失败: %s\n", vaErrorStr(status));
        return -1;
    }

    printf("H.264解码器初始化成功: %dx%d\n", width, height);
    return 0;
}
