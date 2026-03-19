/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\POSIX_1_2024\01_POSIX_System_Programming.md
 * Line: 1804
 * Language: c
 * Block ID: a64d9b28
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file posix_sysv_ipc.c
 * @brief System V IPC机制
 * @description 包含消息队列、共享内存、信号量
 */

#define _POSIX_C_SOURCE 202405L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <errno.h>

#define MSG_KEY 1234
#define SHM_KEY 5678
#define SEM_KEY 9012
#define MSG_SIZE 256

/**
 * @brief 消息结构
 */
struct msg_buffer {
    long msg_type;
    char msg_text[MSG_SIZE];
};

/**
 * @brief 消息队列演示
 */
void message_queue_demo(void) {
    int msgid;
    pid_t pid;
    struct msg_buffer message;

    printf("\n=== System V 消息队列 ===\n");

    /* 创建消息队列 */
    msgid = msgget(MSG_KEY, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget");
        return;
    }

    pid = fork();
    if (pid == 0) {
        /* 子进程：接收消息 */
        sleep(1);  /* 确保父进程先发送 */

        /* 接收类型为1的消息 */
        if (msgrcv(msgid, &message, sizeof(message.msg_text), 1, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }

        printf("子进程收到消息: %s\n", message.msg_text);

        /* 发送回复 */
        message.msg_type = 2;  /* 不同类型 */
        strcpy(message.msg_text, "Reply from child process");
        msgsnd(msgid, &message, strlen(message.msg_text) + 1, 0);

        exit(0);
    } else {
        /* 父进程：发送消息 */
        message.msg_type = 1;
        strcpy(message.msg_text, "Hello from parent process");

        if (msgsnd(msgid, &message, strlen(message.msg_text) + 1, 0) == -1) {
            perror("msgsnd");
        } else {
            printf("父进程发送消息: %s\n", message.msg_text);
        }

        /* 等待子进程回复 */
        if (msgrcv(msgid, &message, sizeof(message.msg_text), 2, 0) != -1) {
            printf("父进程收到回复: %s\n", message.msg_text);
        }

        wait(NULL);

        /* 删除消息队列 */
        msgctl(msgid, IPC_RMID, NULL);
    }
}

/**
 * @brief 共享内存演示
 */
void shared_memory_demo(void) {
    int shmid;
    pid_t pid;
    char *shm_addr;
    const char *msg = "Hello from shared memory!";

    printf("\n=== System V 共享内存 ===\n");

    /* 创建共享内存段 */
    shmid = shmget(SHM_KEY, 1024, 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("shmget");
        return;
    }

    pid = fork();
    if (pid == 0) {
        /* 子进程 */
        sleep(1);

        /* 附加共享内存 */
        shm_addr = (char *)shmat(shmid, NULL, 0);
        if (shm_addr == (char *)-1) {
            perror("shmat");
            exit(1);
        }

        printf("子进程从共享内存读取: %s\n", shm_addr);

        /* 写入回复 */
        strcpy(shm_addr, "Child was here!");

        /* 分离共享内存 */
        shmdt(shm_addr);
        exit(0);
    } else {
        /* 父进程 */
        shm_addr = (char *)shmat(shmid, NULL, 0);
        if (shm_addr == (char *)-1) {
            perror("shmat");
            return;
        }

        /* 写入数据 */
        strcpy(shm_addr, msg);
        printf("父进程写入共享内存: %s\n", msg);

        wait(NULL);

        printf("父进程读取共享内存: %s\n", shm_addr);

        /* 分离并删除共享内存 */
        shmdt(shm_addr);
        shmctl(shmid, IPC_RMID, NULL);
    }
}

/**
 * @brief 信号量操作（PV操作）
 */
void semaphore_demo(void) {
    int semid;
    pid_t pid;
    struct sembuf sb;
    int shared_value = 0;  /* 注意：实际应该在共享内存中 */

    printf("\n=== System V 信号量 ===\n");

    /* 创建信号量集，包含1个信号量 */
    semid = semget(SEM_KEY, 1, 0666 | IPC_CREAT);
    if (semid == -1) {
        perror("semget");
        return;
    }

    /* 初始化信号量值为1（二进制信号量/互斥锁） */
    if (semctl(semid, 0, SETVAL, 1) == -1) {
        perror("semctl SETVAL");
        return;
    }

    pid = fork();
    if (pid == 0) {
        /* 子进程 */
        sleep(1);

        /* P操作：等待信号量 */
        sb.sem_num = 0;
        sb.sem_op = -1;  /* 减1 */
        sb.sem_flg = 0;

        printf("子进程: 等待信号量...\n");
        if (semop(semid, &sb, 1) == -1) {
            perror("semop P");
            exit(1);
        }

        printf("子进程: 获得信号量，访问临界区\n");
        sleep(2);  /* 模拟临界区操作 */
        printf("子进程: 离开临界区\n");

        /* V操作：释放信号量 */
        sb.sem_op = 1;  /* 加1 */
        semop(semid, &sb, 1);

        exit(0);
    } else {
        /* 父进程 */
        /* P操作 */
        sb.sem_num = 0;
        sb.sem_op = -1;
        sb.sem_flg = 0;

        printf("父进程: 等待信号量...\n");
        semop(semid, &sb, 1);

        printf("父进程: 获得信号量，访问临界区\n");
        sleep(2);
        printf("父进程: 离开临界区\n");

        /* V操作 */
        sb.sem_op = 1;
        semop(semid, &sb, 1);

        wait(NULL);

        /* 删除信号量集 */
        semctl(semid, 0, IPC_RMID);
    }

    printf("信号量同步完成\n");
}

int main(void) {
    printf("=== System V IPC演示 ===\n");

    message_queue_demo();
    shared_memory_demo();
    semaphore_demo();

    printf("\n=== System V IPC演示完成 ===\n");
    return 0;
}
