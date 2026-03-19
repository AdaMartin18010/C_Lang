/*
 * Auto-generated from: 06_Thinking_Representation\06_Learning_Paths\02_Intermediate_Learning_Path.md
 * Line: 412
 * Language: c
 * Block ID: cd4f72bf
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// myshell.c - 功能完整的迷你Shell

// 必须支持的功能：
// 1. 命令解析（引号、转义、变量展开）
// 2. 内建命令：cd, pwd, exit, export, echo
// 3. 管道：cmd1 | cmd2 | cmd3
// 4. 重定向：>, <, >>
// 5. 后台执行：cmd &
// 6. 信号处理：Ctrl+C, Ctrl+Z
// 7. 作业控制：fg, bg, jobs

// 数据结构
typedef struct Job {
    int job_id;
    pid_t pgid;
    char *command;
    int status;  // RUNNING, STOPPED, DONE
    struct Job *next;
} Job;

// 核心函数
int parse_command(char *input, Command *cmd);
int execute_command(Command *cmd);
int execute_pipeline(Command **cmds, int n);
int execute_redirection(Command *cmd, Redirection *redirs);
void handle_signal(int sig);
void job_control(int action, int job_id);
