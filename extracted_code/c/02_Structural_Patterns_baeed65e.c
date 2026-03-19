/*
 * Auto-generated from: 07_Modern_Toolchain\08_Design_Patterns\02_Structural_Patterns.md
 * Line: 524
 * Language: c
 * Block ID: baeed65e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 内部状态 (共享) */
typedef struct {
    char* texture_data;
    size_t texture_size;
    int refcount;
} shared_texture_t;

/* 享元工厂 */
typedef struct {
    shared_texture_t** textures;
    int count;
} texture_factory_t;

shared_texture_t* get_texture(texture_factory_t* factory, const char* name) {
    /* 查找已有 */
    for (int i = 0; i < factory->count; i++) {
        if (strcmp(factory->textures[i]->name, name) == 0) {
            factory->textures[i]->refcount++;
            return factory->textures[i];
        }
    }

    /* 加载新纹理 */
    shared_texture_t* tex = malloc(sizeof(shared_texture_t));
    tex->texture_data = load_texture_file(name);
    tex->refcount = 1;

    factory->textures[factory->count++] = tex;
    return tex;
}

/* 外部状态 (不共享) */
typedef struct {
    shared_texture_t* texture;  /* 内部状态 */
    int x, y;                   /* 外部状态 */
    float scale, rotation;
} sprite_t;

/* 渲染大量精灵 */
void render_sprites(sprite_t* sprites, int count) {
    for (int i = 0; i < count; i++) {
        /* 复用相同的纹理 */
        draw_texture(sprites[i].texture->texture_data,
                     sprites[i].x, sprites[i].y,
                     sprites[i].scale, sprites[i].rotation);
    }
}
