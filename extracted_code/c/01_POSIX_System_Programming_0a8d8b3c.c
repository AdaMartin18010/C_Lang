/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\POSIX_1_2024\01_POSIX_System_Programming.md
 * Line: 4238
 * Language: c
 * Block ID: 0a8d8b3c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file posix_shell.c
 * @brief POSIX兼容Shell实现
 * @description 一个简化但功能完整的Shell，展示POSIX系统编程实践
 * 
 * 功能特性:
 * - 命令执行（带参数）
 * - 输入/输出重定向 (<, >, >>)
 * - 管道 (|)
 * - 后台执行 (&)
 * - 环境变量管理
 * - 信号处理
 * - 内置命令 (cd, pwd, exit, export, jobs, fg, bg)
 */

#define _POSIX_C_SOURCE 202405L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <ctype.h>
#include <pwd.h>
#include <readline/readline.h>
#include <readline/history.h>

/* 常量定义 */
#define MAX_INPUT 4096
#define MAX_ARGS 256
#define MAX_PIPES 16
#define MAX_JOBS 32
#define MAX_ENV 256
#define PROMPT_SIZE 256

/* 作业状态 */
typedef enum {
    JOB_RUNNING,
    JOB_STOPPED,
    JOB_DONE
} JobStatus;

/* 作业结构 */
typedef struct {
    pid_t pgid;
    char command[MAX_INPUT];
    JobStatus status;
    int background;
} Job;

/* 全局变量 */
static Job jobs[MAX_JOBS];
static int num_jobs = 0;
static int shell_terminal;
static pid_t shell_pgid;
static volatile sig_atomic_t got_sigchld = 0;

/**
 * @brief 初始化Shell环境
 */
void init_shell(void) {
    /* 确保Shell在交互式模式下运行在前台 */
    shell_terminal = STDIN_FILENO;
    shell_pgid = getpgrp();
    
    /* 如果Shell不是进程组长，则设置为进程组长 */
    while (tcgetpgrp(shell_terminal) != shell_pgid) {
        kill(-shell_pgid, SIGTTIN);
    }
    
    /* 忽略交互式作业控制信号 */
    signal(SIGINT, SIG_IGN);
    signal(SIGQUIT, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    
    /* 将自己放入新的进程组，并获取终端控制 */
    shell_pgid = getpid();
    if (setpgid(shell_pgid, shell_pgid) < 0) {
        perror("Couldn't put the shell in its own process group");
        exit(1);
    }
    
    tcsetpgrp(shell_terminal, shell_pgid);
    tcgetattr(shell_terminal, &shell_tmodes);
    
    /* 初始化作业表 */
    memset(jobs, 0, sizeof(jobs));
}

/**
 * @brief 创建提示符
 */
void make_prompt(char *prompt, size_t size) {
    char cwd[1024];
    const char *home = getenv("HOME");
    
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        /* 用~替换HOME目录 */
        if (home && strstr(cwd, home) == cwd) {
            snprintf(prompt, size, "[POSIX-Shell] ~%s$ ", cwd + strlen(home));
        } else {
            snprintf(prompt, size, "[POSIX-Shell] %s$ ", cwd);
        }
    } else {
        snprintf(prompt, size, "[POSIX-Shell] $ ");
    }
}

/**
 * @brief 解析命令行参数
 * @param line 输入行
 * @param args 参数数组
 * @return 参数数量
 */
int parse_args(char *line, char **args) {
    int argc = 0;
    char *p = line;
    
    while (*p && argc < MAX_ARGS - 1) {
        /* 跳过空白 */
        while (*p && isspace((unsigned char)*p)) p++;
        if (!*p) break;
        
        /* 处理引号 */
        if (*p == '"' || *p == '\'') {
            char quote = *p++;
            args[argc] = p;
            while (*p && *p != quote) p++;
            if (*p) *p++ = '\0';
        } else {
            args[argc] = p;
            while (*p && !isspace((unsigned char)*p)) p++;
            if (*p) *p++ = '\0';
        }
        argc++;
    }
    
    args[argc] = NULL;
    return argc;
}

/**
 * @brief 查找重定向符号
 * @param args 参数数组
 * @param symbol 重定向符号
 * @return 重定向目标文件名，NULL表示没有找到
 */
char *find_redirect(char **args, const char *symbol) {
    for (int i = 0; args[i]; i++) {
        if (strcmp(args[i], symbol) == 0) {
            if (args[i + 1]) {
                /* 移除重定向部分 */
                char *file = strdup(args[i + 1]);
                args[i] = NULL;
                return file;
            }
        }
    }
    return NULL;
}

/**
 * @brief 处理输入重定向
 */
int handle_input_redirect(char *filename) {
    if (!filename) return 0;
    
    int fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror(filename);
        free(filename);
        return -1;
    }
    
    dup2(fd, STDIN_FILENO);
    close(fd);
    free(filename);
    return 0;
}

