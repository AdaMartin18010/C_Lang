/*
 * Auto-generated from: 07_Modern_Toolchain\08_Design_Patterns\README.md
 * Line: 749
 * Language: c
 * Block ID: 4f0a5092
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 使用示例: 温度监控系统
typedef struct {
    Observer interface;
    int threshold;
} TemperatureAlert;

static void alert_update(Observer* self, const char* event, void* data) {
    TemperatureAlert* alert = (TemperatureAlert*)self;
    if (strcmp(event, "temperature") == 0) {
        float temp = *(float*)data;
        if (temp > alert->threshold) {
            printf("ALERT: Temperature %.1f exceeds threshold %d!\n",
                   temp, alert->threshold);
        }
    }
}

void observer_example(void) {
    Subject* bus = event_bus_create();

    // 创建观察者
    TemperatureAlert high_temp_alert = {
        .interface.update = alert_update,
        .threshold = 80
    };

    // 订阅
    bus->attach(bus, (Observer*)&high_temp_alert);

    // 模拟温度变化
    float temp = 85.0f;
    bus->notify(bus, "temperature", &temp);

    // 输出: ALERT: Temperature 85.0 exceeds threshold 80!
}
