/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\MISRA_C_2025_C23_Guidance.md
 * Line: 377
 * Language: c
 * Block ID: 092796f4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

#if __STDC_VERSION__ >= 202311L
    /* ✅ 推荐 [待官方确认] - 嵌入固件 */
    static const uint8_t firmware_data[] = {
        #embed "firmware.bin"
    };

    /* ✅ 推荐 [待官方确认] - 嵌入证书 */
    static const char root_ca_cert[] = {
        #embed "root_ca.pem"
        , '\0'  /* 添加终止符 */
    };

    /* ✅ 推荐 [待官方确认] - 带参数 */
    static const uint8_t image_data[] = {
        #embed "image.png" \
            limit(1024) \
            prefix(0xFF, 0xD8) \
            suffix(0xFF, 0xD9)
    };

    /* ✅ 推荐 [待官方确认] - 获取大小 */
    static const uint8_t data[] = {
        #embed "data.bin"
    };
    static const size_t data_size = sizeof(data);

#else
    /* C11/C17兼容 - 使用外部工具生成 */
    /* firmware_data.h - 由工具生成 */
    static const uint8_t firmware_data[] = {
        #include "firmware_data.h"
    };
    static const size_t firmware_size = sizeof(firmware_data);
#endif
