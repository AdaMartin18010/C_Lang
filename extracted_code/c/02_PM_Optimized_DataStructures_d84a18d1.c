/*
 * Auto-generated from: 03_System_Technology_Domains\12_Persistent_Memory\02_PM_Optimized_DataStructures.md
 * Line: 129
 * Language: c
 * Block ID: d84a18d1
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 类型编号定义 - 每个持久类型唯一 */
enum type_num {
    TYPE_ROOT,
    TYPE_NODE,
    TYPE_DATA,
    TYPE_MAX
};

/* 分配对象 */
PMEMoid pmemobj_alloc(PMEMobjpool *pop, size_t size, uint64_t type_num);
PMEMoid pmemobj_zalloc(PMEMobjpool *pop, size_t size, uint64_t type_num);

/* 重新分配 */
PMEMoid pmemobj_realloc(PMEMobjpool *pop, PMEMoid oid, size_t size,
                        uint64_t type_num);

/* 释放 */
int pmemobj_free(PMEMoid *oidp);

/* 使用示例 */
PMEMoid alloc_node(PMEMobjpool *pop) {
    PMEMoid oid = pmemobj_zalloc(pop, sizeof(struct node), TYPE_NODE);
    if (OID_IS_NULL(oid)) {
        fprintf(stderr, "Allocation failed\n");
    }
    return oid;
}
