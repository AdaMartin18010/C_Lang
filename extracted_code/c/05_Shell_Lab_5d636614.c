/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\05_Shell_Lab.md
 * Line: 439
 * Language: c
 * Block ID: 5d636614
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
