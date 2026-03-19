/*
 * Auto-generated from: 04_Industrial_Scenarios\13_Linux_Kernel\03_Device_Driver_Development.md
 * Line: 1237
 * Language: c
 * Block ID: 3a498c97
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * File: test_char_driver.c
 * Description: 字符设备驱动用户空间测试程序
 * Compile: gcc -o test_char_driver test_char_driver.c
 * Usage: sudo ./test_char_driver
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <poll.h>
#include <errno.h>

#define DEVICE_PATH "/dev/mychar0"

/* ioctl命令定义 (必须与驱动一致) */
#define MYCHAR_IOC_MAGIC    'M'
#define MYCHAR_IOC_RESET    _IO(MYCHAR_IOC_MAGIC, 0)
#define MYCHAR_IOC_GET_LEN  _IOR(MYCHAR_IOC_MAGIC, 1, int)
#define MYCHAR_IOC_SET_LEN  _IOW(MYCHAR_IOC_MAGIC, 2, int)
#define MYCHAR_IOC_GET_DATA _IOR(MYCHAR_IOC_MAGIC, 3, char[256])

void test_basic_io(int fd)
{
    char write_buf[] = "Hello, Kernel! This is a test message.";
    char read_buf[256];
    ssize_t n;

    printf("\n=== Test Basic I/O ===\n");

    /* 写入数据 */
    n = write(fd, write_buf, strlen(write_buf));
    if (n < 0) {
        perror("write failed");
        return;
    }
    printf("Wrote %zd bytes: %s\n", n, write_buf);

    /* 定位到文件开头 */
    if (lseek(fd, 0, SEEK_SET) < 0) {
        perror("lseek failed");
        return;
    }

    /* 读取数据 */
    memset(read_buf, 0, sizeof(read_buf));
    n = read(fd, read_buf, sizeof(read_buf));
    if (n < 0) {
        perror("read failed");
        return;
    }
    printf("Read %zd bytes: %s\n", n, read_buf);
}

void test_ioctl(int fd)
{
    int len;
    char data[256];
    int ret;

    printf("\n=== Test ioctl ===\n");

    /* 写入测试数据 */
    write(fd, "Test data for ioctl", 19);

    /* 获取数据长度 */
    ret = ioctl(fd, MYCHAR_IOC_GET_LEN, &len);
    if (ret < 0) {
        perror("ioctl GET_LEN failed");
        return;
    }
    printf("Current data length: %d\n", len);

    /* 获取数据副本 */
    memset(data, 0, sizeof(data));
    ret = ioctl(fd, MYCHAR_IOC_GET_DATA, data);
    if (ret < 0) {
        perror("ioctl GET_DATA failed");
        return;
    }
    printf("Data from ioctl: %s\n", data);

    /* 重置设备 */
    ret = ioctl(fd, MYCHAR_IOC_RESET);
    if (ret < 0) {
        perror("ioctl RESET failed");
        return;
    }
    printf("Device reset successfully\n");

    /* 验证重置 */
    ret = ioctl(fd, MYCHAR_IOC_GET_LEN, &len);
    printf("After reset, data length: %d\n", len);
}

void test_poll(int fd)
{
    struct pollfd pfd;
    char buf[256];
    int ret;

    printf("\n=== Test poll/select ===\n");

    pfd.fd = fd;
    pfd.events = POLLIN | POLLOUT;

    /* 检查初始状态 */
    ret = poll(&pfd, 1, 1000);
    if (ret < 0) {
        perror("poll failed");
        return;
    }
    printf("Initial state: %s%s\n",
           (pfd.revents & POLLIN) ? "readable " : "",
           (pfd.revents & POLLOUT) ? "writable" : "");

    /* 写入数据 */
    write(fd, "Poll test data", 14);
    lseek(fd, 0, SEEK_SET);

    /* 再次检查 */
    pfd.events = POLLIN;
    ret = poll(&pfd, 1, 1000);
    if (ret > 0 && (pfd.revents & POLLIN)) {
        memset(buf, 0, sizeof(buf));
        read(fd, buf, sizeof(buf));
        printf("Data available via poll: %s\n", buf);
    }
}

void test_nonblocking(int fd)
{
    int flags;
    char buf[256];
    ssize_t n;

    printf("\n=== Test Non-blocking I/O ===\n");

    /* 设置为非阻塞模式 */
    flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);

    /* 尝试从空设备读取 */
    ioctl(fd, MYCHAR_IOC_RESET);  /* 确保设备为空 */

    n = read(fd, buf, sizeof(buf));
    if (n < 0) {
        if (errno == EAGAIN) {
            printf("Non-blocking read returned EAGAIN as expected\n");
        } else {
            perror("unexpected read error");
        }
    } else {
        printf("Unexpected: read returned %zd\n", n);
    }

    /* 恢复阻塞模式 */
    fcntl(fd, F_SETFL, flags);
}

void test_multiple_writes(int fd)
{
    char buf[256];
    int i;
    ssize_t n;

    printf("\n=== Test Multiple Writes ===\n");

    /* 重置设备 */
    ioctl(fd, MYCHAR_IOC_RESET);

    /* 多次写入 */
    for (i = 0; i < 5; i++) {
        snprintf(buf, sizeof(buf), "Message %d: Test data line\n", i);
        n = write(fd, buf, strlen(buf));
        printf("Write %d: %zd bytes\n", i, n);
    }

    /* 读取所有数据 */
    lseek(fd, 0, SEEK_SET);
    printf("\nReading all data:\n");
    while ((n = read(fd, buf, sizeof(buf) - 1)) > 0) {
        buf[n] = '\0';
        printf("%s", buf);
    }
}

int main(void)
{
    int fd;

    printf("=== Character Device Driver Test ===\n");
    printf("Opening device: %s\n", DEVICE_PATH);

    fd = open(DEVICE_PATH, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        printf("Make sure to:\n");
        printf("1. Load the driver: sudo insmod char_driver_basic.ko\n");
        printf("2. Create device node: sudo mknod %s c <major> 0\n", DEVICE_PATH);
        printf("   (Check major number with: cat /proc/devices | grep mychar)\n");
        return 1;
    }

    printf("Device opened successfully\n");

    /* 运行各项测试 */
    test_basic_io(fd);
    test_ioctl(fd);
    test_poll(fd);
    test_nonblocking(fd);
    test_multiple_writes(fd);

    printf("\n=== All tests completed ===\n");

    close(fd);
    return 0;
}
