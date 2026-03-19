/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\05_Shell_Lab.md
 * Line: 375
 * Language: c
 * Block ID: 1e980efc
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
