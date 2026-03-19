/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\08_Lab8_Lock.md
 * Line: 241
 * Language: c
 * Block ID: 9e9bef0c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/sleeplock.c

void
initsleeplock(struct sleeplock *lk, char *name)
{
    initlock(&lk->lk, "sleep lock");
    lk->name = name;
    lk->locked = 0;
    lk->pid = 0;
}

void
acquiresleep(struct sleeplock *lk)
{
    // 先获取保护锁
    acquire(&lk->lk);

    // 等待锁释放
    while (lk->locked) {
        // 睡眠，等待锁释放
        sleep(lk, &lk->lk);
    }

    // 获取锁
    lk->locked = 1;
    lk->pid = myproc()->pid;

    release(&lk->lk);
}

void
releasesleep(struct sleeplock *lk)
{
    acquire(&lk->lk);

    lk->locked = 0;
    lk->pid = 0;

    // 唤醒等待的进程
    wakeup(lk);

    release(&lk->lk);
}

int
holdingsleep(struct sleeplock *lk)
{
    int r;

    acquire(&lk->lk);
    r = lk->locked && (lk->pid == myproc()->pid);
    release(&lk->lk);

    return r;
}
