/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\05_Shell_Lab.md
 * Line: 131
 * Language: c
 * Block ID: ad3ea352
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// tsh.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#define MAXLINE    1024   // 最大命令行长度
#define MAXARGS     128   // 最大参数个数
#define MAXJOBS      16   // 最大作业数
#define MAXJID    (1<<16) // 最大作业ID

typedef enum {
    UNDEF,  // 未定义
    FG,     // 前台作业
    BG,     // 后台作业
    ST      // 停止的作业
} job_state_t;

typedef struct {
    pid_t pid;              // 进程ID
    int jid;                // 作业ID [1, 2, ...]
    job_state_t state;      // 作业状态
    char cmdline[MAXLINE];  // 命令行
} job_t;

// 全局变量
job_t jobs[MAXJOBS];        // 作业表
int next_jid = 1;           // 下一个作业ID
volatile sig_atomic_t fg_pid = 0;  // 前台进程PID
