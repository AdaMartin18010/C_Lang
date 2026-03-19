/*
 * Auto-generated from: 07_Modern_Toolchain\11_Workflow_StateMachines\02_Rule_Engine.md
 * Line: 347
 * Language: c
 * Block ID: 5e83248b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* Cron表达式解析和调度 */

typedef struct {
    int minute[60];      /* 0-59, -1=任意 */
    int hour[24];
    int day_of_month[31];
    int month[12];
    int day_of_week[7];
} cron_expression_t;

/* 解析Cron: "*/5 * * * *" 每5分钟 */
int cron_parse(const char* expr, cron_expression_t* cron);

/* 计算下次执行时间 */
time_t cron_next(cron_expression_t* cron, time_t from);

/* 任务定义 */
typedef struct {
    char name[64];
    cron_expression_t schedule;
    void (*job)(void* arg);
    void* arg;
    bool enabled;
} scheduled_job_t;

/* 调度器 */
typedef struct {
    scheduled_job_t* jobs;
    int job_count;
    mtx_t lock;
    cnd_t cond;
    bool running;
} scheduler_t;

void scheduler_add_job(scheduler_t* s, const char* name,
                       const char* cron_expr,
                       void (*job)(void*), void* arg) {
    mtx_lock(&s->lock);

    scheduled_job_t* j = &s->jobs[s->job_count++];
    strncpy(j->name, name, sizeof(j->name));
    cron_parse(cron_expr, &j->schedule);
    j->job = job;
    j->arg = arg;
    j->enabled = true;

    mtx_unlock(&s->lock);
    cnd_signal(&s->cond);
}

/* 调度循环 */
int scheduler_thread(void* arg) {
    scheduler_t* s = arg;

    while (s->running) {
        mtx_lock(&s->lock);

        time_t now = time(NULL);
        time_t next_run = now + 3600;  /* 默认1小时后 */
        scheduled_job_t* next_job = NULL;

        /* 找到最近要执行的任务 */
        for (int i = 0; i < s->job_count; i++) {
            if (!s->jobs[i].enabled) continue;

            time_t job_next = cron_next(&s->jobs[i].schedule, now);
            if (job_next < next_run) {
                next_run = job_next;
                next_job = &s->jobs[i];
            }
        }

        /* 等待或执行 */
        int wait_sec = next_run - time(NULL);
        if (wait_sec > 0) {
            struct timespec deadline;
            clock_gettime(CLOCK_REALTIME, &deadline);
            deadline.tv_sec += wait_sec;
            cnd_timedwait(&s->cond, &s->lock, &deadline);
        }

        mtx_unlock(&s->lock);

        /* 执行任务 */
        if (next_job && time(NULL) >= next_run) {
            printf("Running job: %s\n", next_job->name);
            next_job->job(next_job->arg);
        }
    }

    return 0;
}
