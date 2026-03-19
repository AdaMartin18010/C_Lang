/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\05_Shell_Lab.md
 * Line: 279
 * Language: c
 * Block ID: 268a3578
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
