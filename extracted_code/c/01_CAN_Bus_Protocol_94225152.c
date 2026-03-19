/*
 * Auto-generated from: 04_Industrial_Scenarios\02_Automotive_ECU\01_CAN_Bus_Protocol.md
 * Line: 155
 * Language: c
 * Block ID: 94225152
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 设置CAN接口参数 */
#define SIOCGIFINDEX    /* 获取接口索引 */
#define SIOCSCANBITRATE /* 设置比特率 */
#define SIOCSCANMODE    /* 设置模式 */

/* CAN模式 */
enum can_mode {
    CAN_MODE_STOP = 0,      /* 停止模式 */
    CAN_MODE_START,         /* 启动模式 */
    CAN_MODE_SLEEP,         /* 休眠模式 */
    CAN_MODE_LOOPBACK,      /* 环回模式 */
    CAN_MODE_LISTENONLY,    /* 仅监听模式 */
    CAN_MODE_ERROR_ACTIVE,  /* 错误主动模式 */
    CAN_MODE_ERROR_PASSIVE  /* 错误被动模式 */
};
