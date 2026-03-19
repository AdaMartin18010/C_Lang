/*
 * Auto-generated from: 07_Modern_Toolchain\08_Design_Patterns\02_Structural_Patterns.md
 * Line: 204
 * Language: c
 * Block ID: 50329154
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    char* filename;
    void* data;
    size_t size;
    bool loaded;
} image_t;

typedef struct {
    image_t* (*get_image)(void*);
} image_proxy_t;

typedef struct {
    image_proxy_t interface;
    char* filename;
    image_t* real_image;
} virtual_image_proxy_t;

image_t* load_image(const char* filename) {
    image_t* img = malloc(sizeof(image_t));
    /* 实际加载文件 */
    FILE* f = fopen(filename, "rb");
    fseek(f, 0, SEEK_END);
    img->size = ftell(f);
    img->data = malloc(img->size);
    rewind(f);
    fread(img->data, 1, img->size, f);
    fclose(f);
    img->loaded = true;
    return img;
}

image_t* virtual_proxy_get_image(void* self) {
    virtual_image_proxy_t* proxy = self;
    if (proxy->real_image == NULL) {
        printf("Lazy loading: %s\n", proxy->filename);
        proxy->real_image = load_image(proxy->filename);
    }
    return proxy->real_image;
}

/* 使用 */
void gallery(void) {
    /* 创建大量图片代理，实际不加载 */
    image_proxy_t* images[1000];
    for (int i = 0; i < 1000; i++) {
        images[i] = virtual_proxy_create(filenames[i]);
    }

    /* 只加载可见的图片 */
    image_t* visible = images[0]->get_image(images[0]);
}
