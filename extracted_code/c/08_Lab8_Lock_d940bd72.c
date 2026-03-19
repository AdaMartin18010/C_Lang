/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\08_Lab8_Lock.md
 * Line: 538
 * Language: c
 * Block ID: d940bd72
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/rwlock.c

void
readlock(struct rwlock *rw)
{
    acquire(&rw->lk);

    // 等待写者完成
    while (rw->writing) {
        sleep(rw, &rw->lk);
    }

    rw->readers++;

    release(&rw->lk);
}

void
readunlock(struct rwlock *rw)
{
    acquire(&rw->lk);

    rw->readers--;

    // 如果最后一个读者离开，唤醒写者
    if (rw->readers == 0) {
        wakeup(rw);
    }

    release(&rw->lk);
}

void
writelock(struct rwlock *rw)
{
    acquire(&rw->lk);

    // 等待所有读者和写者完成
    while (rw->writing || rw->readers > 0) {
        sleep(rw, &rw->lk);
    }

    rw->writing = 1;
    rw->writer = myproc();

    release(&rw->lk);
}

void
writeunlock(struct rwlock *rw)
{
    acquire(&rw->lk);

    rw->writing = 0;
    rw->writer = 0;

    // 唤醒所有等待的读者和写者
    wakeup(rw);

    release(&rw->lk);
}
