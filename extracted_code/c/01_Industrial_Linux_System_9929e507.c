/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\Industrial_Linux\01_Industrial_Linux_System.md
 * Line: 2002
 * Language: c
 * Block ID: 9929e507
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file opcua_server.c
 * @brief open62541 OPC UA服务器实现
 * @details 使用open62541库实现一个完整的OPC UA服务器，
 *          包含变量、方法和事件支持
 *
 * 依赖: libopen62541-dev
 * 编译: gcc -o opcua_server opcua_server.c -lopen62541 -lpthread
 * 运行: ./opcua_server
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <open62541/server.h>
#include <open62541/server_config_default.h>
#include <open62541/plugin/log_stdout.h>
#include <open62541/plugin/accesscontrol_default.h>

/* 版本信息 */
#define SERVER_VERSION      "1.0.0"
#define SERVER_PORT         4840
#define NAMESPACE_URI       "urn:industrial:opcua:server"

/* 服务器上下文 */
typedef struct {
    UA_Server *server;
    UA_ServerConfig *config;
    volatile UA_Boolean running;

    /* 模拟数据 */
    double temperature;
    double pressure;
    double flow_rate;
    UA_Boolean motor_running;
    uint32_t cycle_count;

    /* 节点ID */
    UA_NodeId temp_node;
    UA_NodeId pressure_node;
    UA_NodeId flow_node;
    UA_NodeId motor_node;
    UA_NodeId counter_node;
} server_context_t;

static server_context_t g_context = {0};

/**
 * @brief 信号处理函数
 */
static void signal_handler(int sig)
{
    (void)sig;
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Received signal, shutting down...");
    g_context.running = false;
}

/**
 * @brief 获取当前时间戳
 */
static UA_DateTime get_current_timestamp(void)
{
    return UA_DateTime_now();
}

/**
 * @brief 温度变量回调 - 在读取前调用
 */
static void before_read_temperature(UA_Server *server,
                                     const UA_NodeId *sessionId,
                                     void *sessionContext,
                                     const UA_NodeId *nodeId,
                                     void *nodeContext,
                                     const UA_NumericRange *range)
{
    (void)sessionId;
    (void)sessionContext;
    (void)nodeId;
    (void)nodeContext;
    (void)range;

    /* 模拟温度变化 */
    g_context.temperature = 20.0 + 5.0 * sin((double)time(NULL) / 10.0) +
                           (double)(rand() % 100) / 100.0;

    UA_Variant value;
    UA_Variant_setScalar(&value, &g_context.temperature, &UA_TYPES[UA_TYPES_DOUBLE]);
    UA_Server_writeValue(server, g_context.temp_node, value);
}

/**
 * @brief 压力变量回调 - 在读取前调用
 */
static void before_read_pressure(UA_Server *server,
                                  const UA_NodeId *sessionId,
                                  void *sessionContext,
                                  const UA_NodeId *nodeId,
                                  void *nodeContext,
                                  const UA_NumericRange *range)
{
    (void)sessionId;
    (void)sessionContext;
    (void)nodeId;
    (void)nodeContext;
    (void)range;

    /* 模拟压力变化 */
    g_context.pressure = 100.0 + 10.0 * cos((double)time(NULL) / 15.0) +
                        (double)(rand() % 50) / 10.0;

    UA_Variant value;
    UA_Variant_setScalar(&value, &g_context.pressure, &UA_TYPES[UA_TYPES_DOUBLE]);
    UA_Server_writeValue(server, g_context.pressure_node, value);
}

/**
 * @brief 流量变量回调
 */
static void before_read_flow(UA_Server *server,
                              const UA_NodeId *sessionId,
                              void *sessionContext,
                              const UA_NodeId *nodeId,
                              void *nodeContext,
                              const UA_NumericRange *range)
{
    (void)sessionId;
    (void)sessionContext;
    (void)nodeId;
    (void)nodeContext;
    (void)range;

    /* 流量与电机状态相关 */
    if (g_context.motor_running) {
        g_context.flow_rate = 50.0 + (double)(rand() % 100) / 10.0;
    } else {
        g_context.flow_rate = 0.0;
    }

    UA_Variant value;
    UA_Variant_setScalar(&value, &g_context.flow_rate, &UA_TYPES[UA_TYPES_DOUBLE]);
    UA_Server_writeValue(server, g_context.flow_node, value);
}

