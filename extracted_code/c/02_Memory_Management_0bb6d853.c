/*
 * Auto-generated from: 01_Core_Knowledge_System\02_Core_Layer\02_Memory_Management.md
 * Line: 1415
 * Language: c
 * Block ID: 0bb6d853
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 零拷贝技术实现
 * 减少数据在内核空间和用户空间之间的拷贝次数
 */
#include <sys/mman.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * 技术1: 内存映射文件 (mmap)
 * 避免了read/write的数据拷贝
 */
void *mmap_file(const char *path, size_t *size) {
    int fd = open(path, O_RDONLY);
    if (fd < 0) return NULL;

    off_t file_size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    void *addr = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);

    if (addr == MAP_FAILED) return NULL;

    *size = file_size;
    return addr;
}

void munmap_file(void *addr, size_t size) {
    munmap(addr, size);
}

/*
 * 技术2: sendfile - 内核态直接传输
 * 从文件描述符直接发送到socket，零拷贝
 */
ssize_t send_file_zero_copy(int out_fd, int in_fd, off_t *offset, size_t count) {
    return sendfile(out_fd, in_fd, offset, count);
}

/*
 * 技术3: splice - 管道零拷贝
 * 在两个文件描述符之间移动数据，无需用户空间缓冲
 */
#ifdef SPLICE_SUPPORT
ssize_t splice_zero_copy(int fd_in, loff_t *off_in,
                         int fd_out, loff_t *off_out,
                         size_t len, unsigned int flags) {
    return splice(fd_in, off_in, fd_out, off_out, len, flags);
}
#endif

/*
 * 技术4: 用户空间零拷贝缓冲区管理
 * 使用引用计数避免数据拷贝
 */
typedef struct {
    char *data;
    size_t len;
    size_t offset;
    int *ref_count;  // 引用计数
    void (*free_fn)(void*);
} Buffer;

Buffer buffer_create(size_t size) {
    Buffer buf = {0};
    buf.data = malloc(size);
    if (buf.data) {
        buf.len = size;
        buf.offset = 0;
        buf.ref_count = malloc(sizeof(int));
        if (buf.ref_count) {
            *buf.ref_count = 1;
        }
    }
    return buf;
}

Buffer buffer_from_data(char *data, size_t len, void (*free_fn)(void*)) {
    Buffer buf = {
        .data = data,
        .len = len,
        .offset = 0,
        .ref_count = malloc(sizeof(int)),
        .free_fn = free_fn
    };
    if (buf.ref_count) {
        *buf.ref_count = 1;
    }
    return buf;
}

Buffer buffer_ref(Buffer *buf) {
    if (buf && buf->ref_count) {
        (*buf->ref_count)++;
    }
    return *buf;
}

void buffer_unref(Buffer *buf) {
    if (buf && buf->ref_count) {
        (*buf->ref_count)--;
        if (*buf->ref_count == 0) {
            if (buf->free_fn) {
                buf->free_fn(buf->data);
            } else {
                free(buf->data);
            }
            free(buf->ref_count);
            buf->data = NULL;
            buf->ref_count = NULL;
        }
    }
}

// 切片（不拷贝数据）
Buffer buffer_slice(Buffer *buf, size_t offset, size_t len) {
    Buffer slice = buffer_ref(buf);
    if (slice.data) {
        slice.offset += offset;
        slice.len = len;
    }
    return slice;
}

/*
 * 技术5: 环形缓冲区（减少内存分配）
 */
typedef struct {
    char *buffer;
    size_t size;
    size_t head;  // 写入位置
    size_t tail;  // 读取位置
    size_t count; // 当前数据量
} RingBuffer;

int ringbuffer_init(RingBuffer *rb, size_t size) {
    rb->buffer = malloc(size);
    if (!rb->buffer) return -1;
    rb->size = size;
    rb->head = rb->tail = rb->count = 0;
    return 0;
}

void ringbuffer_destroy(RingBuffer *rb) {
    free(rb->buffer);
    rb->buffer = NULL;
}

size_t ringbuffer_write(RingBuffer *rb, const char *data, size_t len) {
    size_t available = rb->size - rb->count;
    size_t to_write = len < available ? len : available;

    for (size_t i = 0; i < to_write; i++) {
        rb->buffer[rb->head] = data[i];
        rb->head = (rb->head + 1) % rb->size;
    }
    rb->count += to_write;
    return to_write;
}

size_t ringbuffer_read(RingBuffer *rb, char *data, size_t len) {
    size_t to_read = len < rb->count ? len : rb->count;

    for (size_t i = 0; i < to_read; i++) {
        data[i] = rb->buffer[rb->tail];
        rb->tail = (rb->tail + 1) % rb->size;
    }
    rb->count -= to_read;
    return to_read;
}
