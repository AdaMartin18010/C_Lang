/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\POSIX_1_2024\01_POSIX_System_Programming.md
 * Line: 2045
 * Language: c
 * Block ID: 61b96dfb
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file posix_ipc.c
 * @brief POSIX IPC机制
 * @description 使用POSIX命名的共享内存和信号量
 */

#define _POSIX_C_SOURCE 202405L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <errno.h>

#define SHM_NAME "/posix_shm_demo"
#define SEM_NAME "/posix_sem_demo"
#define SHM_SIZE 1024

/**
 * @brief POSIX共享内存演示
 */
void posix_shm_demo(void) {
    int shm_fd;
    pid_t pid;
    char *shm_ptr;
    const char *msg = "Hello from POSIX shared memory!";

    printf("\n=== POSIX 共享内存 ===\n");

    /* 删除可能存在的旧共享内存 */
    shm_unlink(SHM_NAME);

    pid = fork();
    if (pid == 0) {
        /* 子进程 */
        sleep(1);

        /* 打开共享内存对象 */
        shm_fd = shm_open(SHM_NAME, O_RDONLY, 0666);
        if (shm_fd == -1) {
            perror("shm_open (child)");
            exit(1);
        }

        /* 映射到地址空间 */
        shm_ptr = mmap(NULL, SHM_SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
        if (shm_ptr == MAP_FAILED) {
            perror("mmap");
            close(shm_fd);
            exit(1);
        }

        printf("子进程读取: %s\n", shm_ptr);

        munmap(shm_ptr, SHM_SIZE);
        close(shm_fd);
        exit(0);
    } else {
        /* 父进程 */
        /* 创建共享内存对象 */
        shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
        if (shm_fd == -1) {
            perror("shm_open (parent)");
            return;
        }

        /* 设置共享内存大小 */
        if (ftruncate(shm_fd, SHM_SIZE) == -1) {
            perror("ftruncate");
            close(shm_fd);
            return;
        }

        /* 映射到地址空间 */
        shm_ptr = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
        if (shm_ptr == MAP_FAILED) {
            perror("mmap");
            close(shm_fd);
            return;
        }

        /* 写入数据 */
        strcpy(shm_ptr, msg);
        printf("父进程写入: %s\n", msg);

        wait(NULL);

        munmap(shm_ptr, SHM_SIZE);
        close(shm_fd);
        shm_unlink(SHM_NAME);
    }
}

/**
 * @brief POSIX命名信号量演示
 */
void posix_named_semaphore_demo(void) {
    sem_t *sem;
    pid_t pid;

    printf("\n=== POSIX 命名信号量 ===\n");

    /* 删除可能存在的旧信号量 */
    sem_unlink(SEM_NAME);

    /* 创建信号量，初始值为1 */
    sem = sem_open(SEM_NAME, O_CREAT, 0666, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        return;
    }

    pid = fork();
    if (pid == 0) {
        /* 子进程 */
        sleep(1);

        printf("子进程: 等待信号量...\n");
        sem_wait(sem);  /* P操作 */

        printf("子进程: 进入临界区\n");
        sleep(2);
        printf("子进程: 离开临界区\n");

        sem_post(sem);  /* V操作 */

        sem_close(sem);
        exit(0);
    } else {
        /* 父进程 */
        printf("父进程: 等待信号量...\n");
        sem_wait(sem);

        printf("父进程: 进入临界区\n");
        sleep(2);
        printf("父进程: 离开临界区\n");

        sem_post(sem);

        wait(NULL);

        sem_close(sem);
        sem_unlink(SEM_NAME);
    }
}

/**
 * @brief POSIX无名信号量演示（用于线程）
 */
void posix_unnamed_semaphore_demo(void) {
    sem_t sem;

    printf("\n=== POSIX 无名信号量（进程内）===\n");

    /* 初始化无名信号量，进程内共享 */
    if (sem_init(&sem, 0, 1) == -1) {
        perror("sem_init");
        return;
    }

    printf("获取信号量...\n");
    sem_wait(&sem);

    printf("在临界区内操作...\n");

    sem_post(&sem);
    printf("释放信号量\n");

    /* 销毁信号量 */
    sem_destroy(&sem);
}

int main(void) {
    printf("=== POSIX IPC演示 ===\n");

    posix_shm_demo();
    posix_named_semaphore_demo();
    posix_unnamed_semaphore_demo();

    printf("\n=== POSIX IPC演示完成 ===\n");
    return 0;
}
