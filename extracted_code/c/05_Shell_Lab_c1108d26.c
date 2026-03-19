/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\05_Shell_Lab.md
 * Line: 498
 * Language: c
 * Block ID: c1108d26
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
