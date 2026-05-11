/*
 * mmap 内存映射文件示例（POSIX）
 * 编译: gcc -Wall -Wextra -o mmap_example mmap_example.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

int main(void) {
    const char *path = "mmap_test.txt";
    
    /* 创建测试文件 */
    int fd = open(path, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open");
        return 1;
    }
    
    const char *msg = "Hello, mmap!\n";
    write(fd, msg, strlen(msg));
    
    /* 扩展到更大 */
    if (ftruncate(fd, 4096) < 0) {
        perror("ftruncate");
        return 1;
    }
    
    /* 内存映射 */
    char *addr = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        return 1;
    }
    
    printf("File mapped at %p\n", (void *)addr);
    printf("Content: %s", addr);
    
    /* 直接修改内存，文件同步 */
    snprintf(addr, 4096, "Modified via mmap! PID=%d\n", getpid());
    
    if (msync(addr, 4096, MS_SYNC) < 0) {
        perror("msync");
    }
    
    printf("Modified content: %s", addr);
    
    munmap(addr, 4096);
    close(fd);
    unlink(path);
    
    return 0;
}
