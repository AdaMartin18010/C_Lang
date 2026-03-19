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


---

## 深入理解

### 核心原理

深入探讨技术原理和实现细节。

### 实践应用

- 应用场景1
- 应用场景2
- 应用场景3

### 最佳实践

1. 理解基础概念
2. 掌握核心机制
3. 应用到实际项目

---

> **最后更新**: 2026-03-21  
> **维护者**: AI Code Review
