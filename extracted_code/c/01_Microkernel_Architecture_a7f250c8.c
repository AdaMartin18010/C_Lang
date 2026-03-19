/*
 * Auto-generated from: 07_Modern_Toolchain\09_System_Architecture\01_Microkernel_Architecture.md
 * Line: 669
 * Language: c
 * Block ID: a7f250c8
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 用户态文件系统服务器 */

typedef struct {
    uint32_t handle;
    uint32_t flags;
    uint64_t position;
    vnode_t* vnode;
} file_desc_t;

typedef struct {
    file_desc_t files[256];
    int num_open;
} fs_session_t;

static hash_table_t* g_vnode_cache;

void filesystem_server_main(void) {
    message_t msg;

    /* 初始化 */
    init_vnode_cache();
    mount_root_filesystem();

    /* 注册服务 */
    register_service("filesystem", FS_SERVICE_ID);

    while (1) {
        receive(ANY, &msg);

        switch (msg.type) {
            case FS_OPEN:
                handle_open(&msg);
                break;
            case FS_READ:
                handle_read(&msg);
                break;
            case FS_WRITE:
                handle_write(&msg);
                break;
            case FS_CLOSE:
                handle_close(&msg);
                break;
            case FS_STAT:
                handle_stat(&msg);
                break;
            case FS_MOUNT:
                handle_mount(&msg);
                break;
            default:
                reply_error(&msg, -EINVAL);
        }
    }
}

/* 处理打开文件 */
void handle_open(message_t* msg) {
    fs_open_req_t* req = (fs_open_req_t*)msg->data;
    fs_session_t* session = get_session(msg->source);

    /* 路径解析 */
    vnode_t* vnode = lookup_path(req->path);
    if (!vnode) {
        reply_error(msg, -ENOENT);
        return;
    }

    /* 权限检查 */
    if (!check_access(session, vnode, req->flags)) {
        reply_error(msg, -EACCES);
        return;
    }

    /* 分配文件描述符 */
    int fd = alloc_fd(session);
    if (fd < 0) {
        reply_error(msg, -EMFILE);
        return;
    }

    /* 设置文件描述 */
    session->files[fd].vnode = vnode;
    session->files[fd].flags = req->flags;
    session->files[fd].position = 0;

    /* 调用底层FS */
    if (vnode->ops->open) {
        int ret = vnode->ops->open(vnode, req->flags);
        if (ret < 0) {
            free_fd(session, fd);
            reply_error(msg, ret);
            return;
        }
    }

    /* 回复 */
    fs_open_reply_t reply = { .fd = fd };
    send(msg->source, MSG_FS_REPLY, &reply, sizeof(reply));
}
