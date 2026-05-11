/*
 * 进程创建与 IPC（fork + pipe）示例
 * 编译: gcc -Wall -Wextra -o process_fork process_fork.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main(void) {
    int pipefd[2];
    if (pipe(pipefd) < 0) {
        perror("pipe");
        return 1;
    }
    
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }
    
    if (pid == 0) {
        /* 子进程 */
        close(pipefd[1]);  /* 关闭写端 */
        
        char buf[256];
        ssize_t n = read(pipefd[0], buf, sizeof(buf) - 1);
        if (n > 0) {
            buf[n] = '\0';
            printf("[Child] Received: %s", buf);
            
            /* 子进程回复 */
            printf("[Child] Replying...\n");
        }
        close(pipefd[0]);
        exit(0);
    } else {
        /* 父进程 */
        close(pipefd[0]);  /* 关闭读端 */
        
        const char *msg = "Hello from parent!\n";
        write(pipefd[1], msg, strlen(msg));
        close(pipefd[1]);
        
        int status;
        waitpid(pid, &status, 0);
        printf("[Parent] Child exited with %d\n", WEXITSTATUS(status));
    }
    
    return 0;
}
