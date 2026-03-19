/*
 * Auto-generated from: 04_Industrial_Scenarios\11_Space_Computing\01_Radiation_Hardening.md
 * Line: 385
 * Language: c
 * Block ID: f6f9f8af
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ❌ 看门狗本身也需要保护
Watchdog wd;  // 可能被SEU破坏

// ✅ 看门狗使用专用硬件，软件多重检测
static Watchdog wd_primary, wd_backup, wd_tertiary;

void robust_watchdog_feed(void) {
    watchdog_feed(&wd_primary);
    watchdog_feed(&wd_backup);
    watchdog_feed(&wd_tertiary);
}

bool check_watchdog_health(void) {
    return wd_primary.missed_beats < 3 &&
           wd_backup.missed_beats < 3 &&
           wd_tertiary.missed_beats < 3;
}
