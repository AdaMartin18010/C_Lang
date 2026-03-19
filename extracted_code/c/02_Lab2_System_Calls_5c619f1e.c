/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\02_Lab2_System_Calls.md
 * Line: 336
 * Language: c
 * Block ID: 5c619f1e
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// user/sysinfotest.c
#include "kernel/types.h"
#include "user/user.h"
#include "kernel/sysinfo.h"

int main() {
    struct sysinfo info;

    if (sysinfo(&info) < 0) {
        printf("sysinfo failed\n");
        exit(1);
    }

    printf("free memory: %l\n", info.freemem);
    printf("number of processes: %l\n", info.nproc);

    exit(0);
}
