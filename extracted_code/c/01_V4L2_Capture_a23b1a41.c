/*
 * Auto-generated from: 03_System_Technology_Domains\03_Computer_Vision\01_V4L2_Capture.md
 * Line: 219
 * Language: c
 * Block ID: a23b1a41
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 初始化MMAP缓冲区
int v4l2_init_mmap(V4L2Context *ctx) {
    struct v4l2_requestbuffers req = {0};
    req.count = BUFFER_COUNT;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (ioctl(ctx->fd, VIDIOC_REQBUFS, &req) < 0) {
        perror("VIDIOC_REQBUFS");
        return -1;
    }

    if (req.count < 2) {
        fprintf(stderr, "Insufficient buffer memory\n");
        return -1;
    }

    printf("Allocated %d buffers\n", req.count);

    // 映射每个缓冲区
    for (int i = 0; i < req.count; i++) {
        struct v4l2_buffer buf = {0};
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        if (ioctl(ctx->fd, VIDIOC_QUERYBUF, &buf) < 0) {
            perror("VIDIOC_QUERYBUF");
            return -1;
        }

        ctx->buffers[i].length = buf.length;
        ctx->buffers[i].start = mmap(NULL, buf.length,
                                      PROT_READ | PROT_WRITE,
                                      MAP_SHARED,
                                      ctx->fd, buf.m.offset);

        if (ctx->buffers[i].start == MAP_FAILED) {
            perror("mmap");
            return -1;
        }

        printf("Buffer %d: %p, size %zu\n", i,
               ctx->buffers[i].start, ctx->buffers[i].length);
    }

    return 0;
}

// 释放缓冲区
void v4l2_free_mmap(V4L2Context *ctx) {
    for (int i = 0; i < BUFFER_COUNT; i++) {
        if (ctx->buffers[i].start != NULL &&
            ctx->buffers[i].start != MAP_FAILED) {
            munmap(ctx->buffers[i].start, ctx->buffers[i].length);
        }
    }

    // 释放V4L2缓冲区
    struct v4l2_requestbuffers req = {0};
    req.count = 0;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;
    ioctl(ctx->fd, VIDIOC_REQBUFS, &req);
}
