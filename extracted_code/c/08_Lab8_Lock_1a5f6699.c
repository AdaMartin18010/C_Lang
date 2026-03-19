/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\08_Lab8_Lock.md
 * Line: 467
 * Language: c
 * Block ID: 1a5f6699
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/bio.c

// 原始设计: 全局bcache锁
struct {
    struct spinlock lock;
    struct buf buf[NBUF];
    struct buf head;  // LRU链表头
} bcache;

// 优化设计: 哈希桶 + 每桶锁
#define NBUCKETS 13

struct bucket {
    struct spinlock lock;
    struct buf *head;
};

struct {
    struct bucket buckets[NBUCKETS];
    struct buf buf[NBUF];
} bcache;

// 哈希函数
#define HASH(dev, blockno) (((dev) + (blockno)) % NBUCKETS)

struct buf *
bget(uint dev, uint blockno)
{
    int bucket = HASH(dev, blockno);

    // 只锁定对应的桶
    acquire(&bcache.buckets[bucket].lock);

    // 在桶中查找
    for (struct buf *b = bcache.buckets[bucket].head;
         b != 0; b = b->next) {
        if (b->dev == dev && b->blockno == blockno) {
            b->refcnt++;
            release(&bcache.buckets[bucket].lock);
            return b;
        }
    }

    // 未找到，分配新缓冲区...

    release(&bcache.buckets[bucket].lock);
    return b;
}
