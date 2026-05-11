/*
 * 插件系统（动态加载）示例
 * 编译: gcc -Wall -Wextra -o plugin_system plugin_system.c
 * Windows: gcc -Wall -Wextra -o plugin_system plugin_system.c
 * Linux:   gcc -Wall -Wextra -o plugin_system plugin_system.c -ldl
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
typedef HMODULE plugin_handle_t;
#define plugin_load(path) LoadLibraryA(path)
#define plugin_get_sym(h, name) GetProcAddress(h, name)
#define plugin_close(h) FreeLibrary(h)
#else
#include <dlfcn.h>
typedef void *plugin_handle_t;
#define plugin_load(path) dlopen(path, RTLD_NOW)
#define plugin_get_sym(h, name) dlsym(h, name)
#define plugin_close(h) dlclose(h)
#endif

#define PLUGIN_API_VERSION 1

typedef struct {
    int api_version;
    const char *name;
    const char *version;
    int (*init)(void);
    void (*shutdown)(void);
    int (*execute)(const char *input, char *output, size_t outlen);
} PluginInfo;

typedef struct {
    plugin_handle_t handle;
    PluginInfo *info;
} Plugin;

Plugin *plugin_load_info(const char *path) {
    plugin_handle_t h = plugin_load(path);
    if (!h) {
        fprintf(stderr, "Failed to load %s\n", path);
        return NULL;
    }
    
    PluginInfo *(*get_info)(void) = (PluginInfo *(*)(void))plugin_get_sym(h, "plugin_get_info");
    if (!get_info) {
        fprintf(stderr, "Missing plugin_get_info in %s\n", path);
        plugin_close(h);
        return NULL;
    }
    
    PluginInfo *info = get_info();
    if (info->api_version != PLUGIN_API_VERSION) {
        fprintf(stderr, "API version mismatch: %d vs %d\n",
                info->api_version, PLUGIN_API_VERSION);
        plugin_close(h);
        return NULL;
    }
    
    Plugin *p = malloc(sizeof(Plugin));
    p->handle = h;
    p->info = info;
    return p;
}

void plugin_unload(Plugin *p) {
    if (p) {
        if (p->info && p->info->shutdown) p->info->shutdown();
        plugin_close(p->handle);
        free(p);
    }
}

/* 模拟内置插件 */
static int builtin_init(void) { printf("Builtin plugin init\n"); return 0; }
static void builtin_shutdown(void) { printf("Builtin plugin shutdown\n"); }
static int builtin_execute(const char *in, char *out, size_t outlen) {
    snprintf(out, outlen, "Builtin processed: %s", in);
    return 0;
}

static PluginInfo builtin_info = {
    .api_version = PLUGIN_API_VERSION,
    .name = "builtin",
    .version = "1.0.0",
    .init = builtin_init,
    .shutdown = builtin_shutdown,
    .execute = builtin_execute,
};

PluginInfo *plugin_get_info(void) { return &builtin_info; }

int main(void) {
    /* 加载自身作为模拟插件 */
    Plugin *p = plugin_load_info("plugin_system");
    if (!p) {
        /* 回退到内置 */
        printf("Using builtin plugin directly\n");
        builtin_init();
        char out[256];
        builtin_execute("hello", out, sizeof(out));
        printf("Result: %s\n", out);
        builtin_shutdown();
        return 0;
    }
    
    printf("Loaded plugin: %s v%s\n", p->info->name, p->info->version);
    p->info->init();
    
    char out[256];
    p->info->execute("test data", out, sizeof(out));
    printf("Result: %s\n", out);
    
    plugin_unload(p);
    return 0;
}
