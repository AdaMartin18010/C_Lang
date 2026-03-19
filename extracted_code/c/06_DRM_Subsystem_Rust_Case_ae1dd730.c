/*
 * Auto-generated from: 15_Memory_Safe_Languages\01_Rust_vs_C\06_DRM_Subsystem_Rust_Case.md
 * Line: 304
 * Language: c
 * Block ID: ae1dd730
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 漏洞代码片段
static int amdgpu_bo_list_create(struct amdgpu_device *adev,
                                  struct drm_file *filp,
                                  struct drm_amdgpu_bo_list_entry *entries,
                                  uint32_t num_entries,
                                  struct amdgpu_bo_list **result)
{
    struct amdgpu_bo_list *list;
    struct amdgpu_bo *bo;
    int i, r;

    list = kzalloc(sizeof(*list), GFP_KERNEL);
    if (!list)
        return -ENOMEM;

    for (i = 0; i < num_entries; i++) {
        // 获取BO引用
        bo = amdgpu_bo_ref(amdgpu_bo_lookup(filp, entries[i].bo_handle));
        if (IS_ERR(bo)) {
            r = PTR_ERR(bo);
            goto error_free;  // 跳转时list->entries未初始化
        }
        list->entries[i].bo = bo;
    }

    *result = list;
    return 0;

error_free:
    // 漏洞: 尝试释放未初始化的entries[i].bo
    for (int j = 0; j < i; j++) {
        amdgpu_bo_unref(&list->entries[j].bo);  // UAF风险
    }
    kfree(list);
    return r;
}
