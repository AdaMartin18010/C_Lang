/*
 * Auto-generated from: 07_Modern_Toolchain\08_Design_Patterns\README.md
 * Line: 582
 * Language: c
 * Block ID: 868944a4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 虚拟代理: 延迟加载大型资源
typedef struct {
    const char* (*get_image_data)(void* self);
    int (*get_width)(void* self);
    int (*get_height)(void* self);
} Image;

// 真实图像
typedef struct {
    Image interface;
    char* data;
    int width;
    int height;
} RealImage;

// 代理
typedef struct {
    Image interface;
    const char* filename;
    RealImage* real_image;  // 延迟加载
} ImageProxy;

static void ensure_loaded(ImageProxy* proxy) {
    if (!proxy->real_image) {
        proxy->real_image = real_image_load(proxy->filename);
    }
}

static const char* proxy_get_data(void* self) {
    ImageProxy* proxy = self;
    ensure_loaded(proxy);
    return proxy->real_image->interface.get_image_data(proxy->real_image);
}

static int proxy_get_width(void* self) {
    ImageProxy* proxy = self;
    // 元数据可以预先加载，不需要完整数据
    if (proxy->real_image) {
        return proxy->real_image->interface.get_width(proxy->real_image);
    }
    return image_metadata_get_width(proxy->filename);  // 只读元数据
}

// 使用场景: 图片列表，只有显示时才加载
void gallery_display(Image** images, int count, int visible_index) {
    for (int i = 0; i < count; i++) {
        if (i == visible_index) {
            // 这会触发实际加载
            printf("Displaying: %dx%d\n",
                   images[i]->get_width(images[i]),
                   images[i]->get_height(images[i]));
        }
        // 其他图片作为代理，不占用内存
    }
}
