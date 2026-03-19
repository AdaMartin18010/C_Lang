/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\01_Pointer_Multilevel_Linux.md
 * Line: 492
 * Language: c
 * Block ID: 03d365d3
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// Linux路径解析: fs/namei.c
// 使用二级指针优化路径遍历

struct dentry {
    struct dentry *d_parent;     // 父目录
    struct qstr d_name;          // 本节点名
    // ...
};

// 路径查找使用二级指针技巧
// 用于在查找过程中修改指针位置

// 简化示意: 在目录中查找条目
struct dentry *lookup_fast(struct dentry **pentry,   // 二级指针!
                           struct qstr *name,
                           unsigned int flags) {
    struct dentry *entry = *pentry;
    struct dentry *found = NULL;

    // 使用二级指针可以在找到后修改外部传入的指针
    // 例如切换到找到的子目录

    // 遍历目录项...

    if (found) {
        *pentry = found;  // 修改外部指针!
    }

    return found;
}

// 实际应用: 路径walk
int path_walk(const char *path, struct path *path_struct) {
    struct dentry *dentry = path_struct->dentry;

    while (*path) {
        struct qstr name;
        // 解析路径组件...

        // 传入二级指针，lookup_fast可以修改dentry
        if (!lookup_fast(&dentry, &name, 0)) {
            return -ENOENT;
        }
        // dentry现在指向找到的子目录
    }

    path_struct->dentry = dentry;
    return 0;
}
