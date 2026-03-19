/*
 * Auto-generated from: 16_Embedded_AI_Agents\03_AI_Agent_Runtime\01_ZeroClaw_Overview.md
 * Line: 131
 * Language: c
 * Block ID: e3e28449
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// C宿主应用集成ZeroClaw
#include <zeroclaw.h>
#include <stdio.h>

int main() {
    // 初始化运行时
    const char* config = "{"
        "\"provider\": \"ollama\","
        "\"model\": \"llama3.2:1b\","
        "\"memory\": \"sqlite:///tmp/agent.db\""
    "}";

    void* runtime = zeroclaw_init(config);
    if (!runtime) {
        fprintf(stderr, "Failed to initialize ZeroClaw\n");
        return 1;
    }

    // 执行Agent
    char output[4096];
    int result = zeroclaw_execute(
        runtime,
        "embedded_agent",
        "What is the current temperature?",
        output,
        sizeof(output)
    );

    if (result == 0) {
        printf("Agent response: %s\n", output);
    }

    // 清理
    zeroclaw_destroy(runtime);
    return 0;
}
