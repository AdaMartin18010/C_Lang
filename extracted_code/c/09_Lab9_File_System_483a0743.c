/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\09_Lab9_File_System.md
 * Line: 574
 * Language: c
 * Block ID: 483a0743
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/sysfile.c

// 创建符号链接
uint64
sys_symlink(void)
{
    char target[MAXPATH], path[MAXPATH];
    struct inode *ip;

    // 获取参数
    if (argstr(0, target, MAXPATH) < 0 ||
        argstr(1, path, MAXPATH) < 0)
        return -1;

    begin_op();

    // 创建新inode
    ip = create(path, T_SYMLINK, 0, 0);
    if (ip == 0) {
        end_op();
        return -1;
    }

    // 写入目标路径
    if (writei(ip, 0, (uint64)target, 0, strlen(target)) < 0) {
        iunlockput(ip);
        end_op();
        return -1;
    }

    iunlockput(ip);
    end_op();
    return 0;
}

// 修改open以支持符号链接
uint64
sys_open(void)
{
    // ... 原有代码 ...

    // 处理符号链接
    int depth = 0;
    while (ip->type == T_SYMLINK && !(omode & O_NOFOLLOW)) {
        // 防止循环链接
        if (depth++ > 10) {
            iunlockput(ip);
            end_op();
            return -1;
        }

        // 读取目标路径
        char target[MAXPATH];
        int n = readi(ip, 0, (uint64)target, 0, MAXPATH);
        iunlockput(ip);

        if (n < 0 || n >= MAXPATH) {
            end_op();
            return -1;
        }
        target[n] = '\0';

        // 解析目标路径
        if ((ip = namei(target)) == 0) {
            end_op();
            return -1;
        }
        ilock(ip);
    }

    // ... 原有代码 ...
}
