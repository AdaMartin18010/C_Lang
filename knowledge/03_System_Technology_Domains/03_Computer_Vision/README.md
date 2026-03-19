# 计算机视觉

> **层级定位**: 03_System_Technology_Domains > 03_Computer_Vision

---

## 内容

- [V4L2视频捕获](./01_V4L2_Capture.md)
- [光流算法](./02_Optical_Flow.md)
- [边缘检测](./03_Edge_Detection.md)

---

## V4L2基础

Video4Linux2是Linux视频捕获框架。

```c
#include <linux/videodev2.h>

// 打开设备
int fd = open("/dev/video0", O_RDWR);

// 查询能力
struct v4l2_capability cap;
ioctl(fd, VIDIOC_QUERYCAP, &cap);
```

---

> **最后更新**: 2026-03-21