/**
 * @brief 处理输出重定向
 */
int handle_output_redirect(char *filename, int append) {
    if (!filename) return 0;
    
    int flags = O_WRONLY | O_CREAT;
    if (append) {
        flags |= O_APPEND;
    } else {
        flags |= O_TRUNC;
    }
    
    int fd = open(filename, flags, 0644);
    if (fd < 0) {
        perror(filename);
        free(filename);
        return -1;
    }
    
    dup2(fd, STDOUT_FILENO);
    close(fd);
    free(filename);
    return 0;
}

/**
 * @brief 执行外部命令
 */
int execute_command(char **args, int background) {
    pid_t pid;
    int status;
    
    /* 处理重定向 */
    char *infile = find_redirect(args, "<");
    char *outfile = find_redirect(args, ">");
    char *appendfile = find_redirect(args, ">>");
    
    if (outfile && appendfile) {
        fprintf(stderr, "Cannot use both > and >>\n");
        free(infile);
        free(outfile);
        free(appendfile);
        return 1;
    }
    
    pid = fork();
    if (pid == 0) {
        /* 子进程 */
        signal(SIGINT, SIG_DFL);
        signal(SIGQUIT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        signal(SIGTTIN, SIG_DFL);
        signal(SIGTTOU, SIG_DFL);
        
        /* 处理重定向 */
        if (infile && handle_input_redirect(infile) < 0) {
            exit(1);
        }
        if (outfile && handle_output_redirect(outfile, 0) < 0) {
            exit(1);
        }
        if (appendfile && handle_output_redirect(appendfile, 1) < 0) {
            exit(1);
        }
        
        /* 执行命令 */
        execvp(args[0], args);
        perror(args[0]);
        exit(127);
    } else if (pid < 0) {
        perror("fork");
        return 1;
    }
    
    /* 父进程 */
    if (!background) {
        waitpid(pid, &status, WUNTRACED);
        return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
    } else {
        printf("[%d] %d\n", num_jobs + 1, pid);
        /* 添加到作业表 */
        if (num_jobs < MAX_JOBS) {
            jobs[num_jobs].pgid = pid;
            strncpy(jobs[num_jobs].command, args[0], MAX_INPUT - 1);
            jobs[num_jobs].status = JOB_RUNNING;
            jobs[num_jobs].background = 1;
            num_jobs++;
        }
        return 0;
    }
}

/**
 * @brief 执行管道命令
 */
int execute_pipeline(char **commands, int num_commands) {
    int pipes[MAX_PIPES][2];
    pid_t pids[MAX_PIPES];
    char *args[MAX_ARGS];
    
    /* 创建所有管道 */
    for (int i = 0; i < num_commands - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            return 1;
        }
    }
    
    /* 创建子进程 */
    for (int i = 0; i < num_commands; i++) {
        parse_args(commands[i], args);
        if (!args[0]) continue;
        
        pids[i] = fork();
        if (pids[i] == 0) {
            /* 子进程 */
            signal(SIGINT, SIG_DFL);
            
            /* 如果不是第一个命令，从上一个管道读取 */
            if (i > 0) {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }
            /* 如果不是最后一个命令，写入下一个管道 */
            if (i < num_commands - 1) {
                dup2(pipes[i][1], STDOUT_FILENO);
            }
            
            /* 关闭所有管道描述符 */
            for (int j = 0; j < num_commands - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            
            execvp(args[0], args);
            perror(args[0]);
            exit(127);
        } else if (pids[i] < 0) {
            perror("fork");
            return 1;
        }
    }
    
    /* 父进程：关闭所有管道 */
    for (int i = 0; i < num_commands - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    
    /* 等待所有子进程 */
    int status;
    for (int i = 0; i < num_commands; i++) {
        waitpid(pids[i], &status, 0);
    }
    
    return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
}

/**
 * @brief 内置命令: cd
 */
int builtin_cd(char **args) {
    const char *dir = args[1];
    if (!dir) {
        dir = getenv("HOME");
        if (!dir) {
            fprintf(stderr, "cd: HOME not set\n");
            return 1;
        }
    }
    
    if (chdir(dir) != 0) {
        perror("cd");
        return 1;
    }
    return 0;
}

/**
 * @brief 内置命令: pwd
 */
int builtin_pwd(void) {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd))) {
        printf("%s\n", cwd);
        return 0;
    }
    perror("pwd");
    return 1;
}

/**
 * @brief 内置命令: export
 */
int builtin_export(char **args) {
    if (!args[1]) {
        /* 打印所有环境变量 */
        extern char **environ;
        for (char **env = environ; *env; env++) {
            printf("%s\n", *env);
        }
        return 0;
    }
    
    /* 设置环境变量 */
    char *eq = strchr(args[1], '=');
    if (eq) {
        *eq = '\0';
        setenv(args[1], eq + 1, 1);
    } else {
        /* 没有值，只设置为空 */
        setenv(args[1], "", 1);
    }
    return 0;
}

