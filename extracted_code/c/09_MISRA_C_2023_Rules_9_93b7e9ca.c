/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\09_MISRA_C_2023_Rules_9.md
 * Line: 245
 * Language: c
 * Block ID: 93b7e9ca
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 严重违反 - 信号处理中做太多操作 */
void unsafe_handler(int sig)
{
    /* 这些在信号处理程序中都是未定义行为！ */
    printf("Signal received\n");  /* 非异步信号安全 */
    free(global_ptr);             /* 可能破坏堆 */
    fclose(file);                 /* 非异步信号安全 */
    malloc(100);                  /* 非异步信号安全 */
}

/* ✅ 合规 - 异步信号安全函数列表 */
/* _Exit, _exit, abort, accept, access, alarm, bind, cfgetispeed,
 * cfgetospeed, cfsetispeed, cfsetospeed, chdir, chmod, chown,
 * clock_gettime, close, connect, creat, dup, dup2, execl, execle,
 * execv, execve, faccessat, fchdir, fchmod, fchmodat, fchown,
 * fchownat, fcntl, fdatasync, fexecve, ffs, fork, fstat, fstatat,
 * fsync, ftruncate, futimens, getegid, geteuid, getgid, getgroups,
 * getpeername, getpgrp, getpid, getppid, getsockname, getsockopt,
 * getuid, htonl, htons, inet_addr, inet_ntoa, ioctl, kill, link,
 * linkat, listen, longjmp, lseek, lstat, memccpy, memchr, memcmp,
 * memcpy, memmove, memset, mkdir, mkdirat, mkfifo, mkfifoat, mknod,
 * mknodat, ntohl, ntohs, open, openat, pause, pipe, poll, posix_trace_event,
 * pselect, pthread_kill, pthread_self, pthread_sigmask, raise, read,
 * readlink, readlinkat, recv, recvfrom, recvmsg, rename, renameat,
 * rmdir, select, sem_post, send, sendmsg, sendto, setgid, setpgid,
 * setsid, setsockopt, setuid, shutdown, sigaction, sigaddset, sigdelset,
 * sigemptyset, sigfillset, sigismember, signal, sigpause, sigpending,
 * sigprocmask, sigqueue, sigset, sigsuspend, sleep, sockatmark, socket,
 * socketpair, stat, stpcpy, stpncpy, strcat, strchr, strcmp, strcpy,
 * strcspn, strlen, strncat, strncmp, strncpy, strnlen, strpbrk, strrchr,
 * strspn, strstr, strtok_r, strxfrm, symlink, symlinkat, tcdrain,
 * tcflow, tcflush, tcgetattr, tcgetpgrp, tcsendbreak, tcsetattr,
 * tcsetpgrp, time, timer_getoverrun, timer_gettime, timer_settime,
 * times, umask, uname, unlink, unlinkat, utime, utimensat, utimes,
 * wait, waitpid, write
 */
