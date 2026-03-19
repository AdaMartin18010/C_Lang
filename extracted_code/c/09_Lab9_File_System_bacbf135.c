/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\09_Lab9_File_System.md
 * Line: 151
 * Language: c
 * Block ID: bacbf135
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/fs.c

// 从inode编号获取inode (增加引用计数)
struct inode*
iget(uint dev, uint inum)
{
    struct inode *ip, *empty;

    acquire(&itable.lock);

    // 查找是否已在缓存中
    empty = 0;
    for (ip = &itable.inode[0]; ip < &itable.inode[NINODE]; ip++) {
        if (ip->ref > 0 && ip->dev == dev && ip->inum == inum) {
            ip->ref++;
            release(&itable.lock);
            return ip;
        }
        if (empty == 0 && ip->ref == 0)
            empty = ip;
    }

    // 分配新inode缓存项
    if (empty == 0)
        panic("iget: no inodes");

    ip = empty;
    ip->dev = dev;
    ip->inum = inum;
    ip->ref = 1;
    ip->valid = 0;

    release(&itable.lock);
    return ip;
}

// 减少引用计数，如为0则释放
void
iput(struct inode *ip)
{
    acquire(&itable.lock);

    if (ip->ref == 1 && ip->valid && ip->nlink == 0) {
        // inode不再被引用，且无链接
        // 释放数据块和inode
        release(&itable.lock);

        itrunc(ip);      // 截断文件(释放数据块)
        ip->type = 0;
        iupdate(ip);     // 写入磁盘

        acquire(&itable.lock);
    }

    ip->ref--;
    release(&itable.lock);
}

// 从磁盘读取inode
void
ilock(struct inode *ip)
{
    // ...
    if (!ip->valid) {
        // 从磁盘读取dinode
        struct buf *bp = bread(ip->dev, IBLOCK(ip->inum, sb));
        struct dinode *dip = (struct dinode *)bp->data +
                             ip->inum % IPB;
        ip->type = dip->type;
        ip->major = dip->major;
        ip->minor = dip->minor;
        ip->nlink = dip->nlink;
        ip->size = dip->size;
        memmove(ip->addrs, dip->addrs, sizeof(ip->addrs));
        brelse(bp);
        ip->valid = 1;
    }
    // ...
}