/**
 * @brief 内置命令: jobs
 */
int builtin_jobs(void) {
    for (int i = 0; i < num_jobs; i++) {
        int status;
        pid_t result = waitpid(jobs[i].pgid, &status, WNOHANG);
        
        if (result == 0) {
            /* 仍在运行 */
            printf("[%d] %s %s\n", i + 1,
                   jobs[i].status == JOB_RUNNING ? "Running" : "Stopped",
                   jobs[i].command);
        } else {
            jobs[i].status = JOB_DONE;
        }
    }
    return 0;
}

/**
 * @brief 内置命令: echo
 */
int builtin_echo(char **args) {
    for (int i = 1; args[i]; i++) {
        printf("%s", args[i]);
        if (args[i + 1]) printf(" ");
    }
    printf("\n");
    return 0;
}

/**
 * @brief 检查并执行内置命令
 * @return 1表示执行了内置命令，0表示不是内置命令
 */
int try_builtin(char **args) {
    if (!args[0]) return 1;
    
    if (strcmp(args[0], "cd") == 0) {
        builtin_cd(args);
        return 1;
    }
    if (strcmp(args[0], "pwd") == 0) {
        builtin_pwd();
        return 1;
    }
    if (strcmp(args[0], "exit") == 0) {
        exit(args[1] ? atoi(args[1]) : 0);
    }
    if (strcmp(args[0], "export") == 0) {
        builtin_export(args);
        return 1;
    }
    if (strcmp(args[0], "jobs") == 0) {
        builtin_jobs();
        return 1;
    }
    if (strcmp(args[0], "echo") == 0) {
        builtin_echo(args);
        return 1;
    }
    
    return 0;
}

/**
 * @brief 解析管道
 * @param line 输入行
 * @param commands 命令数组
 * @return 命令数量
 */
int parse_pipes(char *line, char **commands) {
    int num = 0;
    char *token = strtok(line, "|");
    
    while (token && num < MAX_PIPES) {
        /* 去掉前后空白 */
        while (*token && isspace((unsigned char)*token)) token++;
        char *end = token + strlen(token) - 1;
        while (end > token && isspace((unsigned char)*end)) *end-- = '\0';
        
        commands[num++] = token;
        token = strtok(NULL, "|");
    }
    
    return num;
}

/**
 * @brief SIGCHLD信号处理
 */
void sigchld_handler(int sig) {
    (void)sig;
    got_sigchld = 1;
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

/**
 * @brief 主循环
 */
void shell_loop(void) {
    char *input;
    char prompt[PROMPT_SIZE];
    char *commands[MAX_PIPES];
    char *args[MAX_ARGS];
    
    /* 设置SIGCHLD处理 */
    signal(SIGCHLD, sigchld_handler);
    
    while (1) {
        make_prompt(prompt, sizeof(prompt));
        
        /* 读取输入 */
        input = readline(prompt);
        if (!input) {
            /* EOF */
            printf("\nexit\n");
            break;
        }
        
        /* 忽略空行 */
        char *p = input;
        while (*p && isspace((unsigned char)*p)) p++;
        if (!*p) {
            free(input);
            continue;
        }
        
        /* 添加到历史 */
        add_history(input);
        
        /* 检查后台执行 */
        int background = 0;
        size_t len = strlen(input);
        if (len > 0 && input[len - 1] == '&') {
            background = 1;
            input[len - 1] = '\0';
            /* 去掉末尾空白 */
            len--;
            while (len > 0 && isspace((unsigned char)input[len - 1])) {
                input[--len] = '\0';
            }
        }
        
        /* 解析管道 */
        int num_commands = parse_pipes(input, commands);
        
        if (num_commands == 1) {
            /* 单个命令 */
            parse_args(commands[0], args);
            if (!try_builtin(args)) {
                execute_command(args, background);
            }
        } else if (num_commands > 1) {
            /* 管道命令 */
            execute_pipeline(commands, num_commands);
        }
        
        /* 处理完成的作业 */
        if (got_sigchld) {
            got_sigchld = 0;
            for (int i = 0; i < num_jobs; i++) {
                int status;
                pid_t result = waitpid(jobs[i].pgid, &status, WNOHANG);
                if (result > 0) {
                    printf("[%d]+ Done\t%s\n", i + 1, jobs[i].command);
                    jobs[i].status = JOB_DONE;
                }
            }
        }
        
        free(input);
    }
}

/**
 * @brief 主函数
 */
int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    
    printf("╔══════════════════════════════════════════╗\n");
    printf("║     POSIX Shell - 系统编程示例           ║\n");
    printf("║  演示: fork, exec, pipe, signal, job     ║\n");
    printf("╚══════════════════════════════════════════╝\n\n");
    
    init_shell();
    shell_loop();
    
    return 0;
}
