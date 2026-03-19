/*
 * Auto-generated from: 03_System_Technology_Domains\03_Computer_Vision\01_V4L2_Capture.md
 * Line: 416
 * Language: c
 * Block ID: efab67f5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 查询和设置控制参数
void v4l2_query_controls(V4L2Context *ctx) {
    struct v4l2_queryctrl queryctrl = {0};

    for (queryctrl.id = V4L2_CID_BASE;
         queryctrl.id < V4L2_CID_LASTP1;
         queryctrl.id++) {
        if (ioctl(ctx->fd, VIDIOC_QUERYCTRL, &queryctrl) == 0) {
            if (!(queryctrl.flags & V4L2_CTRL_FLAG_DISABLED)) {
                printf("Control: %s, min=%d, max=%d, default=%d\n",
                       queryctrl.name, queryctrl.minimum,
                       queryctrl.maximum, queryctrl.default_value);
            }
        }
    }
}

// 设置控制值
int v4l2_set_control(V4L2Context *ctx, uint32_t id, int value) {
    struct v4l2_control ctrl = {0};
    ctrl.id = id;
    ctrl.value = value;

    if (ioctl(ctx->fd, VIDIOC_S_CTRL, &ctrl) < 0) {
        perror("VIDIOC_S_CTRL");
        return -1;
    }

    return 0;
}

// 常用控制ID
#define SET_BRIGHTNESS(ctx, val) v4l2_set_control(ctx, V4L2_CID_BRIGHTNESS, val)
#define SET_CONTRAST(ctx, val) v4l2_set_control(ctx, V4L2_CID_CONTRAST, val)
#define SET_SATURATION(ctx, val) v4l2_set_control(ctx, V4L2_CID_SATURATION, val)
