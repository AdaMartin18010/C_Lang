/*
 * Auto-generated from: 03_System_Technology_Domains\04_Video_Codec\03_Hardware_Acceleration.md
 * Line: 98
 * Language: c
 * Block ID: 252e9b5b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
