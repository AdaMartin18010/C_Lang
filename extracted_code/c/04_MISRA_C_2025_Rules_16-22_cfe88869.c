/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2025\04_MISRA_C_2025_Rules_16-22.md
 * Line: 82
 * Language: c
 * Block ID: cfe88869
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 不符合规范 - malloc在信号处理程序中不安全 */
void signal_handler(int sig) {
    char* msg = malloc(100);  /* 违反规则 */
    strcpy(msg, "Signal received");
    /* ... */
}

/* 符合规范 - 只使用异步信号安全函数 */
void signal_handler(int sig) {
    const char msg[] = "Signal received\n";
    write(STDERR_FILENO, msg, sizeof(msg) - 1);
    _exit(1);
}
