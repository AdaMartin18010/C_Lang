/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\09_Interrupt_Hardware\02_System_Call_Hardware.md
 * Line: 1759
 * Language: c
 * Block ID: f9ed0d11
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 批量系统调用优化示例
 * 减少用户态/内核态切换次数
 */

// 传统方式: 多次单独调用
void slow_readv(int fd, struct iovec *iov, int iovcnt)
{
    for (int i = 0; i < iovcnt; i++) {
        read(fd, iov[i].iov_base, iov[i].iov_len);  // 每次系统调用
    }
}

// 优化方式: 使用readv单次调用
void fast_readv(int fd, struct iovec *iov, int iovcnt)
{
    // 一次系统调用处理多个缓冲区
    readv(fd, iov, iovcnt);
}

// io_uring: 真正的异步批量处理
struct io_uring ring;

void setup_io_uring(void)
{
    // 初始化io_uring
    io_uring_queue_init(32, &ring, 0);
}

void batch_submit(struct io_request *reqs, int count)
{
    struct io_uring_sqe *sqe;

    // 批量提交请求
    for (int i = 0; i < count; i++) {
        sqe = io_uring_get_sqe(&ring);

        if (reqs[i].type == IO_READ) {
            io_uring_prep_read(sqe, reqs[i].fd, reqs[i].buf,
                               reqs[i].len, reqs[i].offset);
        } else if (reqs[i].type == IO_WRITE) {
            io_uring_prep_write(sqe, reqs[i].fd, reqs[i].buf,
                                reqs[i].len, reqs[i].offset);
        }

        sqe->user_data = i;  // 标识请求
    }

    // 单次系统调用提交所有请求
    io_uring_submit(&ring);

    // 稍后异步获取结果
}
