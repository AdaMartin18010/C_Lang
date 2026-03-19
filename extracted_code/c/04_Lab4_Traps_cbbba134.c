/*
 * Auto-generated from: 04_Industrial_Scenarios\16_MIT_6S081_Labs\04_Lab4_Traps.md
 * Line: 508
 * Language: c
 * Block ID: cbbba134
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// user/alarmtest.c
#include "kernel/types.h"
#include "user/user.h"

int ticks = 0;

void alarm_handler() {
    ticks++;
    printf("alarm! ticks=%d\n", ticks);
    sigreturn();
}

int main() {
    // 每10个ticks触发一次alarm
    sigalarm(10, alarm_handler);

    // 执行一些计算
    for (int i = 0; i < 100000000; i++) {
        // 消耗时间
    }

    printf("total alarms: %d\n", ticks);
    exit(0);
}
