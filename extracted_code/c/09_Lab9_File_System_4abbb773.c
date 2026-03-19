/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\09_Lab9_File_System.md
 * Line: 446
 * Language: c
 * Block ID: 4abbb773
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// kernel/log.c

// 开始事务
void
begin_op(void)
{
    acquire(&log.lock);

    while (1) {
        // 检查是否可以开始新事务
        if (log.committing) {
            // 正在提交，等待
            sleep(&log, &log.lock);
        } else if (log.lh.n + (log.outstanding + 1) * MAXOPBLOCKS > LOGSIZE) {
            // 日志空间不足，等待
            sleep(&log, &log.lock);
        } else {
            // 可以开始
            log.outstanding++;
            release(&log.lock);
            break;
        }
    }
}

// 结束事务
void
end_op(void)
{
    int do_commit = 0;

    acquire(&log.lock);
    log.outstanding--;

    // 如果是最后一个事务，负责提交
    if (log.outstanding == 0) {
        do_commit = 1;
        log.committing = 1;
    }

    release(&log.lock);

    if (do_commit) {
        commit();           // 提交日志
        acquire(&log.lock);
        log.committing = 0;
        wakeup(&log);       // 唤醒等待的begin_op
        release(&log.lock);
    }
}

// 写块到日志
void
log_write(struct buf *b)
{
    acquire(&log.lock);

    // 检查是否已在日志中
    int i;
    for (i = 0; i < log.lh.n; i++) {
        if (log.lh.block[i] == b->blockno)
            break;
    }

    // 记录目标块号
    log.lh.block[i] = b->blockno;
    if (i == log.lh.n) {
        // 新块，增加计数
        log.lh.n++;
    }

    release(&log.lock);

    // 标记缓冲区为已修改，但不立即写入磁盘
    b->flags |= B_DIRTY;
}

// 提交日志
static void
commit(void)
{
    if (log.lh.n > 0) {
        // 1. 将修改的块写入日志
        write_log();

        // 2. 写入日志头 (提交点)
        write_head();

        // 3. 将日志中的块安装到实际位置
        install_trans(0);

        // 4. 清空日志头
        log.lh.n = 0;
        write_head();
    }
}
