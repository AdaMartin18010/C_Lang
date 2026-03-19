/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\09_Lab9_File_System.md
 * Line: 255
 * Language: c
 * Block ID: 97367c09
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/fs.c

// 在目录中查找文件名
// 返回锁定的inode，设置*poff为目录项偏移
struct inode*
dirlookup(struct inode *dp, char *name, uint *poff)
{
    uint off, inum;
    struct dirent de;

    // 检查是否为目录
    if (dp->type != T_DIR)
        panic("dirlookup not DIR");

    // 遍历所有目录项
    for (off = 0; off < dp->size; off += sizeof(de)) {
        // 读取目录项
        if (readi(dp, 0, (uint64)&de, off, sizeof(de)) != sizeof(de))
            panic("dirlookup read");

        // 跳过空闲项
        if (de.inum == 0)
            continue;

        // 比较文件名
        if (strncmp(name, de.name, DIRSIZ) == 0) {
            // 找到匹配
            if (poff)
                *poff = off;
            inum = de.inum;
            return iget(dp->dev, inum);
        }
    }

    return 0;  // 未找到
}

// 在目录中创建链接
int
dirlink(struct inode *dp, char *name, uint inum)
{
    int off;
    struct dirent de;
    struct inode *ip;

    // 检查是否已存在
    if ((ip = dirlookup(dp, name, 0)) != 0) {
        iput(ip);
        return -1;
    }

    // 查找空闲目录项
    for (off = 0; off < dp->size; off += sizeof(de)) {
        if (readi(dp, 0, (uint64)&de, off, sizeof(de)) != sizeof(de))
            panic("dirlink read");
        if (de.inum == 0)
            break;  // 找到空闲项
    }

    // 填充目录项
    strncpy(de.name, name, DIRSIZ);
    de.inum = inum;

    // 写入目录
    if (writei(dp, 0, (uint64)&de, off, sizeof(de)) != sizeof(de))
        panic("dirlink");

    return 0;
}
