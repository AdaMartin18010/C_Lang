# Lab 5: Shell Lab - Unix Shell实现实验

## 1. 实验目标

### 1.1 总体目标

Shell Lab通过实现一个支持作业控制的Unix Shell来深入学习进程控制和信号处理。

| 目标 | 描述 |
|------|------|
| **进程控制** | fork, exec, waitpid的使用 |
| **信号处理** | 实现信号处理器 |
| **作业控制** | 前台/后台作业管理 |
| **I/O重定向** | 实现输入/输出重定向 |

### 1.2 功能需求

```
┌─────────────────────────────────────────┐
│           tsh (Tiny Shell)              │
├─────────────────────────────────────────┤
│ 基本功能:                               │
│   - 命令解析与执行                      │
│   - 内建命令 (quit, jobs, bg, fg)      │
│   - 前台/后台作业                       │
│   - 信号处理 (SIGINT, SIGTSTP, SIGCHLD) │
├─────────────────────────────────────────┤
│ 高级功能:                               │
│   - I/O重定向 (<, >)                   │
│   - 管道 (可选)                         │
│   - 作业挂起与恢复                      │
└─────────────────────────────────────────┘
```

### 1.3 评分标准

| 功能 | 分值 |
|------|------|
| 正确处理前台作业 | 20分 |
| 正确处理后台作业 | 20分 |
| SIGINT处理 | 10分 |
| SIGTSTP处理 | 10分 |
| 内建命令 | 20分 |
| 信号竞争处理 | 20分 |

---

## 2. 背景知识

### 2.1 进程控制基础

```c
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// 创建子进程
pid_t fork(void);
// 返回值: 0(子进程), 子进程PID(父进程), -1(错误)

// 执行新程序
int execve(const char *filename, char *const argv[], char *const envp[]);

// 等待子进程
pid_t waitpid(pid_t pid, int *status, int options);
// options: WNOHANG(非阻塞), WUNTRACED(报告停止), WCONTINUED(报告继续)

// 检查进程状态
WIFEXITED(status)      // 是否正常退出
WEXITSTATUS(status)    // 获取退出状态
WIFSIGNALED(status)    // 是否被信号终止
WTERMSIG(status)       // 获取终止信号
WIFSTOPPED(status)     // 是否被信号停止
WSTOPSIG(status)       // 获取停止信号
```

### 2.2 信号处理

```c
#include <signal.h>

// 信号处理器类型
typedef void (*sighandler_t)(int);

// 设置信号处理器
sighandler_t signal(int signum, sighandler_t handler);
int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);

// 阻塞信号
int sigemptyset(sigset_t *set);
int sigfillset(sigset_t *set);
int sigaddset(sigset_t *set, int signum);
int sigprocmask(int how, const sigset_t *set, sigset_t *oldset);

// 发送信号
int kill(pid_t pid, int sig);
int raise(int sig);
```

### 2.3 作业控制信号

```
SIGINT   (Ctrl+C)   - 中断前台作业
SIGTSTP  (Ctrl+Z)   - 停止前台作业
SIGCONT             - 继续被停止的作业
SIGCHLD             - 子进程状态改变
SIGTTIN             - 后台进程试图读取终端
SIGTTOU             - 后台进程试图写入终端
```

### 2.4 终端控制

```c
#include <unistd.h>

// 设置前台进程组
int tcsetpgrp(int fd, pid_t pgrp);
pid_t tcgetpgrp(int fd);

// 获取/设置进程组ID
pid_t getpgrp(void);
int setpgid(pid_t pid, pid_t pgid);
```

---

## 3. Shell架构

### 3.1 数据结构

```c
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
```

### 3.2 主循环

