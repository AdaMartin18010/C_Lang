/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\04_Secure_Coding_Guide.md
 * Line: 336
 * Language: c
 * Block ID: 08382a84
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 漏洞代码
void vulnerable_exec(const char *filename) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd), "ls -la %s", filename);
    system(cmd);  // ⚠️ filename="; rm -rf /"
}
