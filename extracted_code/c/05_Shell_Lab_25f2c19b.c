/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\05_Shell_Lab.md
 * Line: 169
 * Language: c
 * Block ID: 25f2c19b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

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
