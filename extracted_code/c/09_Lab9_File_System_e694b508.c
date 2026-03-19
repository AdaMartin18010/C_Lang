/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\09_Lab9_File_System.md
 * Line: 329
 * Language: c
 * Block ID: e694b508
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/fs.c

// 解析路径，返回父目录和最终文件名
static struct inode*
namex(char *path, int nameiparent, char *name)
{
    struct inode *ip, *next;

    // 确定起始目录
    if (*path == '/')
        ip = iget(ROOTDEV, ROOTINO);  // 绝对路径
    else
        ip = idup(myproc()->cwd);      // 相对路径

    while ((path = skipelem(path, name)) != 0) {
        ilock(ip);

        // 必须是目录
        if (ip->type != T_DIR) {
            iunlockput(ip);
            return 0;
        }

        // 如果要获取父目录且这是最后一个元素
        if (nameiparent && *path == '\0') {
            iunlock(ip);
            return ip;
        }

        // 查找下一个组件
        if ((next = dirlookup(ip, name, 0)) == 0) {
            iunlockput(ip);
            return 0;
        }

        iunlockput(ip);
        ip = next;
    }

    // 如果只需要父目录，但路径已结束
    if (nameiparent) {
        iput(ip);
        return 0;
    }

    return ip;
}

struct inode*
namei(char *path)
{
    char name[DIRSIZ];
    return namex(path, 0, name);
}

struct inode*
nameiparent(char *path, char *name)
{
    return namex(path, 1, name);
}