```c
int main(int argc, char *argv[]) {
    char cmdline[MAXLINE];
    int emit_prompt = 1;    // 默认显示提示符

    // 重定向stderr到stdout
    dup2(1, 2);

    // 初始化信号处理器
    signal(SIGINT, sigint_handler);    // Ctrl+C
    signal(SIGTSTP, sigtstp_handler);  // Ctrl+Z
    signal(SIGCHLD, sigchld_handler);  // 子进程结束
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);

    // 主循环
    while (1) {
        if (emit_prompt) {
            printf("tsh> ");
            fflush(stdout);
        }

        if ((fgets(cmdline, MAXLINE, stdin) == NULL) && ferror(stdin)) {
            app_error("fgets error");
        }

        if (feof(stdin)) {
            printf("\n");
            exit(0);
        }

        eval(cmdline);  // 执行命令
    }

    return 0;
}
```

---

## 4. 核心功能实现

### 4.1 命令解析

```c
// 解析命令行
int parseline(const char *cmdline, char **argv) {
    static char array[MAXLINE];
    char *buf = array;
    char *delim;
    int argc;
    int bg;

    strcpy(buf, cmdline);
    buf[strlen(buf) - 1] = ' ';  // 替换末尾换行符为空格

    // 跳过前导空格
    while (*buf && (*buf == ' ')) {
        buf++;
    }

    // 解析参数
    argc = 0;
    while ((delim = strchr(buf, ' '))) {
        argv[argc++] = buf;
        *delim = '\0';
        buf = delim + 1;

        // 跳过空格
        while (*buf && (*buf == ' ')) {
            buf++;
        }
    }
    argv[argc] = NULL;

    // 空命令行
    if (argc == 0) {
        return 1;
    }

    // 检查是否是后台作业
    bg = (*argv[argc - 1] == '&');
    if (bg) {
        argv[--argc] = NULL;
    }

    return bg;
}

// 解析重定向
void parse_redirects(char **argv, char **infile, char **outfile) {
    *infile = NULL;
    *outfile = NULL;

    for (int i = 0; argv[i] != NULL; i++) {
        if (strcmp(argv[i], "<") == 0) {
            *infile = argv[i + 1];
            argv[i] = NULL;
            i++;
        } else if (strcmp(argv[i], ">") == 0) {
            *outfile = argv[i + 1];
            argv[i] = NULL;
            i++;
        }
    }
}
```

### 4.2 命令执行

```c
// 执行命令
void eval(char *cmdline) {
    char *argv[MAXARGS];
    char buf[MAXLINE];
    int bg;
    pid_t pid;
    sigset_t mask, prev_mask;

    strcpy(buf, cmdline);
    bg = parseline(buf, argv);

    if (argv[0] == NULL) {
        return;  // 空命令
    }

    // 内建命令
    if (!builtin_cmd(argv)) {
        // 阻塞SIGCHLD信号，防止竞争条件
        sigemptyset(&mask);
        sigaddset(&mask, SIGCHLD);
        sigprocmask(SIG_BLOCK, &mask, &prev_mask);

        if ((pid = fork()) == 0) {
            // 子进程
            sigprocmask(SIG_SETMASK, &prev_mask, NULL);
            setpgid(0, 0);  // 创建新进程组

            // 处理I/O重定向
            char *infile, *outfile;
            parse_redirects(argv, &infile, &outfile);

            if (infile) {
                int fd = open(infile, O_RDONLY);
                if (fd < 0) {
                    printf("%s: No such file or directory\n", infile);
                    exit(1);
                }
                dup2(fd, STDIN_FILENO);
                close(fd);
            }

            if (outfile) {
                int fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                dup2(fd, STDOUT_FILENO);
                close(fd);
            }

            // 恢复默认信号处理
            signal(SIGINT, SIG_DFL);
            signal(SIGTSTP, SIG_DFL);

            if (execve(argv[0], argv, environ) < 0) {
                printf("%s: Command not found\n", argv[0]);
                exit(0);
            }
        }

        // 父进程
        if (!bg) {
            // 前台作业
            fg_pid = pid;
            addjob(jobs, pid, FG, cmdline);
            sigprocmask(SIG_SETMASK, &prev_mask, NULL);
            waitfg(pid);  // 等待前台作业完成
        } else {
            // 后台作业
            addjob(jobs, pid, BG, cmdline);
            sigprocmask(SIG_SETMASK, &prev_mask, NULL);
            printf("[%d] (%d) %s", pid2jid(pid), pid, cmdline);
        }
    }
}

// 内建命令
int builtin_cmd(char **argv) {
    if (!strcmp(argv[0], "quit")) {
        exit(0);
    }
    if (!strcmp(argv[0], "jobs")) {
        listjobs(jobs);
        return 1;
    }
    if (!strcmp(argv[0], "bg") || !strcmp(argv[0], "fg")) {
        do_bgfg(argv);
        return 1;
    }
    if (!strcmp(argv[0], "&")) {
        return 1;  // 忽略单独的&
    }
    return 0;  // 不是内建命令
}
```

