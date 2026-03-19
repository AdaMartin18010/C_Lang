/*
 * Auto-generated from: 12_Practice_Exercises\09_Security_Coding.md
 * Line: 300
 * Language: c
 * Block ID: cab2aaf4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

SandboxConfig config = {
    .allowed_syscalls = (int[]){SYS_read, SYS_write, SYS_exit, SYS_exit_group},
    .num_allowed = 4,
    .cpu_limit = {5, 0},  // 5秒CPU时间
    .memory_limit = {64 * 1024 * 1024, 64 * 1024 * 1024},  // 64MB
};

int result = sandbox_execute(sandbox_compute, args, &config);
