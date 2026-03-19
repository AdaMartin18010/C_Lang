/*
 * Auto-generated from: 16_Embedded_AI_Agents\03_AI_Agent_Runtime\01_ZeroClaw_Overview.md
 * Line: 308
 * Language: c
 * Block ID: a57479c9
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// main.c: 基于ZeroClaw的智能温度监控系统
#include <zeroclaw.h>
#include <sensor.h>
#include <stdio.h>
#include <unistd.h>

// 传感器读数回调
void on_sensor_read(float temp, float humidity) {
    char context[256];
    snprintf(context, sizeof(context),
        "Current temperature: %.1f°C, humidity: %.1f%%. "
        "Alert if temperature > 30°C.",
        temp, humidity);

    char response[1024];
    zeroclaw_execute(
        g_runtime,
        "temp_monitor_agent",
        context,
        response,
        sizeof(response)
    );

    // 解析Agent决策
    if (strstr(response, "ALERT")) {
        trigger_alarm();
    }
}

int main() {
    // 初始化ZeroClaw运行时
    const char* agent_config = "{"
        "\"provider\": \"local\","
        "\"model\": \"/opt/models/tinyllama.gguf\","
        "\"system_prompt\": \"You are a temperature monitor. "
            "Respond with 'ALERT' if temperature is critical.\""
    "}";

    g_runtime = zeroclaw_init(agent_config);

    // 主循环
    while (1) {
        float temp = sensor_read_temperature();
        float humidity = sensor_read_humidity();
        on_sensor_read(temp, humidity);
        sleep(60);  // 每分钟检查
    }

    return 0;
}
