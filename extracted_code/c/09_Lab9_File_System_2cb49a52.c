/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\09_Lab9_File_System.md
 * Line: 769
 * Language: c
 * Block ID: 2cb49a52
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 打印inode信息
void debug_inode(struct inode *ip) {
    printf("inode %d:\n", ip->inum);
    printf("  type=%d major=%d minor=%d\n",
           ip->type, ip->major, ip->minor);
    printf("  nlink=%d size=%d\n",
           ip->nlink, ip->size);
    printf("  blocks:");
    for (int i = 0; i < NDIRECT + 1; i++) {
        if (ip->addrs[i])
            printf(" %d", ip->addrs[i]);
    }
    printf("\n");
}

// 打印目录内容
void debug_dir(struct inode *dp) {
    struct dirent de;

    printf("directory (inode %d):\n", dp->inum);
    for (int off = 0; off < dp->size; off += sizeof(de)) {
        readi(dp, 0, (uint64)&de, off, sizeof(de));
        if (de.inum) {
            printf("  %s -> inode %d\n", de.name, de.inum);
        }
    }
}
