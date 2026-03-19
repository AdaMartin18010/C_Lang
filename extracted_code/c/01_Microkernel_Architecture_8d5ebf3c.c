/*
 * Auto-generated from: 07_Modern_Toolchain\09_System_Architecture\01_Microkernel_Architecture.md
 * Line: 772
 * Language: c
 * Block ID: 8d5ebf3c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 基于能力的访问控制 */

/* 权限位 */
#define CAP_READ    (1 << 0)
#define CAP_WRITE   (1 << 1)
#define CAP_EXEC    (1 << 2)
#define CAP_MAP     (1 << 3)
#define CAP_GRANT   (1 << 4)    /* 可转授 */
#define CAP_DELETE  (1 << 5)

/* 能力结构 */
typedef struct capability {
    uint64_t obj_id;        /* 对象标识 */
    uint32_t rights;        /* 权限位图 */
    uint32_t type;          /* 对象类型 */
    struct capability* next; /* CNode链表 */
} capability_t;

/* CNode (能力节点) */
typedef struct {
    capability_t* slots[256];  /* 256个槽位 */
    spinlock_t lock;
} cnode_t;

/* 能力查找 */
capability_t* lookup_cap(cnode_t* cnode, unsigned slot) {
    if (slot >= 256) return NULL;

    spin_lock(&cnode->lock);
    capability_t* cap = cnode->slots[slot];
    spin_unlock(&cnode->lock);

    return cap;
}

/* 能力派生 (降级权限) */
int derive_cap(capability_t* parent, uint32_t new_rights,
               cnode_t* dest_cnode, unsigned dest_slot) {
    /* 检查父能力是否有GRANT权限 */
    if (!(parent->rights & CAP_GRANT)) {
        return -EPERM;
    }

    /* 新权限必须是子集 */
    if ((new_rights & ~parent->rights) != 0) {
        return -EINVAL;
    }

    /* 分配新能力 */
    capability_t* child = kmalloc(sizeof(capability_t));
    if (!child) return -ENOMEM;

    child->obj_id = parent->obj_id;
    child->rights = new_rights;
    child->type = parent->type;

    /* 插入目标CNode */
    spin_lock(&dest_cnode->lock);
    if (dest_cnode->slots[dest_slot]) {
        spin_unlock(&dest_cnode->lock);
        kfree(child);
        return -EEXIST;
    }
    dest_cnode->slots[dest_slot] = child;
    spin_unlock(&dest_cnode->lock);

    return 0;
}

/* 能力调用 */
int invoke_cap(cnode_t* cnode, unsigned slot, uint32_t op, void* args) {
    capability_t* cap = lookup_cap(cnode, slot);
    if (!cap) return -EBADF;

    /* 检查操作权限 */
    uint32_t required = 0;
    switch (op) {
        case OP_READ:  required = CAP_READ; break;
        case OP_WRITE: required = CAP_WRITE; break;
        case OP_EXEC:  required = CAP_EXEC; break;
        default: return -EINVAL;
    }

    if ((cap->rights & required) != required) {
        return -EPERM;
    }

    /* 执行操作 */
    return object_invoke(cap->obj_id, op, args);
}
