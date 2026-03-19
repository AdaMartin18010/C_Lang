/*
 * Auto-generated from: 07_Modern_Toolchain\09_System_Architecture\02_Event_Driven_Architecture.md
 * Line: 204
 * Language: c
 * Block ID: f8b7fd3b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* Proactor: 异步完成处理 */

#include <aio.h>
#include <signal.h>

typedef struct {
    void (*complete)(struct aiocb* cb, ssize_t nbytes);
    void* context;
} aio_callback_t;

/* 异步读取 */
void async_read(int fd, void* buf, size_t len, off_t offset,
                void (*callback)(struct aiocb*, ssize_t)) {
    struct aiocb* cb = calloc(1, sizeof(struct aiocb));
    cb->aio_fildes = fd;
    cb->aio_buf = buf;
    cb->aio_nbytes = len;
    cb->aio_offset = offset;

    /* 设置完成通知 */
    cb->aio_sigevent.sigev_notify = SIGEV_THREAD;
    cb->aio_sigevent.sigev_notify_function =
        (void (*)(sigval_t))callback;
    cb->aio_sigevent.sigev_value.sival_ptr = cb;

    aio_read(cb);
}

/* 使用io_uring的Proactor (Linux 5.1+) */
#include <liburing.h>

struct io_uring ring;

typedef void (*io_completion_t)(struct io_uring_cqe* cqe);

void proactor_init(void) {
    io_uring_queue_init(4096, &ring, 0);
}

void proactor_submit_read(int fd, void* buf, size_t len, off_t offset,
                          io_completion_t callback) {
    struct io_uring_sqe* sqe = io_uring_get_sqe(&ring);
    io_uring_prep_read(sqe, fd, buf, len, offset);
    io_uring_sqe_set_data(sqe, callback);
    io_uring_submit(&ring);
}

void proactor_run(void) {
    struct io_uring_cqe* cqe;

    while (1) {
        int ret = io_uring_wait_cqe(&ring, &cqe);
        if (ret < 0) continue;

        io_completion_t callback = io_uring_cqe_get_data(cqe);
        if (callback) {
            callback(cqe);
        }

        io_uring_cqe_seen(&ring, cqe);
    }
}
