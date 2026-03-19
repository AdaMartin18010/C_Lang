# C语言结构型设计模式

## 目录

- [C语言结构型设计模式](#c语言结构型设计模式)
  - [目录](#目录)
  - [适配器模式](#适配器模式)
    - [对象适配器](#对象适配器)
    - [函数适配器](#函数适配器)
  - [装饰器模式](#装饰器模式)
    - [动态装饰器](#动态装饰器)
  - [代理模式](#代理模式)
    - [虚拟代理 (延迟加载)](#虚拟代理-延迟加载)
    - [保护代理 (访问控制)](#保护代理-访问控制)
    - [远程代理 (RPC)](#远程代理-rpc)
  - [外观模式](#外观模式)
  - [桥接模式](#桥接模式)
  - [组合模式](#组合模式)
  - [享元模式](#享元模式)
  - [总结](#总结)
  - [参考](#参考)

---

## 适配器模式

### 对象适配器

```c
/* 目标接口 */
typedef struct {
    void (*request)(void*);
} target_t;

/* 被适配者 */
typedef struct {
    void (*specific_request)(void*);
} adaptee_t;

/* 适配器 */
typedef struct {
    target_t target;
    adaptee_t* adaptee;
} adapter_t;

void adapter_request(void* self) {
    adapter_t* adapter = self;
    /* 转换调用 */
    printf("Adapter: converting call\n");
    adapter->adaptee->specific_request(adapter->adaptee);
}

adapter_t* adapter_create(adaptee_t* adaptee) {
    adapter_t* adapter = malloc(sizeof(adapter_t));
    adapter->target.request = adapter_request;
    adapter->adaptee = adaptee;
    return adapter;
}

/* 使用 */
void example_adapter(void) {
    adaptee_t* legacy = adaptee_create();
    target_t* target = (target_t*)adapter_create(legacy);
    target->request(target);  /* 通过适配器调用 */
}
```

### 函数适配器

```c
/* 将一种函数签名适配为另一种 */

typedef void (*callback_t)(int result, void* user_data);
typedef void (*legacy_callback_t)(int result);

typedef struct {
    legacy_callback_t legacy_fn;
    callback_t new_fn;
    void* user_data;
} callback_adapter_t;

void adapted_callback(int result) {
    /* 获取上下文 */
    callback_adapter_t* ctx = get_thread_local_context();
    ctx->new_fn(result, ctx->user_data);
}

/* 适配器函数 */
void call_with_adapter(legacy_callback_t legacy, callback_t modern,
                       void* user_data) {
    callback_adapter_t ctx = {
        .legacy_fn = legacy,
        .new_fn = modern,
        .user_data = user_data
    };
    set_thread_local_context(&ctx);
    legacy(adapted_callback);
}
```

---

## 装饰器模式

### 动态装饰器

```c
/* 组件接口 */
typedef struct {
    void (*write)(void*, const char*, size_t);
    void (*flush)(void*);
} writer_t;

/* 基础实现: 文件写入 */
typedef struct {
    writer_t interface;
    FILE* file;
} file_writer_t;

void file_write(void* self, const char* data, size_t len) {
    file_writer_t* fw = self;
    fwrite(data, 1, len, fw->file);
}

/* 装饰器: 缓冲 */
typedef struct {
    writer_t interface;
    writer_t* wrapped;
    char buffer[4096];
    size_t buffered;
} buffered_writer_t;

void buffered_write(void* self, const char* data, size_t len) {
    buffered_writer_t* bw = self;

    /* 尝试放入缓冲区 */
    if (bw->buffered + len < sizeof(bw->buffer)) {
        memcpy(bw->buffer + bw->buffered, data, len);
        bw->buffered += len;
    } else {
        /* 刷新缓冲区 */
        bw->wrapped->write(bw->wrapped, bw->buffer, bw->buffered);
        bw->buffered = 0;

        /* 写入新数据 */
        if (len < sizeof(bw->buffer)) {
            memcpy(bw->buffer, data, len);
            bw->buffered = len;
        } else {
            bw->wrapped->write(bw->wrapped, data, len);
        }
    }
}

void buffered_flush(void* self) {
    buffered_writer_t* bw = self;
    if (bw->buffered > 0) {
        bw->wrapped->write(bw->wrapped, bw->buffer, bw->buffered);
        bw->buffered = 0;
    }
    bw->wrapped->flush(bw->wrapped);
}

writer_t* buffer_decorator_create(writer_t* wrapped) {
    buffered_writer_t* bw = malloc(sizeof(buffered_writer_t));
    bw->interface.write = buffered_write;
    bw->interface.flush = buffered_flush;
    bw->wrapped = wrapped;
    bw->buffered = 0;
    return (writer_t*)bw;
}

/* 装饰器: 压缩 */
typedef struct {
    writer_t interface;
    writer_t* wrapped;
    z_stream zstream;
} compressed_writer_t;

void compressed_write(void* self, const char* data, size_t len) {
    compressed_writer_t* cw = self;
    /* 压缩并写入 */
    deflate(&cw->zstream, Z_NO_FLUSH);
    /* ... */
}

/* 链式装饰: File -> Buffer -> Compress */
void example_decorator(void) {
    writer_t* file = file_writer_create("output.txt");
    writer_t* buffered = buffer_decorator_create(file);
    writer_t* compressed = compress_decorator_create(buffered);

    compressed->write(compressed, "Hello", 5);
    compressed->flush(compressed);
}
```

---

## 代理模式

### 虚拟代理 (延迟加载)

```c
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
```

### 保护代理 (访问控制)

```c
typedef struct {
    void (*read)(void*, char*, size_t);
    void (*write)(void*, const char*, size_t);
    void (*delete)(void*);
} document_ops_t;

typedef struct {
    document_ops_t ops;
    void* real_doc;
    int user_permission;
} document_proxy_t;

void protected_write(void* self, const char* data, size_t len) {
    document_proxy_t* proxy = self;
    if (!(proxy->user_permission & PERM_WRITE)) {
        printf("Access denied: no write permission\n");
        return;
    }
    /* 转发到真实对象 */
    ((document_ops_t*)proxy->real_doc)->write(proxy->real_doc, data, len);
}
```

### 远程代理 (RPC)

```c
/* 本地接口 */
typedef struct {
    int (*add)(void*, int, int);
    int (*multiply)(void*, int, int);
} calculator_t;

/* 远程代理 */
typedef struct {
    calculator_t interface;
    int socket_fd;
    char server_addr[64];
    int port;
} remote_calculator_t;

int remote_add(void* self, int a, int b) {
    remote_calculator_t* proxy = self;

    /* 序列化请求 */
    rpc_request_t req = {
        .method = "add",
        .arg1 = a,
        .arg2 = b
    };
    send(proxy->socket_fd, &req, sizeof(req), 0);

    /* 接收响应 */
    rpc_response_t resp;
    recv(proxy->socket_fd, &resp, sizeof(resp), 0);
    return resp.result;
}

/* 调用方完全透明 */
void use_calculator(calculator_t* calc) {
    int result = calc->add(calc, 2, 3);  /* 不知道是本地还是远程 */
}
```

---

## 外观模式

```c
/* 复杂子系统 */
typedef struct { /* compiler frontend */ } lexer_t;
typedef struct { /* parser */ } parser_t;
typedef struct { /* semantic analyzer */ } semantic_t;
typedef struct { /* code generator */ } codegen_t;

typedef struct {
    lexer_t* lexer;
    parser_t* parser;
    semantic_t* semantic;
    codegen_t* codegen;
} compiler_subsystem_t;

/* 外观: 简化接口 */
typedef struct {
    compiler_subsystem_t* subsys;
} compiler_facade_t;

compiler_facade_t* compiler_new(void) {
    compiler_facade_t* f = malloc(sizeof(compiler_facade_t));
    f->subsys = malloc(sizeof(compiler_subsystem_t));
    f->subsys->lexer = lexer_create();
    f->subsys->parser = parser_create();
    f->subsys->semantic = semantic_create();
    f->subsys->codegen = codegen_create();
    return f;
}

bool compile_file(compiler_facade_t* compiler, const char* source,
                  const char* output) {
    /* 隐藏复杂流程 */
    tokens_t* tokens = lexer_tokenize(compiler->subsys->lexer, source);
    ast_t* ast = parser_parse(compiler->subsys->parser, tokens);

    if (!semantic_check(compiler->subsys->semantic, ast)) {
        return false;
    }

    ir_t* ir = semantic_to_ir(compiler->subsys->semantic, ast);
    codegen_generate(compiler->subsys->codegen, ir, output);

    return true;
}

/* 使用 */
void example_facade(void) {
    compiler_facade_t* c = compiler_new();
    compile_file(c, "main.c", "main.o");
}
```

---

## 桥接模式

```c
/* 实现接口 */
typedef struct {
    void (*draw_line)(void*, int, int, int, int);
    void (*draw_circle)(void*, int, int, int);
    void (*render)(void*);
} renderer_impl_t;

/* 抽象 */
typedef struct {
    renderer_impl_t* impl;
    void (*draw_shape)(void*);
} shape_t;

/* 细化抽象: 圆形 */
typedef struct {
    shape_t base;
    int x, y, radius;
} circle_shape_t;

void circle_draw(void* self) {
    circle_shape_t* circle = self;
    circle->base.impl->draw_circle(circle->base.impl,
                                   circle->x, circle->y, circle->radius);
}

/* 实现: OpenGL */
typedef struct {
    renderer_impl_t interface;
    GLuint program;
} opengl_renderer_t;

void opengl_draw_circle(void* self, int x, int y, int r) {
    opengl_renderer_t* gl = self;
    /* OpenGL绘制 */
}

/* 实现: DirectX */
typedef struct {
    renderer_impl_t interface;
    ID3D11Device* device;
} d3d_renderer_t;

void d3d_draw_circle(void* self, int x, int y, int r) {
    d3d_renderer_t* d3d = self;
    /* DirectX绘制 */
}

/* 桥接: 抽象与实现分离 */
void example_bridge(void) {
    /* 可以用OpenGL或DirectX渲染相同的形状 */
    renderer_impl_t* gl = opengl_renderer_create();
    renderer_impl_t* d3d = d3d_renderer_create();

    circle_shape_t* circle = circle_create(100, 100, 50);

    circle->base.impl = gl;
    circle->base.draw_shape(circle);  /* OpenGL渲染 */

    circle->base.impl = d3d;
    circle->base.draw_shape(circle);  /* DirectX渲染 */
}
```

---

## 组合模式

```c
/* 组件接口 */
typedef struct component {
    struct component* parent;
    void (*render)(struct component*);
    void (*add)(struct component*, struct component*);
    void (*remove)(struct component*, struct component*);
} component_t;

/* 叶子: 按钮 */
typedef struct {
    component_t base;
    char label[64];
} button_t;

void button_render(component_t* self) {
    button_t* btn = (button_t*)self;
    printf("Button: %s\n", btn->label);
}

/* 组合: 面板 */
typedef struct {
    component_t base;
    component_t** children;
    int child_count;
    int capacity;
} panel_t;

void panel_render(component_t* self) {
    panel_t* panel = (panel_t*)self;
    printf("Panel {\n");
    for (int i = 0; i < panel->child_count; i++) {
        panel->children[i]->render(panel->children[i]);
    }
    printf("}\n");
}

void panel_add(component_t* self, component_t* child) {
    panel_t* panel = (panel_t*)self;
    if (panel->child_count >= panel->capacity) {
        panel->capacity *= 2;
        panel->children = realloc(panel->children,
                                  sizeof(component_t*) * panel->capacity);
    }
    panel->children[panel->child_count++] = child;
    child->parent = self;
}

/* 使用 */
void example_composite(void) {
    panel_t* root = panel_create("root");
    panel_t* sidebar = panel_create("sidebar");
    panel_t* main = panel_create("main");

    button_t* btn1 = button_create("Submit");
    button_t* btn2 = button_create("Cancel");

    sidebar->base.add((component_t*)sidebar, (component_t*)btn1);
    main->base.add((component_t*)main, (component_t*)btn2);

    root->base.add((component_t*)root, (component_t*)sidebar);
    root->base.add((component_t*)root, (component_t*)main);

    /* 统一渲染整棵树 */
    root->base.render((component_t*)root);
}
```

---

## 享元模式

```c
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
```

---

## 总结

结构型模式在C语言中的实现:

- 使用结构体包含和函数指针实现组合
- 通过指针包装实现代理和装饰
- 接口分离实现桥接和适配

---

## 参考

- [Design Patterns in C](https://en.wikibooks.org/wiki/C_Programming/Coroutines)


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