### 4.3 信号处理器

```c
// SIGCHLD处理器 - 处理子进程终止或停止
void sigchld_handler(int sig) {
    int olderrno = errno;
    pid_t pid;
    int status;

    // 使用WNOHANG | WUNTRACED回收所有子进程
    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED | WCONTINUED)) > 0) {
        // 获取作业信息
        int jid = pid2jid(pid);

        if (WIFEXITED(status)) {
            // 正常终止
            if (verbose) {
                printf("Job [%d] (%d) terminated by exit %d\n",
                       jid, pid, WEXITSTATUS(status));
            }
            deletejob(jobs, pid);
        } else if (WIFSIGNALED(status)) {
            // 被信号终止
            printf("Job [%d] (%d) terminated by signal %d\n",
                   jid, pid, WTERMSIG(status));
            deletejob(jobs, pid);
        } else if (WIFSTOPPED(status)) {
            // 被信号停止
            printf("Job [%d] (%d) stopped by signal %d\n",
                   jid, pid, WSTOPSIG(status));
            getjobpid(jobs, pid)->state = ST;
        }
    }

    errno = olderrno;
}

// SIGINT处理器 - 终止前台作业
void sigint_handler(int sig) {
    int olderrno = errno;
    pid_t pid = fgpid(jobs);

    if (pid != 0) {
        // 向前台进程组发送SIGINT
        kill(-pid, SIGINT);
    }

    errno = olderrno;
}

// SIGTSTP处理器 - 停止前台作业
void sigtstp_handler(int sig) {
    int olderrno = errno;
    pid_t pid = fgpid(jobs);

    if (pid != 0) {
        // 向前台进程组发送SIGTSTP
        kill(-pid, SIGTSTP);
    }

    errno = olderrno;
}
```

### 4.4 作业管理

```c
// 等待前台作业完成
void waitfg(pid_t pid) {
    sigset_t mask;
    sigemptyset(&mask);

    // 忙等待（使用sigsuspend更优）
    while (fgpid(jobs) == pid) {
        sigsuspend(&mask);  // 暂时解除所有信号阻塞并等待
    }
}

// bg/fg命令处理
void do_bgfg(char **argv) {
    if (argv[1] == NULL) {
        printf("%s command requires PID or %%jobid argument\n", argv[0]);
        return;
    }

    job_t *job;
    int id;

    // 解析作业ID或PID
    if (argv[1][0] == '%') {
        // 作业ID
        id = atoi(&argv[1][1]);
        job = getjobjid(jobs, id);
        if (job == NULL) {
            printf("%%%d: No such job\n", id);
            return;
        }
    } else {
        // PID
        id = atoi(argv[1]);
        job = getjobpid(jobs, id);
        if (job == NULL) {
            printf("(%d): No such process\n", id);
            return;
        }
    }

    // 发送SIGCONT信号
    kill(-job->pid, SIGCONT);

    if (!strcmp(argv[0], "bg")) {
        // 后台继续
        job->state = BG;
        printf("[%d] (%d) %s", job->jid, job->pid, job->cmdline);
    } else {
        // 前台继续
        job->state = FG;
        fg_pid = job->pid;
        waitfg(job->pid);
    }
}
```

### 4.5 作业表操作

