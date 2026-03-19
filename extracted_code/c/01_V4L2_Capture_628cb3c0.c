/*
 * Auto-generated from: 03_System_Technology_Domains\03_Computer_Vision\01_V4L2_Capture.md
 * Line: 149
 * Language: c
 * Block ID: 628cb3c0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 枚举支持的格式
void v4l2_enum_formats(V4L2Context *ctx) {
    struct v4l2_fmtdesc fmtdesc = {0};
    fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    printf("Supported formats:\n");
    while (ioctl(ctx->fd, VIDIOC_ENUM_FMT, &fmtdesc) == 0) {
        printf("  %s (%.4s)\n",
               fmtdesc.description,
               (char*)&fmtdesc.pixelformat);
        fmtdesc.index++;
    }
}

// 设置视频格式
int v4l2_set_format(V4L2Context *ctx, int width, int height, uint32_t pixelformat) {
    struct v4l2_format fmt = {0};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = width;
    fmt.fmt.pix.height = height;
    fmt.fmt.pix.pixelformat = pixelformat;
    fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

    if (ioctl(ctx->fd, VIDIOC_S_FMT, &fmt) < 0) {
        perror("VIDIOC_S_FMT");
        return -1;
    }

    // 检查实际应用的格式
    if (fmt.fmt.pix.pixelformat != pixelformat) {
        printf("Requested format not supported, got: %.4s\n",
               (char*)&fmt.fmt.pix.pixelformat);
    }

    ctx->width = fmt.fmt.pix.width;
    ctx->height = fmt.fmt.pix.height;
    ctx->pixel_format = fmt.fmt.pix.pixelformat;
    ctx->fmt = fmt;

    printf("Format set: %dx%d, %.4s\n",
           ctx->width, ctx->height,
           (char*)&ctx->pixel_format);
    printf("Bytes per line: %d\n", fmt.fmt.pix.bytesperline);
    printf("Image size: %d\n", fmt.fmt.pix.sizeimage);

    return 0;
}

// 获取当前格式
int v4l2_get_format(V4L2Context *ctx) {
    struct v4l2_format fmt = {0};
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (ioctl(ctx->fd, VIDIOC_G_FMT, &fmt) < 0) {
        perror("VIDIOC_G_FMT");
        return -1;
    }

    ctx->width = fmt.fmt.pix.width;
    ctx->height = fmt.fmt.pix.height;
    ctx->pixel_format = fmt.fmt.pix.pixelformat;
    ctx->fmt = fmt;

    return 0;
}