/**
 * @brief 启动电机方法回调
 */
static UA_StatusCode method_start_motor(UA_Server *server,
                                        const UA_NodeId *sessionId,
                                        void *sessionContext,
                                        const UA_NodeId *methodId,
                                        void *methodContext,
                                        const UA_NodeId *objectId,
                                        void *objectContext,
                                        size_t inputSize,
                                        const UA_Variant *input,
                                        size_t outputSize,
                                        UA_Variant *output)
{
    (void)server;
    (void)sessionId;
    (void)sessionContext;
    (void)methodId;
    (void)methodContext;
    (void)objectId;
    (void)objectContext;
    (void)inputSize;
    (void)input;
    (void)outputSize;

    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Method 'StartMotor' called");

    if (!g_context.motor_running) {
        g_context.motor_running = true;
        g_context.cycle_count = 0;
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Motor started");

        /* 返回成功消息 */
        UA_String success = UA_STRING("Motor started successfully");
        UA_Variant_setScalarCopy(output, &success, &UA_TYPES[UA_TYPES_STRING]);
    } else {
        UA_String warning = UA_STRING("Motor is already running");
        UA_Variant_setScalarCopy(output, &warning, &UA_TYPES[UA_TYPES_STRING]);
    }

    return UA_STATUSCODE_GOOD;
}

/**
 * @brief 停止电机方法回调
 */
static UA_StatusCode method_stop_motor(UA_Server *server,
                                       const UA_NodeId *sessionId,
                                       void *sessionContext,
                                       const UA_NodeId *methodId,
                                       void *methodContext,
                                       const UA_NodeId *objectId,
                                       void *objectContext,
                                       size_t inputSize,
                                       const UA_Variant *input,
                                       size_t outputSize,
                                       UA_Variant *output)
{
    (void)server;
    (void)sessionId;
    (void)sessionContext;
    (void)methodId;
    (void)methodContext;
    (void)objectId;
    (void)objectContext;
    (void)inputSize;
    (void)input;
    (void)outputSize;

    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Method 'StopMotor' called");

    if (g_context.motor_running) {
        g_context.motor_running = false;
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Motor stopped");

        UA_String success = UA_STRING("Motor stopped successfully");
        UA_Variant_setScalarCopy(output, &success, &UA_TYPES[UA_TYPES_STRING]);
    } else {
        UA_String warning = UA_STRING("Motor is already stopped");
        UA_Variant_setScalarCopy(output, &warning, &UA_TYPES[UA_TYPES_STRING]);
    }

    return UA_STATUSCODE_GOOD;
}

/**
 * @brief 获取系统信息方法
 */
static UA_StatusCode method_get_system_info(UA_Server *server,
                                            const UA_NodeId *sessionId,
                                            void *sessionContext,
                                            const UA_NodeId *methodId,
                                            void *methodContext,
                                            const UA_NodeId *objectId,
                                            void *objectContext,
                                            size_t inputSize,
                                            const UA_Variant *input,
                                            size_t outputSize,
                                            UA_Variant *output)
{
    (void)server;
    (void)sessionId;
    (void)sessionContext;
    (void)methodId;
    (void)methodContext;
    (void)objectId;
    (void)objectContext;
    (void)inputSize;
    (void)input;
    (void)outputSize;

    /* 创建系统信息字符串 */
    char info[256];
    snprintf(info, sizeof(info),
             "Server Version: %s\n"
             "Motor State: %s\n"
             "Cycle Count: %u\n"
             "Uptime: %ld seconds",
             SERVER_VERSION,
             g_context.motor_running ? "Running" : "Stopped",
             g_context.cycle_count,
             (long)time(NULL));

    UA_String result = UA_STRING(info);
    UA_Variant_setScalarCopy(output, &result, &UA_TYPES[UA_TYPES_STRING]);

    return UA_STATUSCODE_GOOD;
}

/**
 * @brief 创建设备对象模型
 */
