/*
 * Auto-generated from: 03_System_Technology_Domains\04_Video_Codec\03_Hardware_Acceleration.md
 * Line: 297
 * Language: c
 * Block ID: 905ed897
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * H.264 NAL单元解码
 * 将压缩数据提交给硬件解码器
 */
int vaapi_decode_nal(decode_session_t *session,
                      const uint8_t *nal_data,
                      size_t nal_size,
                      int surface_idx)
{
    if (!session || !nal_data || nal_size == 0) {
        return -1;
    }

    vaapi_context_t *ctx = &session->va_ctx;

    // 创建图片参数缓冲区 (简化示例)
    VAPictureParameterBufferH264 pic_param = {0};
    pic_param.CurrPic.picture_id = ctx->surfaces[surface_idx];
    pic_param.CurrPic.frame_idx = 0;
    pic_param.pic_width_in_mbs_minus1 = (session->width / 16) - 1;
    pic_param.pic_height_in_map_units_minus1 = (session->height / 16) - 1;
    pic_param.bit_depth_luma_minus8 = 0;
    pic_param.bit_depth_chroma_minus8 = 0;

    VABufferID pic_buf;
    VAStatus status = vaCreateBuffer(
        ctx->display,
        ctx->context_id,
        VAPictureParameterBufferType,
        sizeof(pic_param), 1,
        &pic_param,
        &pic_buf
    );

    if (status != VA_STATUS_SUCCESS) {
        fprintf(stderr, "创建图片参数缓冲区失败\n");
        return -1;
    }

    // 创建IQ矩阵缓冲区 (简化)
    VAIQMatrixBufferH264 iq_matrix = {0};
    // 填充量化矩阵...

    VABufferID iq_buf;
    vaCreateBuffer(
        ctx->display,
        ctx->context_id,
        VAIQMatrixBufferType,
        sizeof(iq_matrix), 1,
        &iq_matrix,
        &iq_buf
    );

    // 创建切片参数和数据缓冲区
    VASliceParameterBufferH264 slice_param = {0};
    slice_param.slice_data_size = (uint32_t)nal_size;
    slice_param.slice_data_offset = 0;
    slice_param.slice_data_flag = VA_SLICE_DATA_FLAG_ALL;
    slice_param.slice_data_bit_offset = 0;
    slice_param.first_mb_in_slice = 0;

    VABufferID slice_buf;
    vaCreateBuffer(
        ctx->display,
        ctx->context_id,
        VASliceParameterBufferType,
        sizeof(slice_param), 1,
        &slice_param,
        &slice_buf
    );

    // 创建数据缓冲区
    VABufferID data_buf;
    vaCreateBuffer(
        ctx->display,
        ctx->context_id,
        VASliceDataBufferType,
        nal_size, 1,
        (void *)nal_data,
        &data_buf
    );

    // 开始解码帧
    status = vaBeginPicture(ctx->display, ctx->context_id,
                            ctx->surfaces[surface_idx]);
    if (status != VA_STATUS_SUCCESS) {
        fprintf(stderr, "BeginPicture失败\n");
        return -1;
    }

    // 渲染缓冲区
    vaRenderPicture(ctx->display, ctx->context_id, &pic_buf, 1);
    vaRenderPicture(ctx->display, ctx->context_id, &iq_buf, 1);
    vaRenderPicture(ctx->display, ctx->context_id, &slice_buf, 1);
    vaRenderPicture(ctx->display, ctx->context_id, &data_buf, 1);

    // 结束解码
    status = vaEndPicture(ctx->display, ctx->context_id);
    if (status != VA_STATUS_SUCCESS) {
        fprintf(stderr, "EndPicture失败\n");
        return -1;
    }

    return 0;
}

/**
 * 等待解码完成并获取输出表面
 */
int vaapi_get_output(decode_session_t *session,
                      int surface_idx,
                      uint8_t *y_plane,
                      uint8_t *uv_plane,
                      size_t y_pitch,
                      size_t uv_pitch)
{
    vaapi_context_t *ctx = &session->va_ctx;

    // 同步表面
    VAStatus status = vaSyncSurface(ctx->display, ctx->surfaces[surface_idx]);
    if (status != VA_STATUS_SUCCESS) {
        fprintf(stderr, "表面同步失败\n");
        return -1;
    }

    // 查询表面状态
    VASurfaceStatus surface_status;
    vaQuerySurfaceStatus(ctx->display, ctx->surfaces[surface_idx],
                         &surface_status);

    if (surface_status != VASurfaceReady) {
        fprintf(stderr, "表面未就绪\n");
        return -1;
    }

    // 导出DMA-BUF (零拷贝路径)
    VADRMPRIMESurfaceDescriptor desc = {0};
    status = vaExportSurfaceHandle(
        ctx->display,
        ctx->surfaces[surface_idx],
        VA_SURFACE_ATTRIB_MEM_TYPE_DRM_PRIME_2,
        VA_EXPORT_SURFACE_READ_ONLY | VA_EXPORT_SURFACE_SEPARATE_LAYERS,
        &desc
    );

    if (status == VA_STATUS_SUCCESS) {
        printf("成功导出DMA-BUF, 层数: %d\n", desc.num_layers);
        // 可以通过DMA-FD进行零拷贝渲染...
    }

    return 0;
}
