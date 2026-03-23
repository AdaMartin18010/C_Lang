---

## 🔗 文档关联

### 核心关联
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [内存管理](../../../01_Core_Knowledge_System/02_Core_Layer/02_Memory_Management.md) | 核心关联 | 内存管理基础 |
| [指针深度](../../../01_Core_Knowledge_System/02_Core_Layer/01_Pointer_Depth.md) | 核心关联 | 指针深度基础 |
| [并发编程](../../../03_System_Technology_Domains/14_Concurrency_Parallelism/README.md) | 核心关联 | 并发编程基础 |
| [数据类型](../../../01_Core_Knowledge_System/01_Basic_Layer/02_Data_Type_System.md) | 核心关联 | 数据类型基础 |
| [数组与指针](../../../01_Core_Knowledge_System/02_Core_Layer/05_Arrays_Pointers.md) | 核心关联 | 数组与指针基础 |

### 扩展阅读
| 文档 | 关系类型 | 说明 |
|:-----|:---------|:-----|
| [软件工程](../../../01_Core_Knowledge_System/05_Engineering_Layer/README.md) | 核心关联 | 软件工程基础 |
| [形式语义](../../../02_Formal_Semantics_and_Physics/README.md) | 核心关联 | 形式语义基础 |
| [系统技术](../../../03_System_Technology_Domains/README.md) | 核心关联 | 系统技术基础 |
| [工业场景](../../../04_Industrial_Scenarios/README.md) | 核心关联 | 工业场景基础 |
| [思维表征](../../../06_Thinking_Representation/README.md) | 核心关联 | 思维表征基础 |
﻿# 视频编解码硬件加速 (VAAPI/VDPAU)


---

## 📑 目录