static void create_device_model(UA_Server *server)
{
    UA_StatusCode retval;

    /* 添加命名空间 */
    UA_UInt16 nsIndex = UA_Server_addNamespace(server, NAMESPACE_URI);
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
                "Namespace index: %d", nsIndex);

    /* ===== 创建对象类型: ProcessDevice ===== */
    UA_ObjectTypeAttributes deviceTypeAttr = UA_ObjectTypeAttributes_default;
    deviceTypeAttr.displayName = UA_LOCALIZEDTEXT("en-US", "ProcessDeviceType");
    deviceTypeAttr.description = UA_LOCALIZEDTEXT("en-US", "Industrial process device");

    UA_NodeId deviceTypeId;
    retval = UA_Server_addObjectTypeNode(server,
                                         UA_NODEID_NULL,
                                         UA_NODEID_NUMERIC(0, UA_NS0ID_BASEOBJECTTYPE),
                                         UA_NODEID_NUMERIC(0, UA_NS0ID_HASSUBTYPE),
                                         UA_QUALIFIEDNAME(nsIndex, "ProcessDeviceType"),
                                         deviceTypeAttr,
                                         NULL,
                                         &deviceTypeId);
    if (retval != UA_STATUSCODE_GOOD) {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
                     "Failed to add device type: %s", UA_StatusCode_name(retval));
        return;
    }

    /* ===== 创建设备实例 ===== */
    UA_ObjectAttributes deviceAttr = UA_ObjectAttributes_default;
    deviceAttr.displayName = UA_LOCALIZEDTEXT("en-US", "IndustrialDevice1");
    deviceAttr.description = UA_LOCALIZEDTEXT("en-US", "Main process device");

    UA_NodeId deviceId;
    retval = UA_Server_addObjectNode(server,
                                     UA_NODEID_NULL,
                                     UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
                                     UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
                                     UA_QUALIFIEDNAME(nsIndex, "IndustrialDevice1"),
                                     deviceTypeId,
                                     deviceAttr,
                                     NULL,
                                     &deviceId);
    if (retval != UA_STATUSCODE_GOOD) {
        UA_LOG_ERROR(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
                     "Failed to add device instance: %s", UA_StatusCode_name(retval));
        return;
    }

    /* ===== 添加温度变量 ===== */
    UA_VariableAttributes tempAttr = UA_VariableAttributes_default;
    tempAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Temperature");
    tempAttr.description = UA_LOCALIZEDTEXT("en-US", "Process temperature in Celsius");
    tempAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_HISTORYREAD;
    UA_Double tempValue = 25.0;
    UA_Variant_setScalar(&tempAttr.value, &tempValue, &UA_TYPES[UA_TYPES_DOUBLE]);
    tempAttr.dataType = UA_TYPES[UA_TYPES_DOUBLE].typeId;

    /* 添加工程单位属性 */
    UA_Argument euInfo;
    UA_Argument_init(&euInfo);
    euInfo.description = UA_LOCALIZEDTEXT("en-US", "Engineering units");
    euInfo.name = UA_STRING("EngineeringUnits");
    euInfo.dataType = UA_TYPES[UA_TYPES_EUINFORMATION].typeId;
    euInfo.valueRank = UA_VALUERANK_SCALAR;

    retval = UA_Server_addVariableNode(server,
                                       UA_NODEID_NULL,
                                       deviceId,
                                       UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                                       UA_QUALIFIEDNAME(nsIndex, "Temperature"),
                                       UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
                                       tempAttr,
                                       NULL,
                                       &g_context.temp_node);
    if (retval == UA_STATUSCODE_GOOD) {
        /* 设置数据源回调 */
        UA_ValueCallback callback;
        callback.onRead = before_read_temperature;
        callback.onWrite = NULL;
        UA_Server_setVariableNode_valueCallback(server, g_context.temp_node, callback);
    }

    /* ===== 添加压力变量 ===== */
    UA_VariableAttributes pressureAttr = UA_VariableAttributes_default;
    pressureAttr.displayName = UA_LOCALIZEDTEXT("en-US", "Pressure");
    pressureAttr.description = UA_LOCALIZEDTEXT("en-US", "Process pressure in kPa");
    pressureAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_HISTORYREAD;
    UA_Double pressureValue = 100.0;
    UA_Variant_setScalar(&pressureAttr.value, &pressureValue, &UA_TYPES[UA_TYPES_DOUBLE]);
    pressureAttr.dataType = UA_TYPES[UA_TYPES_DOUBLE].typeId;

    retval = UA_Server_addVariableNode(server,
                                       UA_NODEID_NULL,
                                       deviceId,
                                       UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                                       UA_QUALIFIEDNAME(nsIndex, "Pressure"),
                                       UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
                                       pressureAttr,
                                       NULL,
                                       &g_context.pressure_node);
    if (retval == UA_STATUSCODE_GOOD) {
        UA_ValueCallback callback;
        callback.onRead = before_read_pressure;
        callback.onWrite = NULL;
        UA_Server_setVariableNode_valueCallback(server, g_context.pressure_node, callback);
    }

    /* ===== 添加流量变量 ===== */
    UA_VariableAttributes flowAttr = UA_VariableAttributes_default;
    flowAttr.displayName = UA_LOCALIZEDTEXT("en-US", "FlowRate");
    flowAttr.description = UA_LOCALIZEDTEXT("en-US", "Flow rate in L/min");
    flowAttr.accessLevel = UA_ACCESSLEVELMASK_READ;
    UA_Double flowValue = 0.0;
    UA_Variant_setScalar(&flowAttr.value, &flowValue, &UA_TYPES[UA_TYPES_DOUBLE]);
    flowAttr.dataType = UA_TYPES[UA_TYPES_DOUBLE].typeId;

    retval = UA_Server_addVariableNode(server,
                                       UA_NODEID_NULL,
                                       deviceId,
                                       UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                                       UA_QUALIFIEDNAME(nsIndex, "FlowRate"),
                                       UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
                                       flowAttr,
                                       NULL,
                                       &g_context.flow_node);
    if (retval == UA_STATUSCODE_GOOD) {
        UA_ValueCallback callback;
        callback.onRead = before_read_flow;
        callback.onWrite = NULL;
        UA_Server_setVariableNode_valueCallback(server, g_context.flow_node, callback);
    }

    /* ===== 添加电机状态变量 ===== */
    UA_VariableAttributes motorAttr = UA_VariableAttributes_default;
    motorAttr.displayName = UA_LOCALIZEDTEXT("en-US", "MotorRunning");
    motorAttr.description = UA_LOCALIZEDTEXT("en-US", "Motor running status");
    motorAttr.accessLevel = UA_ACCESSLEVELMASK_READ;
    UA_Boolean motorValue = false;
    UA_Variant_setScalar(&motorAttr.value, &motorValue, &UA_TYPES[UA_TYPES_BOOLEAN]);
    motorAttr.dataType = UA_TYPES[UA_TYPES_BOOLEAN].typeId;

    retval = UA_Server_addVariableNode(server,
                                       UA_NODEID_NULL,
                                       deviceId,
                                       UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                                       UA_QUALIFIEDNAME(nsIndex, "MotorRunning"),
                                       UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
                                       motorAttr,
                                       NULL,
                                       &g_context.motor_node);

    /* ===== 添加计数器变量 ===== */
    UA_VariableAttributes counterAttr = UA_VariableAttributes_default;
    counterAttr.displayName = UA_LOCALIZEDTEXT("en-US", "CycleCount");
    counterAttr.description = UA_LOCALIZEDTEXT("en-US", "Number of production cycles");
    counterAttr.accessLevel = UA_ACCESSLEVELMASK_READ;
    UA_UInt32 counterValue = 0;
    UA_Variant_setScalar(&counterAttr.value, &counterValue, &UA_TYPES[UA_TYPES_UINT32]);
    counterAttr.dataType = UA_TYPES[UA_TYPES_UINT32].typeId;

    retval = UA_Server_addVariableNode(server,
                                       UA_NODEID_NULL,
                                       deviceId,
                                       UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                                       UA_QUALIFIEDNAME(nsIndex, "CycleCount"),
                                       UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
                                       counterAttr,
                                       NULL,
                                       &g_context.counter_node);

    /* ===== 添加方法: 启动电机 ===== */
    UA_Argument outputArgument;
    UA_Argument_init(&outputArgument);
    outputArgument.description = UA_LOCALIZEDTEXT("en-US", "Operation result");
    outputArgument.name = UA_STRING("Result");
    outputArgument.dataType = UA_TYPES[UA_TYPES_STRING].typeId;
    outputArgument.valueRank = UA_VALUERANK_SCALAR;

    UA_MethodAttributes startAttr = UA_MethodAttributes_default;
    startAttr.displayName = UA_LOCALIZEDTEXT("en-US", "StartMotor");
    startAttr.description = UA_LOCALIZEDTEXT("en-US", "Start the main motor");
    startAttr.executable = true;
    startAttr.userExecutable = true;

    retval = UA_Server_addMethodNode(server,
                                     UA_NODEID_NULL,
                                     deviceId,
                                     UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                                     UA_QUALIFIEDNAME(nsIndex, "StartMotor"),
                                     startAttr,
                                     &method_start_motor,
                                     0, NULL,
                                     1, &outputArgument,
                                     NULL,
                                     NULL);

    /* ===== 添加方法: 停止电机 ===== */
    UA_MethodAttributes stopAttr = UA_MethodAttributes_default;
    stopAttr.displayName = UA_LOCALIZEDTEXT("en-US", "StopMotor");
    stopAttr.description = UA_LOCALIZEDTEXT("en-US", "Stop the main motor");
    stopAttr.executable = true;
    stopAttr.userExecutable = true;

    retval = UA_Server_addMethodNode(server,
                                     UA_NODEID_NULL,
                                     deviceId,
                                     UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                                     UA_QUALIFIEDNAME(nsIndex, "StopMotor"),
                                     stopAttr,
                                     &method_stop_motor,
                                     0, NULL,
                                     1, &outputArgument,
                                     NULL,
                                     NULL);

    /* ===== 添加方法: 获取系统信息 ===== */
    UA_MethodAttributes infoAttr = UA_MethodAttributes_default;
    infoAttr.displayName = UA_LOCALIZEDTEXT("en-US", "GetSystemInfo");
    infoAttr.description = UA_LOCALIZEDTEXT("en-US", "Get system information");
    infoAttr.executable = true;
    infoAttr.userExecutable = true;

    retval = UA_Server_addMethodNode(server,
                                     UA_NODEID_NULL,
                                     deviceId,
                                     UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                                     UA_QUALIFIEDNAME(nsIndex, "GetSystemInfo"),
                                     infoAttr,
                                     &method_get_system_info,
                                     0, NULL,
                                     1, &outputArgument,
                                     NULL,
                                     NULL);

    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
                "Device model created successfully");
}

