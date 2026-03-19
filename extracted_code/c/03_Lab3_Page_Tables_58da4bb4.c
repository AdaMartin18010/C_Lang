/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\03_Lab3_Page_Tables.md
 * Line: 246
 * Language: c
 * Block ID: 58da4bb4
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// user/user.h
struct usyscall {
    int pid;
};

// USYSCALL页地址 (与内核一致)
#define USYSCALL 0x40000000

static inline int getpid_fast(void) {
    struct usyscall *u = (struct usyscall *)USYSCALL;
    return u->pid;
}