```c
// 初始化作业表
void clearjob(job_t *job) {
    job->pid = 0;
    job->jid = 0;
    job->state = UNDEF;
    job->cmdline[0] = '\0';
}

void initjobs(job_t *jobs) {
    for (int i = 0; i < MAXJOBS; i++) {
        clearjob(&jobs[i]);
    }
}

// 添加作业
int addjob(job_t *jobs, pid_t pid, job_state_t state, char *cmdline) {
    if (pid < 1) {
        return 0;
    }

    for (int i = 0; i < MAXJOBS; i++) {
        if (jobs[i].pid == 0) {
            jobs[i].pid = pid;
            jobs[i].state = state;
            jobs[i].jid = next_jid++;
            strcpy(jobs[i].cmdline, cmdline);

            if (verbose) {
                printf("Added job [%d] %d %s\n",
                       jobs[i].jid, pid, cmdline);
            }
            return 1;
        }
    }
    printf("Tried to create too many jobs\n");
    return 0;
}

// 删除作业
int deletejob(job_t *jobs, pid_t pid) {
    if (pid < 1) {
        return 0;
    }

    for (int i = 0; i < MAXJOBS; i++) {
        if (jobs[i].pid == pid) {
            clearjob(&jobs[i]);
            next_jid = maxjid(jobs) + 1;
            return 1;
        }
    }
    return 0;
}

// 获取前台作业PID
pid_t fgpid(job_t *jobs) {
    for (int i = 0; i < MAXJOBS; i++) {
        if (jobs[i].state == FG) {
            return jobs[i].pid;
        }
    }
    return 0;
}

// 通过PID获取作业
job_t *getjobpid(job_t *jobs, pid_t pid) {
    if (pid < 1) {
        return NULL;
    }
    for (int i = 0; i < MAXJOBS; i++) {
        if (jobs[i].pid == pid) {
            return &jobs[i];
        }
    }
    return NULL;
}

// 通过JID获取作业
job_t *getjobjid(job_t *jobs, int jid) {
    if (jid < 1) {
        return NULL;
    }
    for (int i = 0; i < MAXJOBS; i++) {
        if (jobs[i].jid == jid) {
            return &jobs[i];
        }
    }
    return NULL;
}

// PID转JID
int pid2jid(pid_t pid) {
    if (pid < 1) {
        return 0;
    }
    for (int i = 0; i < MAXJOBS; i++) {
        if (jobs[i].pid == pid) {
            return jobs[i].jid;
        }
    }
    return 0;
}

// 列出所有作业
void listjobs(job_t *jobs) {
    for (int i = 0; i < MAXJOBS; i++) {
        if (jobs[i].pid != 0) {
            printf("[%d] (%d) ", jobs[i].jid, jobs[i].pid);
            switch (jobs[i].state) {
                case BG:
                    printf("Running ");
                    break;
                case FG:
                    printf("Foreground ");
                    break;
                case ST:
                    printf("Stopped ");
                    break;
                default:
                    printf("listjobs: Internal error state=%d ",
                           jobs[i].state);
            }
            printf("%s", jobs[i].cmdline);
        }
    }
}
```

---

## 5. 完整代码

### 5.1 tsh.c 完整实现

