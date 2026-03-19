/*
 * Auto-generated from: 03_System_Technology_Domains\03_Computer_Vision\01_V4L2_Capture.md
 * Line: 289
 * Language: c
 * Block ID: 4e703ce7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 启动视频流
int v4l2_start_streaming(V4L2Context *ctx) {
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    // 将所有缓冲区入队
    for (int i = 0; i < BUFFER_COUNT; i++) {
        struct v4l2_buffer buf = {0};
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        if (ioctl(ctx->fd, VIDIOC_QBUF, &buf) < 0) {
            perror("VIDIOC_QBUF");
            return -1;
        }
    }

    // 启动流
    if (ioctl(ctx->fd, VIDIOC_STREAMON, &type) < 0) {
        perror("VIDIOC_STREAMON");
        return -1;
    }

    printf("Streaming started\n");
    return 0;
}

// 停止视频流
int v4l2_stop_streaming(V4L2Context *ctx) {
    enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (ioctl(ctx->fd, VIDIOC_STREAMOFF, &type) < 0) {
        perror("VIDIOC_STREAMOFF");
        return -1;
    }

    printf("Streaming stopped\n");
    return 0;
}

// 捕获一帧
int v4l2_capture_frame(V4L2Context *ctx, void **data, size_t *size) {
    struct v4l2_buffer buf = {0};
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_MMAP;

    // 出队缓冲区
    if (ioctl(ctx->fd, VIDIOC_DQBUF, &buf) < 0) {
        if (errno == EAGAIN) {
            return -1;  // 无数据
        }
        perror("VIDIOC_DQBUF");
        return -1;
    }

    // 返回数据
    *data = ctx->buffers[buf.index].start;
    *size = buf.bytesused;

    uint64_t timestamp = (uint64_t)buf.timestamp.tv_sec * 1000000 +
                         buf.timestamp.tv_usec;
    printf("Frame %d: %zu bytes, timestamp %lu\n",
           buf.sequence, *size, timestamp);

    // 重新入队
    if (ioctl(ctx->fd, VIDIOC_QBUF, &buf) < 0) {
        perror("VIDIOC_QBUF");
        return -1;
    }

    return 0;
}
