/*
 * Auto-generated from: 03_System_Technology_Domains\03_Computer_Vision\01_V4L2_Capture.md
 * Line: 460
 * Language: c
 * Block ID: cf540e85
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// VIDIOC_S_FMT可能返回不同的格式
// 总是检查实际应用的格式
struct v4l2_format fmt = {0};
fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
fmt.fmt.pix.width = 1920;
fmt.fmt.pix.height = 1080;
fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;

ioctl(fd, VIDIOC_S_FMT, &fmt);

// 检查实际格式
if (fmt.fmt.pix.pixelformat != V4L2_PIX_FMT_YUYV) {
    // 需要格式转换
}
if (fmt.fmt.pix.width != 1920) {
    // 驱动选择了不同的分辨率
}