```c
/*
 * tsh - A tiny shell program with job control
 *
 * Usage: ./tsh
 * Then type commands at the prompt.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>

/* Misc manifest constants */
#define MAXLINE    1024   /* max line size */
#define MAXARGS     128   /* max args on a command line */
#define MAXJOBS      16   /* max jobs at any point in time */
#define MAXJID    (1<<16) /* max job ID */

/* Job states */
#define UNDEF 0 /* undefined */
#define FG 1    /* running in foreground */
#define BG 2    /* running in background */
#define ST 3    /* stopped */

/* Global variables */
int verbose = 0;            /* if true, print additional output */
int nextjid = 1;            /* next job ID to allocate */
char *last_bg_cmd = NULL;   /* last background command */

extern char **environ;      /* defined in libc */
char prompt[] = "tsh> ";    /* command line prompt */

typedef struct {
    pid_t pid;              /* job PID */
    int jid;                /* job ID [1, 2, ...] */
    int state;              /* UNDEF, BG, FG, or ST */
    char cmdline[MAXLINE];  /* command line */
} job_t;

job_t jobs[MAXJOBS];        /* The job list */

/* Function prototypes */
void eval(char *cmdline);
int builtin_cmd(char **argv);
void do_bgfg(char **argv);
void waitfg(pid_t pid);

void sigchld_handler(int sig);
void sigtstp_handler(int sig);
void sigint_handler(int sig);

int parseline(const char *cmdline, char **argv);
void sigquit_handler(int sig);

void clearjob(job_t *job);
void initjobs(job_t *jobs);
int maxjid(job_t *jobs);
int addjob(job_t *jobs, pid_t pid, int state, char *cmdline);
int deletejob(job_t *jobs, pid_t pid);
pid_t fgpid(job_t *jobs);
job_t *getjobpid(job_t *jobs, pid_t pid);
job_t *getjobjid(job_t *jobs, int jid);
int pid2jid(pid_t pid);
void listjobs(job_t *jobs);

void usage(void);
void unix_error(char *msg);
void app_error(char *msg);
typedef void handler_t(int);
handler_t *Signal(int signum, handler_t *handler);

/* main - The shell's main routine */
int main(int argc, char **argv) {
    char c;
    char cmdline[MAXLINE];
    int emit_prompt = 1;

    dup2(1, 2);

    while ((c = getopt(argc, argv, "hvp")) != EOF) {
        switch (c) {
            case 'h':
                usage();
                break;
            case 'v':
                verbose = 1;
                break;
            case 'p':
                emit_prompt = 0;
                break;
            default:
                usage();
        }
    }

    Signal(SIGINT, sigint_handler);    /* ctrl-c */
    Signal(SIGTSTP, sigtstp_handler);  /* ctrl-z */
    Signal(SIGCHLD, sigchld_handler);  /* terminated or stopped child */

    Signal(SIGTTIN, SIG_IGN);
    Signal(SIGTTOU, SIG_IGN);

    initjobs(jobs);

    while (1) {
        if (emit_prompt) {
            printf("%s", prompt);
            fflush(stdout);
        }
        if ((fgets(cmdline, MAXLINE, stdin) == NULL) && ferror(stdin))
            app_error("fgets error");
        if (feof(stdin)) {
            fflush(stdout);
            exit(0);
        }

        eval(cmdline);
        fflush(stdout);
    }

    exit(0);
}

/* eval - Evaluate a command line */
void eval(char *cmdline) {
    char *argv[MAXARGS];
    char buf[MAXLINE];
    int bg;
    pid_t pid;
    sigset_t mask;

    strcpy(buf, cmdline);
    bg = parseline(buf, argv);
    if (argv[0] == NULL)
        return;

    if (!builtin_cmd(argv)) {
        sigemptyset(&mask);
        sigaddset(&mask, SIGCHLD);
        sigprocmask(SIG_BLOCK, &mask, NULL);

        if ((pid = fork()) == 0) {
            sigprocmask(SIG_UNBLOCK, &mask, NULL);
            setpgid(0, 0);

            if (execve(argv[0], argv, environ) < 0) {
                printf("%s: Command not found.\n", argv[0]);
                exit(0);
            }
        }

        if (!bg) {
            addjob(jobs, pid, FG, cmdline);
            sigprocmask(SIG_UNBLOCK, &mask, NULL);
            waitfg(pid);
        } else {
            addjob(jobs, pid, BG, cmdline);
            sigprocmask(SIG_UNBLOCK, &mask, NULL);
            printf("[%d] (%d) %s", pid2jid(pid), pid, cmdline);
        }
    }
    return;
}

/* parseline - Parse the command line and build the argv array */
int parseline(const char *cmdline, char **argv) {
    static char array[MAXLINE];
    char *buf = array;
    char *delim;
    int argc;
    int bg;

    strcpy(buf, cmdline);
    buf[strlen(buf)-1] = ' ';
    while (*buf && (*buf == ' '))
        buf++;

    argc = 0;
    while ((delim = strchr(buf, ' '))) {
        argv[argc++] = buf;
        *delim = '\0';
        buf = delim + 1;
        while (*buf && (*buf == ' '))
            buf++;
    }
    argv[argc] = NULL;

    if (argc == 0)
        return 1;

    if ((bg = (*argv[argc-1] == '&')) != 0)
        argv[--argc] = NULL;

    return bg;
}

/* builtin_cmd - If the user has typed a built-in command then execute it */
int builtin_cmd(char **argv) {
    if (!strcmp(argv[0], "quit"))
        exit(0);
    if (!strcmp(argv[0], "&"))
        return 1;
    if (!strcmp(argv[0], "jobs")) {
        listjobs(jobs);
        return 1;
    }
    if (!strcmp(argv[0], "bg") || !strcmp(argv[0], "fg")) {
        do_bgfg(argv);
        return 1;
    }
    return 0;
}

/* do_bgfg - Execute the builtin bg and fg commands */
void do_bgfg(char **argv) {
    struct job_t *job;
    char *id;
    int jid;
    pid_t pid;

    if (argv[1] == NULL) {
        printf("%s command requires PID or %%jobid argument\n", argv[0]);
        return;
    }

    id = argv[1];
    if (id[0] == '%') {
        jid = atoi(&id[1]);
        job = getjobjid(jobs, jid);
        if (job == NULL) {
            printf("%%%d: No such job\n", jid);
            return;
        }
    } else {
        pid = atoi(id);
        job = getjobpid(jobs, pid);
        if (job == NULL) {
            printf("(%d): No such process\n", pid);
            return;
        }
    }

    kill(-job->pid, SIGCONT);

    if (!strcmp(argv[0], "bg")) {
        job->state = BG;
        printf("[%d] (%d) %s", job->jid, job->pid, job->cmdline);
    } else {
        job->state = FG;
        waitfg(job->pid);
    }
    return;
}

/* waitfg - Block until process pid is no longer the foreground process */
void waitfg(pid_t pid) {
    sigset_t mask;
    sigemptyset(&mask);

    while (fgpid(jobs) == pid)
        sigsuspend(&mask);
    return;
}

/* Signal handlers */

/* sigchld_handler - The kernel sends a SIGCHLD to the shell whenever
 *     a child job terminates (becomes a zombie), or stops because it
 *     received a SIGSTOP or SIGTSTP signal. */
void sigchld_handler(int sig) {
    int olderrno = errno;
    pid_t pid;
    int status;

    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED)) > 0) {
        if (WIFEXITED(status)) {
            deletejob(jobs, pid);
        } else if (WIFSIGNALED(status)) {
            printf("Job [%d] (%d) terminated by signal %d\n",
                   pid2jid(pid), pid, WTERMSIG(status));
            deletejob(jobs, pid);
        } else if (WIFSTOPPED(status)) {
            printf("Job [%d] (%d) stopped by signal %d\n",
                   pid2jid(pid), pid, WSTOPSIG(status));
            getjobpid(jobs, pid)->state = ST;
        }
    }

    errno = olderrno;
    return;
}

/* sigint_handler - The kernel sends a SIGINT to the shell whenver the
 *    user types ctrl-c at the keyboard. */
void sigint_handler(int sig) {
    int olderrno = errno;
    pid_t pid = fgpid(jobs);

    if (pid != 0)
        kill(-pid, SIGINT);

    errno = olderrno;
    return;
}

/* sigtstp_handler - The kernel sends a SIGTSTP to the shell whenever
 *     the user types ctrl-z at the keyboard. */
void sigtstp_handler(int sig) {
    int olderrno = errno;
    pid_t pid = fgpid(jobs);

    if (pid != 0)
        kill(-pid, SIGTSTP);

    errno = olderrno;
    return;
}

/* Job list helper routines */

void clearjob(struct job_t *job) {
    job->pid = 0;
    job->jid = 0;
    job->state = UNDEF;
    job->cmdline[0] = '\0';
}

void initjobs(struct job_t *jobs) {
    int i;
    for (i = 0; i < MAXJOBS; i++)
        clearjob(&jobs[i]);
}

int maxjid(struct job_t *jobs) {
    int i, max = 0;
    for (i = 0; i < MAXJOBS; i++)
        if (jobs[i].jid > max)
            max = jobs[i].jid;
    return max;
}

int addjob(struct job_t *jobs, pid_t pid, int state, char *cmdline) {
    int i;
    if (pid < 1)
        return 0;

    for (i = 0; i < MAXJOBS; i++) {
        if (jobs[i].pid == 0) {
            jobs[i].pid = pid;
            jobs[i].state = state;
            jobs[i].jid = nextjid++;
            strcpy(jobs[i].cmdline, cmdline);
            return 1;
        }
    }
    printf("Tried to create too many jobs\n");
    return 0;
}

int deletejob(struct job_t *jobs, pid_t pid) {
    int i;
    if (pid < 1)
        return 0;

    for (i = 0; i < MAXJOBS; i++) {
        if (jobs[i].pid == pid) {
            clearjob(&jobs[i]);
            nextjid = maxjid(jobs)+1;
            return 1;
        }
    }
    return 0;
}

pid_t fgpid(struct job_t *jobs) {
    int i;
    for (i = 0; i < MAXJOBS; i++)
        if (jobs[i].state == FG)
            return jobs[i].pid;
    return 0;
}

struct job_t *getjobpid(struct job_t *jobs, pid_t pid) {
    int i;
    if (pid < 1)
        return NULL;
    for (i = 0; i < MAXJOBS; i++)
        if (jobs[i].pid == pid)
            return &jobs[i];
    return NULL;
}

struct job_t *getjobjid(struct job_t *jobs, int jid) {
    int i;
    if (jid < 1)
        return NULL;
    for (i = 0; i < MAXJOBS; i++)
        if (jobs[i].jid == jid)
            return &jobs[i];
    return NULL;
}

int pid2jid(pid_t pid) {
    int i;
    if (pid < 1)
        return 0;
    for (i = 0; i < MAXJOBS; i++)
        if (jobs[i].pid == pid)
            return jobs[i].jid;
    return 0;
}

void listjobs(struct job_t *jobs) {
    int i;
    for (i = 0; i < MAXJOBS; i++) {
        if (jobs[i].pid != 0) {
            printf("[%d] (%d) ", jobs[i].jid, jobs[i].pid);
            switch (jobs[i].state) {
                case BG:
                    printf("Running ");
                    break;
                case FG:
                    printf("Foreground ");
                    break;
                case ST:
                    printf("Stopped ");
                    break;
                default:
                    printf("listjobs: Internal error state=%d ",
                           jobs[i].state);
            }
            printf("%s", jobs[i].cmdline);
        }
    }
}

void usage(void) {
    printf("Usage: shell [-hvp]\n");
    printf("   -h   print this message\n");
    printf("   -v   print additional diagnostic information\n");
    printf("   -p   do not emit a command prompt\n");
    exit(1);
}

void unix_error(char *msg) {
    fprintf(stdout, "%s: %s\n", msg, strerror(errno));
    exit(1);
}

void app_error(char *msg) {
    fprintf(stdout, "%s\n", msg);
    exit(1);
}

handler_t *Signal(int signum, handler_t *handler) {
    struct sigaction action, old_action;
    action.sa_handler = handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_RESTART;
    if (sigaction(signum, &action, &old_action) < 0)
        unix_error("Signal error");
    return (old_action.sa_handler);
}
```

---

## 6. 测试方法

```bash
# 编译
make

# 基本测试
./tsh

# 运行测试套件
make test

# 使用sdriver测试
./sdriver.pl -t trace01.txt -s ./tsh -a "-p"
./sdriver.pl -t trace15.txt -s ./tsh -a "-p"

# 与参考shell对比
./sdriver.pl -t trace15.txt -s ./tshref -a "-p"
```

---

**创建日期**: 2026-03-19
