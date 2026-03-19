/*
 * Auto-generated from: 02_Formal_Semantics_and_Physics\10_ISA_Machine_Code\09_Interrupt_Hardware\02_System_Call_Hardware.md
 * Line: 1505
 * Language: c
 * Block ID: 6ab3c0b7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * Linux x86_64系统调用表
 * arch/x86/entry/syscalls/syscall_64.tbl
 */

/*
 * 格式: 系统调用号  abi   名称            入口点
 *       64          common  read           sys_read
 */

// 部分系统调用列表
0   common  read                    sys_read
1   common  write                   sys_write
2   common  open                    sys_open
3   common  close                   sys_close
4   common  stat                    sys_newstat
5   common  fstat                   sys_newfstat
9   common  mmap                    sys_mmap
10  common  mprotect                sys_mprotect
11  common  munmap                  sys_munmap
12  common  brk                     sys_brk

// 进程管理
39  common  getpid                  sys_getpid
56  common  clone                   sys_clone
57  common  fork                    sys_fork
58  common  vfork                   sys_vfork
59  common  execve                  sys_execve
60  common  exit                    sys_exit
61  common  wait4                   sys_wait4

// 文件系统
80  common  chdir                   sys_chdir
82  common  rename                  sys_rename
83  common  mkdir                   sys_mkdir
84  common  rmdir                   sys_rmdir
85  common  creat                   sys_creat

// 网络
41  common  socket                  sys_socket
42  common  connect                 sys_connect
43  common  accept                  sys_accept
44  common  sendto                  sys_sendto
45  common  recvfrom                sys_recvfrom

// 时间
96  common  gettimeofday            sys_gettimeofday

// 特殊
218 common  set_tid_address         sys_set_tid_address
219 common  restart_syscall         sys_restart_syscall

// 总计: 约500+个系统调用
