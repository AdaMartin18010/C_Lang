/*
 * Auto-generated from: 16_Embedded_AI_Agents\05_Case_Studies\04_ZeroClaw_Edge_Deployment.md
 * Line: 210
 * Language: c
 * Block ID: 3025cae0
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// sensor_collector.cpp
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <modbus/modbus.h>
#include <mqtt/async_client.h>
#include <sqlite3.h>

struct SensorData {
    uint64_t timestamp;
    float temperature;
    float vibration_x;
    float vibration_y;
    float vibration_z;
    float current;
    float voltage;
};

class IndustrialDataCollector {
private:
    modbus_t* modbus_ctx;
    mqtt::async_client mqtt_client;
    sqlite3* db;

    // Modbus寄存器地址
    const int REG_TEMP = 0;
    const int REG_VIB_X = 1;
    const int REG_VIB_Y = 2;
    const int REG_VIB_Z = 3;
    const int REG_CURRENT = 4;
    const int REG_VOLTAGE = 5;

public:
    IndustrialDataCollector(const std::string& modbus_device,
                           const std::string& mqtt_server)
        : mqtt_client(mqtt_server, "pi_collector") {

        // 初始化Modbus RTU
        modbus_ctx = modbus_new_rtu(modbus_device.c_str(), 9600, 'N', 8, 1);
        if (!modbus_ctx) {
            throw std::runtime_error("Failed to create Modbus context");
        }

        modbus_set_slave(modbus_ctx, 1);
        if (modbus_connect(modbus_ctx) == -1) {
            throw std::runtime_error("Modbus connection failed");
        }

        // 初始化MQTT
        mqtt::connect_options connOpts;
        connOpts.set_keep_alive_interval(20);
        connOpts.set_clean_session(true);
        mqtt_client.connect(connOpts)->wait();

        // 初始化SQLite
        int rc = sqlite3_open("/home/pi/data/sensor.db", &db);
        if (rc) {
            throw std::runtime_error("Can't open database");
        }

        // 创建表
        const char* create_table = R"(
            CREATE TABLE IF NOT EXISTS sensor_data (
                timestamp INTEGER PRIMARY KEY,
                temperature REAL,
                vibration_x REAL,
                vibration_y REAL,
                vibration_z REAL,
                current REAL,
                voltage REAL
            );
            CREATE INDEX IF NOT EXISTS idx_time ON sensor_data(timestamp);
        )";

        char* errMsg = nullptr;
        sqlite3_exec(db, create_table, nullptr, nullptr, &errMsg);
        if (errMsg) {
            sqlite3_free(errMsg);
        }
    }

    SensorData read_sensors() {
        SensorData data;
        data.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()
        ).count();

        uint16_t regs[6];
        int rc = modbus_read_registers(modbus_ctx, 0, 6, regs);
        if (rc == -1) {
            std::cerr << "Modbus read error" << std::endl;
            return data;  // 返回带时间戳的空数据
        }

        // 转换为实际值 (根据传感器规格)
        data.temperature = regs[REG_TEMP] / 10.0f;     // 0.1°C精度
        data.vibration_x = regs[REG_VIB_X] / 1000.0f;  // mg
        data.vibration_y = regs[REG_VIB_Y] / 1000.0f;
        data.vibration_z = regs[REG_VIB_Z] / 1000.0f;
        data.current = regs[REG_CURRENT] / 100.0f;     // 0.01A
        data.voltage = regs[REG_VOLTAGE] / 10.0f;      // 0.1V

        return data;
    }

    void store_locally(const SensorData& data) {
        const char* insert = "INSERT INTO sensor_data VALUES (?, ?, ?, ?, ?, ?, ?)";
        sqlite3_stmt* stmt;

        sqlite3_prepare_v2(db, insert, -1, &stmt, nullptr);
        sqlite3_bind_int64(stmt, 1, data.timestamp);
        sqlite3_bind_double(stmt, 2, data.temperature);
        sqlite3_bind_double(stmt, 3, data.vibration_x);
        sqlite3_bind_double(stmt, 4, data.vibration_y);
        sqlite3_bind_double(stmt, 5, data.vibration_z);
        sqlite3_bind_double(stmt, 6, data.current);
        sqlite3_bind_double(stmt, 7, data.voltage);

        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }

    void publish_mqtt(const SensorData& data) {
        std::string payload = std::string("{") +
            "\"timestamp\":" + std::to_string(data.timestamp) + "," +
            "\"temperature\":" + std::to_string(data.temperature) + "," +
            "\"vibration\":{" +
                "\"x\":" + std::to_string(data.vibration_x) + "," +
                "\"y\":" + std::to_string(data.vibration_y) + "," +
                "\"z\":" + std::to_string(data.vibration_z) +
            "}," +
            "\"current\":" + std::to_string(data.current) + "," +
            "\"voltage\":" + std::to_string(data.voltage) +
        "}";

        mqtt::message_ptr pubmsg = mqtt::make_message("factory/sensor001", payload);
        pubmsg->set_qos(1);
        mqtt_client.publish(pubmsg)->wait_for(std::chrono::seconds(1));
    }

    void run() {
        while (true) {
            auto data = read_sensors();
            store_locally(data);
            publish_mqtt(data);

            // 1秒采样间隔
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    ~IndustrialDataCollector() {
        if (modbus_ctx) {
            modbus_close(modbus_ctx);
            modbus_free(modbus_ctx);
        }
        if (db) {
            sqlite3_close(db);
        }
    }
};

int main() {
    try {
        IndustrialDataCollector collector("/dev/ttyUSB0", "tcp://localhost:1883");
        collector.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
