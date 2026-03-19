/*
 * Auto-generated from: 07_Modern_Toolchain\05_Case_Studies\04_Linux_Kernel_Toolchain.md
 * Line: 755
 * Language: c
 * Block ID: 62827c2b
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* eBPF 程序示例 - 追踪系统调用 */

#include <linux/bpf.h>
#include <linux/ptrace.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>

SEC("tracepoint/syscalls/sys_enter_openat")
int trace_openat(struct trace_event_raw_sys_enter *ctx)
{
    char comm[16];
    char filename[256];

    bpf_get_current_comm(&comm, sizeof(comm));
    bpf_probe_read_user_str(&filename, sizeof(filename), (char *)ctx->args[1]);

    bpf_printk("Process %s opened: %s\n", comm, filename);

    return 0;
}

char _license[] SEC("license") = "GPL";
