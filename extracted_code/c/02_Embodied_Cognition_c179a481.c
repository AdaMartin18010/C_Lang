/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\04_Cognitive_Representation\02_Embodied_Cognition.md
 * Line: 359
 * Language: c
 * Block ID: c179a481
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 热重载支持
typedef struct {
    void *handle;
    time_t last_modified;
    void (*reload)(void);
} HotReloadModule;

void check_and_reload(HotReloadModule *mod) {
    struct stat st;
    if (stat(mod->path, &st) == 0) {
        if (st.st_mtime > mod->last_modified) {
            // 文件已修改，重新加载
            mod->reload();
            mod->last_modified = st.st_mtime;
            printf("[Hot Reload] Module updated\n");
        }
    }
}