/**
 * @brief 周期性回调 - 更新数据
 */
static void cyclic_callback(UA_Server *server, void *data)
{
    (void)server;
    (void)data;

    /* 更新周期计数 */
    if (g_context.motor_running) {
        g_context.cycle_count++;
    }

    /* 每100周期打印一次状态 */
    if (g_context.cycle_count % 100 == 0) {
        UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
                    "Status: Motor=%s, Temp=%.2f°C, Pressure=%.2fkPa, Cycles=%u",
                    g_context.motor_running ? "ON" : "OFF",
                    g_context.temperature,
                    g_context.pressure,
                    g_context.cycle_count);
    }
}

/**
 * @brief 主函数
 */
int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    UA_StatusCode retval;

    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║    Industrial OPC UA Server v%s                        ║\n", SERVER_VERSION);
    printf("╚══════════════════════════════════════════════════════════╝\n\n");

    /* 注册信号处理 */
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    /* 初始化服务器配置 */
    g_context.config = UA_ServerConfig_new_default();
    if (!g_context.config) {
        fprintf(stderr, "[ERROR] Failed to create server config\n");
        return 1;
    }

    /* 设置服务器端口 */
    g_context.config->networkLayersSize = 1;
    g_context.config->networkLayers =
        UA_ServerNetworkLayerTCP(UA_ConnectionConfig_default, SERVER_PORT);

    /* 创建服务器 */
    g_context.server = UA_Server_new(g_context.config);
    if (!g_context.server) {
        UA_ServerConfig_delete(g_context.config);
        fprintf(stderr, "[ERROR] Failed to create server\n");
        return 1;
    }

    /* 创建设备模型 */
    create_device_model(g_context.server);

    /* 添加周期性回调 (每1000ms) */
    UA_Server_addRepeatedCallback(g_context.server, cyclic_callback,
                                   NULL, 1000.0, NULL);

    /* 启动服务器 */
    g_context.running = true;

    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
                "Server starting on port %d...", SERVER_PORT);
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
                "Use any OPC UA client to connect (e.g., UA Expert)");
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER,
                "Press Ctrl+C to stop\n");

    /* 运行服务器 */
    retval = UA_Server_run(g_context.server, &g_context.running);

    /* 清理 */
    UA_Server_delete(g_context.server);
    UA_ServerConfig_delete(g_context.config);

    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Server stopped");

    return (int)retval;
}
