/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\09_Interrupt_Hardware\02_System_Call_Hardware.md
 * Line: 945
 * Language: c
 * Block ID: 14723256
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 系统调用参数验证机制
 */

// 1. 基本指针检查
SYSCALL_DEFINE3(read, unsigned int, fd, char __user *, buf, size_t, count)
{
    struct fd f;
    ssize_t ret = -EBADF;

    // 验证buf是有效的用户空间指针
    if (!access_ok(buf, count))
        return -EFAULT;

    f = fdget(fd);
    if (f.file) {
        // 使用安全的复制函数
        loff_t pos = file_pos_read(f.file);
        ret = vfs_read(f.file, buf, count, &pos);
        file_pos_write(f.file, pos);
        fdput(f);
    }

    return ret;
}

// 2. 复杂结构体验证
SYSCALL_DEFINE2(gettimeofday, struct __kernel_old_timeval __user *, tv,
                struct timezone __user *, tz)
{
    if (tv) {
        struct __kernel_old_timeval ktv;

        // 验证tv指针
        if (!access_ok(tv, sizeof(*tv)))
            return -EFAULT;

        // 获取时间
        do_gettimeofday(&ktv);

        // 安全复制到用户空间
        if (copy_to_user(tv, &ktv, sizeof(ktv)))
            return -EFAULT;
    }

    if (tz) {
        // 类似处理...
    }

    return 0;
}

// 3. 路径名验证
static int getname_flags(const char __user *filename, int flags,
                         struct filename **result)
{
    int len;
    long max;
    char *kname;
    struct filename *name;

    // 首先检查基本访问权限
    if (!(flags & LOOKUP_EMPTY)) {
        if (unlikely(!filename))
            return -EFAULT;
    }

    max = PATH_MAX;
    kname = (char *)__getname();  // 分配内核缓冲区
    if (!kname)
        return -ENOMEM;

    // 从用户空间复制字符串
    len = strncpy_from_user(kname, filename, max);
    if (unlikely(len < 0)) {
        __putname(kname);
        return len;
    }

    // 路径名太长
    if (unlikely(len == max)) {
        __putname(kname);
        return -ENAMETOOLONG;
    }

    // 创建filename结构
    name = __alloc_name(kname, len);
    if (!name) {
        __putname(kname);
        return -ENOMEM;
    }

    *result = name;
    return 0;
}

// 4. 批量参数验证 (ioctl等)
SYSCALL_DEFINE3(ioctl, unsigned int, fd, unsigned int, cmd,
                unsigned long, arg)
{
    int error;
    struct fd f = fdget(fd);

    if (!f.file)
        return -EBADF;

    // 根据cmd的方向验证arg指针
    // _IOC_DIR(cmd) 返回: _IOC_NONE, _IOC_READ, _IOC_WRITE
    if (_IOC_DIR(cmd) & _IOC_READ) {
        // 用户读取，内核写入
        if (!access_ok((void __user *)arg, _IOC_SIZE(cmd))) {
            fdput(f);
            return -EFAULT;
        }
    }
    if (_IOC_DIR(cmd) & _IOC_WRITE) {
        // 用户写入，内核读取
        if (!access_ok((void __user *)arg, _IOC_SIZE(cmd))) {
            fdput(f);
            return -EFAULT;
        }
    }

    error = vfs_ioctl(f.file, cmd, arg);
    fdput(f);

    return error;
}
