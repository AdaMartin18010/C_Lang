/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\00_MIT_Labs_Overview.md
 * Line: 60
 * Language: c
 * Block ID: 5ece04e0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// user/sleep.c
#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(2, "Usage: sleep ticks\n");
        exit(1);
    }
    sleep(atoi(argv[1]));
    exit(0);
}
