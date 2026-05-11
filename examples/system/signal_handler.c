/*
 * 信号处理示例（POSIX）
 * 编译: gcc -Wall -Wextra -o signal_handler signal_handler.c
 * 运行: ./signal_handler 然后 Ctrl+C
 */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

static volatile sig_atomic_t should_exit = 0;
static volatile sig_atomic_t signal_count = 0;

void handler(int sig) {
    signal_count++;
    if (sig == SIGINT) {
        printf("\nReceived SIGINT (%d), count=%d\n", sig, signal_count);
        if (signal_count >= 3) {
            should_exit = 1;
        }
    }
}

int main(void) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    
    sigaction(SIGINT, &sa, NULL);
    
    printf("PID: %d\n", getpid());
    printf("Press Ctrl+C 3 times to exit, or send SIGTERM\n");
    
    while (!should_exit) {
        printf("Working...\n");
        sleep(1);
    }
    
    printf("Graceful exit after %d signals\n", signal_count);
    return 0;
}
