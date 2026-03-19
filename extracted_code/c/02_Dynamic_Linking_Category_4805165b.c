/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\08_Linking_Loading_Topology\02_Dynamic_Linking_Category.md
 * Line: 246
 * Language: c
 * Block ID: 4805165b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 动态加载API实现
#include <dlfcn.h>

// 句柄结构
typedef struct {
    struct link_map *lm;
    int mode;
    void *sym_cache;
} DlHandle;

// 打开共享库
void *dlopen_impl(const char *filename, int mode) {
    // 1. 查找或加载库
    struct link_map *lm = find_or_load_library(filename);
    if (!lm) return NULL;

    // 2. 增加引用计数
    lm->l_refcount++;

    // 3. 处理依赖
    if (mode & RTLD_NOW) {
        resolve_all_symbols(lm);
    }

    // 4. 执行初始化
    if (lm->l_info[DT_INIT]) {
        void (*init)(void) = (void (*)(void))
            (lm->l_addr + lm->l_info[DT_INIT]->d_un.d_ptr);
        init();
    }

    // 5. 执行.init_array
    if (lm->l_info[DT_INIT_ARRAY]) {
        Elf64_Addr *init_array = (Elf64_Addr *)(
            lm->l_addr + lm->l_info[DT_INIT_ARRAY]->d_un.d_ptr
        );
        size_t count = lm->l_info[DT_INIT_ARRAYSZ]->d_un.d_val / sizeof(void *);
        for (size_t i = 0; i < count; i++) {
            void (*fn)(void) = (void (*)(void))(lm->l_addr + init_array[i]);
            fn();
        }
    }

    DlHandle *handle = malloc(sizeof(DlHandle));
    handle->lm = lm;
    handle->mode = mode;
    return handle;
}

// 符号查找
void *dlsym_impl(void *handle, const char *symbol) {
    DlHandle *h = handle;
    struct link_map *l = h->lm;

    // 查找符号
    struct link_map *definer;
    Elf64_Sym *sym = lookup_symbol(symbol, l, &definer);

    if (sym) {
        return (void *)(definer->l_addr + sym->st_value);
    }
    return NULL;
}

// 关闭库
int dlclose_impl(void *handle) {
    DlHandle *h = handle;
    struct link_map *l = h->lm;

    // 减少引用计数
    if (--l->l_refcount == 0) {
        // 执行终止函数
        if (l->l_info[DT_FINI]) {
            void (*fini)(void) = (void (*)(void))
                (l->l_addr + l->l_info[DT_FINI]->d_un.d_ptr);
            fini();
        }

        // 卸载库（如果支持）
        // munmap(l->l_addr, ...);
    }

    free(h);
    return 0;
}
