/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\07_Lab7_Network_Driver.md
 * Line: 503
 * Language: c
 * Block ID: 5fed374d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/sysnet.c

// 创建socket
int
sys_socket(void)
{
    int domain, type, protocol;

    if (argint(0, &domain) < 0 ||
        argint(1, &type) < 0 ||
        argint(2, &protocol) < 0)
        return -1;

    // 分配socket结构
    struct sock *so = sockalloc(domain, type, protocol);
    if (!so)
        return -1;

    // 分配文件描述符
    int fd = fdalloc(so);
    if (fd < 0) {
        sockclose(so);
        return -1;
    }

    return fd;
}

// 发送数据
int
sys_send(void)
{
    int fd, n;
    uint64 addr;
    struct file *f;
    struct mbuf *m;

    if (argint(0, &fd) < 0 ||
        argint(2, &n) < 0 ||
        argaddr(1, &addr) < 0)
        return -1;

    f = myproc()->ofile[fd];
    if (!f || f->type != FD_SOCK)
        return -1;

    // 分配mbuf并复制数据
    m = mbufalloc(0);
    if (!m)
        return -1;

    if (copyin(myproc()->pagetable, mbufput(m, n), addr, n) < 0) {
        mbuffree(m);
        return -1;
    }

    return socksend(f->sock, m);
}

// 接收数据
int
sys_recv(void)
{
    int fd, n;
    uint64 addr;
    struct file *f;
    struct mbuf *m;

    if (argint(0, &fd) < 0 ||
        argint(2, &n) < 0 ||
        argaddr(1, &addr) < 0)
        return -1;

    f = myproc()->ofile[fd];
    if (!f || f->type != FD_SOCK)
        return -1;

    m = sockrecv(f->sock);
    if (!m)
        return -1;

    // 复制到用户空间
    int len = m->len;
    if (len > n)
        len = n;

    if (copyout(myproc()->pagetable, addr, m->head, len) < 0) {
        mbuffree(m);
        return -1;
    }

    mbuffree(m);
    return len;
}