- [视频编解码硬件加速 (VAAPI/VDPAU)](#视频编解码硬件加速-vaapivdpau)
  - [📑 目录](#-目录)
  - [1. Linux视频加速架构概述](#1-linux视频加速架构概述)
    - [1.1 架构层次](#11-架构层次)
    - [1.2 VAAPI vs VDPAU](#12-vaapi-vs-vdpau)
  - [2. VAAPI核心接口](#2-vaapi核心接口)
    - [2.1 主要数据结构](#21-主要数据结构)
    - [2.2 初始化和配置](#22-初始化和配置)
  - [3. H.264硬件解码实现](#3-h264硬件解码实现)
    - [3.1 解码器初始化](#31-解码器初始化)
    - [3.2 解码流程](#32-解码流程)
  - [4. 零拷贝渲染](#4-零拷贝渲染)
    - [4.1 EGL + DMA-BUF集成](#41-egl--dma-buf集成)
  - [5. 性能对比](#5-性能对比)
    - [5.1 性能测试代码](#51-性能测试代码)
    - [5.2 典型性能数据](#52-典型性能数据)
  - [6. 清理资源](#6-清理资源)
  - [7. 总结](#7-总结)
  - [深入理解](#深入理解)
    - [核心原理](#核心原理)
    - [实践应用](#实践应用)
    - [最佳实践](#最佳实践)


---

## 1. Linux视频加速架构概述

Linux平台上的视频硬件加速主要通过两种接口实现：VAAPI (Video Acceleration API) 和 VDPAU (Video Decode and Presentation API for Unix)。
这些接口允许应用程序利用GPU的硬件解码能力，显著降低CPU负载。

### 1.1 架构层次

```text
┌─────────────────────────────────────────────────────────┐
│              应用程序 (FFmpeg/VLC/MPV)                   │
├─────────────────────────────────────────────────────────┤
│              VAAPI / VDPAU 接口层                        │
├─────────────────────────────────────────────────────────┤
│         驱动层 (i965, nouveau, amdgpu)                   │
├─────────────────────────────────────────────────────────┤
│              DRM/KMS 内核层                              │
├─────────────────────────────────────────────────────────┤
│              GPU 硬件层                                  │
└─────────────────────────────────────────────────────────┘
```

### 1.2 VAAPI vs VDPAU

| 特性 | VAAPI | VDPAU |
|------|-------|-------|
| 发起方 | Intel | NVIDIA |
| 主要用户 | Intel/AMD | NVIDIA |
| 活跃状态 | 活跃开发 | 维护模式 |
| Wayland支持 | 良好 | 有限 |

---

## 2. VAAPI核心接口

### 2.1 主要数据结构

```c
#include <va/va.h>
#include <va/va_drm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

// VAAPI上下文结构
typedef struct {
    VADisplay       display;        // VADisplay句柄
    VAConfigID      config_id;      // 配置ID
    VAContextID     context_id;     // 上下文ID
    VASurfaceID     *surfaces;      // 表面数组
    int             num_surfaces;   // 表面数量
    VABufferID      *pic_buffers;   // 图片参数缓冲区
    int             pic_buf_count;  // 缓冲区计数
} vaapi_context_t;

// 解码会话状态
typedef struct {
    int             width;          // 视频宽度
    int             height;         // 视频高度
    int             format;         // 像素格式
    int             codec;          // 编解码器类型
    vaapi_context_t va_ctx;         // VAAPI上下文
} decode_session_t;
```

### 2.2 初始化和配置

```c
/**
 * 初始化VAAPI显示
 * 通过DRM设备打开VAAPI显示连接
 */
VADisplay vaapi_init_display(const char *drm_device)
{
    int fd = open(drm_device, O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "无法打开DRM设备: %s\n", drm_device);
        return NULL;
    }

    VADisplay disp = vaGetDisplayDRM(fd);
    if (!disp) {
        fprintf(stderr, "无法获取VADisplay\n");
        close(fd);
        return NULL;
    }

    int major, minor;
    VAStatus status = vaInitialize(disp, &major, &minor);
    if (status != VA_STATUS_SUCCESS) {
        fprintf(stderr, "VAAPI初始化失败: %s\n", vaErrorStr(status));
        close(fd);
        return NULL;
    }

    printf("VAAPI版本: %d.%d\n", major, minor);
    return disp;
}

/**
 * 查询并显示硬件支持的配置文件
 */
void vaapi_list_profiles(VADisplay disp)
{
    VAEntrypoint *entrypoints = NULL;
    int num_entrypoints;
    VAProfile *profiles = NULL;
    int num_profiles;

    VAStatus status = vaQueryConfigProfiles(disp, NULL, &num_profiles);
    if (status != VA_STATUS_SUCCESS) {
        return;
    }

    profiles = malloc(num_profiles * sizeof(VAProfile));
    vaQueryConfigProfiles(disp, profiles, &num_profiles);

    printf("\n支持的编解码配置文件:\n");
    for (int i = 0; i < num_profiles; i++) {
        const char *profile_name = "Unknown";
        switch (profiles[i]) {
            case VAProfileH264ConstrainedBaseline:
                profile_name = "H.264 Constrained Baseline"; break;
            case VAProfileH264Main:
                profile_name = "H.264 Main"; break;
            case VAProfileH264High:
                profile_name = "H.264 High"; break;
            case VAProfileHEVCMain:
                profile_name = "HEVC Main"; break;
            case VAProfileHEVCMain10:
                profile_name = "HEVC Main 10"; break;
            case VAProfileVP9Profile0:
                profile_name = "VP9 Profile 0"; break;
            case VAProfileVP9Profile2:
                profile_name = "VP9 Profile 2"; break;
            case VAProfileAV1Profile0:
                profile_name = "AV1 Profile 0"; break;
            default:
                break;
        }
        printf("  [%d] %s\n", profiles[i], profile_name);
    }

    free(profiles);
}
```

---

## 3. H.264硬件解码实现

### 3.1 解码器初始化

```c
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
```

### 3.2 解码流程

```c
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
```

---

## 4. 零拷贝渲染

### 4.1 EGL + DMA-BUF集成

```c
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GL/gl.h>

/**
 * 零拷贝渲染设置
 * 将VAAPI解码的表面直接绑定到OpenGL纹理
 */
typedef struct {
    EGLDisplay      egl_display;
    EGLContext      egl_context;
    PFNEGLCREATEIMAGEKHRPROC        eglCreateImageKHR;
    PFNEGLDESTROYIMAGEKHRPROC       eglDestroyImageKHR;
    PFNGLEGLIMAGETARGETTEXTURE2DOESPROC glEGLImageTargetTexture2DOES;
} egl_zero_copy_t;

/**
 * 创建EGL图像从DMA-BUF
 */
EGLImageKHR create_egl_image_from_dmabuf(egl_zero_copy_t *egl,
                                          int dmabuf_fd,
                                          int width, int height,
                                          int fourcc)
{
    EGLint attribs[] = {
        EGL_WIDTH, width,
        EGL_HEIGHT, height,
        EGL_LINUX_DRM_FOURCC_EXT, fourcc,
        EGL_DMA_BUF_PLANE0_FD_EXT, dmabuf_fd,
        EGL_DMA_BUF_PLANE0_OFFSET_EXT, 0,
        EGL_DMA_BUF_PLANE0_PITCH_EXT, width,
        EGL_NONE
    };

    EGLImageKHR image = egl->eglCreateImageKHR(
        egl->egl_display,
        EGL_NO_CONTEXT,
        EGL_LINUX_DMA_BUF_EXT,
        NULL,
        attribs
    );

    return image;
}

/**
 * 将VAAPI表面转换为OpenGL纹理
 */
GLuint vaapi_surface_to_texture(decode_session_t *session,
                                 egl_zero_copy_t *egl,
                                 int surface_idx)
{
    vaapi_context_t *ctx = &session->va_ctx;

    // 导出为DMA-BUF
    VADRMPRIMESurfaceDescriptor desc = {0};
    VAStatus status = vaExportSurfaceHandle(
        ctx->display,
        ctx->surfaces[surface_idx],
        VA_SURFACE_ATTRIB_MEM_TYPE_DRM_PRIME_2,
        VA_EXPORT_SURFACE_READ_ONLY,
        &desc
    );

    if (status != VA_STATUS_SUCCESS) {
        return 0;
    }

    // 创建EGL图像
    EGLImageKHR image = create_egl_image_from_dmabuf(
        egl,
        desc.objects[0].fd,
        desc.width, desc.height,
        desc.layers[0].drm_format
    );

    // 创建纹理并绑定
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_EXTERNAL_OES, texture);

    egl->glEGLImageTargetTexture2DOES(GL_TEXTURE_EXTERNAL_OES, image);

    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_EXTERNAL_OES, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 清理EGL图像
    egl->eglDestroyImageKHR(egl->egl_display, image);
    close(desc.objects[0].fd);

    return texture;
}
```

---

## 5. 性能对比

### 5.1 性能测试代码

```c
#include <time.h>
#include <sys/time.h>

typedef struct {
    double cpu_time_ms;
    double gpu_time_ms;
    double cpu_usage_percent;
    double gpu_usage_percent;
    size_t frames_decoded;
    double throughput_fps;
} perf_result_t;

/**
 * 测量CPU软解码性能
 */
double benchmark_cpu_decode(const uint8_t *data, size_t size,
                            int width, int height,
                            int num_frames)
{
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    // 模拟CPU解码工作负载
    for (int i = 0; i < num_frames; i++) {
        // 此处应调用软解码器如libavcodec
        volatile int sum = 0;
        for (size_t j = 0; j < size / 100; j++) {
            sum += data[j % size];
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed = (end.tv_sec - start.tv_sec) * 1000.0 +
                     (end.tv_nsec - start.tv_nsec) / 1000000.0;

    return elapsed;
}

/**
 * 测量GPU硬解码性能
 */
double benchmark_gpu_decode(decode_session_t *session,
                            const uint8_t *data, size_t size,
                            int num_frames)
{
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < num_frames; i++) {
        int surface_idx = i % session->va_ctx.num_surfaces;
        vaapi_decode_nal(session, data, size, surface_idx);
        vaSyncSurface(session->va_ctx.display,
                      session->va_ctx.surfaces[surface_idx]);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed = (end.tv_sec - start.tv_sec) * 1000.0 +
                     (end.tv_nsec - start.tv_nsec) / 1000000.0;

    return elapsed;
}

/**
 * 打印性能对比报告
 */
void print_performance_report(perf_result_t *result)
{
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║           视频解码性能对比报告                            ║\n");
    printf("╠══════════════════════════════════════════════════════════╣\n");
    printf("║ 解码帧数:        %6zu 帧                               ║\n",
           result->frames_decoded);
    printf("║                                                         ║\n");
    printf("║ CPU 软解码:      %8.2f ms  (%5.1f%% 占用率)            ║\n",
           result->cpu_time_ms, result->cpu_usage_percent);
    printf("║ GPU 硬解码:      %8.2f ms  (%5.1f%% 占用率)            ║\n",
           result->gpu_time_ms, result->gpu_usage_percent);
    printf("║                                                         ║\n");
    printf("║ 加速比:          %8.2fx                               ║\n",
           result->cpu_time_ms / result->gpu_time_ms);
    printf("║ 吞吐量:          %8.2f FPS                           ║\n",
           result->throughput_fps);
    printf("╚══════════════════════════════════════════════════════════╝\n");
}
```

### 5.2 典型性能数据

| 分辨率 | CPU解码 (ms/帧) | GPU解码 (ms/帧) | 加速比 |
|--------|-----------------|-----------------|--------|
| 720p   | 8-12           | 1-2            | 6-8x   |
| 1080p  | 20-30          | 2-3            | 8-12x  |
| 4K     | 80-120         | 5-8            | 15-20x |

---

## 6. 清理资源

```c
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
```

---

## 7. 总结

VAAPI和VDPAU为Linux平台提供了强大的视频硬件加速能力。关键要点：

1. **VAAPI** 是现代Linux系统的首选，支持Intel和AMD GPU
2. **零拷贝渲染** 通过DMA-BUF避免CPU-GPU间不必要的数据拷贝
3. **硬件解码** 可提供10-20倍的性能提升，显著降低功耗
4. **EGL集成** 允许将解码表面直接用作OpenGL纹理

实际应用中，建议使用FFmpeg的hwaccel接口，它会自动处理VAAPI的复杂细节。


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
