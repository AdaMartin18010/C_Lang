/*
 * Auto-generated from: 07_Modern_Toolchain\09_System_Architecture\README.md
 * Line: 62
 * Language: c
 * Block ID: aac0803c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* layer_architecture.h - 分层架构核心定义 */
#ifndef LAYER_ARCHITECTURE_H
#define LAYER_ARCHITECTURE_H

#include <stdint.h>
#include <stdbool.h>

/* 错误码定义 */
typedef enum {
    LAYER_OK = 0,
    LAYER_ERROR_INVALID_PARAM = -1,
    LAYER_ERROR_NOT_INITIALIZED = -2,
    LAYER_ERROR_IO = -3,
    LAYER_ERROR_MEMORY = -4
} layer_error_t;

/* 基础设施层 - 硬件抽象 */
typedef struct {
    int (*init)(void *config);
    int (*read)(uint32_t addr, void *buf, size_t len);
    int (*write)(uint32_t addr, const void *buf, size_t len);
    int (*deinit)(void);
} hal_interface_t;

/* 数据层 - 数据访问对象 */
typedef struct {
    hal_interface_t *hal;
    uint8_t *buffer;
    size_t buffer_size;
    bool initialized;
} data_layer_t;

/* 服务层 - 业务逻辑 */
typedef struct {
    data_layer_t *data;
    int (*process)(void *input, void *output);
} service_layer_t;

/* 应用层 - 用户接口 */
typedef struct {
    service_layer_t *service;
    int (*run)(int argc, char **argv);
} app_layer_t;

/* API声明 */
layer_error_t data_layer_init(data_layer_t *layer, hal_interface_t *hal, size_t buf_size);
layer_error_t data_layer_read(data_layer_t *layer, uint32_t offset, void *buf, size_t len);
layer_error_t service_layer_init(service_layer_t *service, data_layer_t *data);
layer_error_t app_layer_init(app_layer_t *app, service_layer_t *service);

#endif /* LAYER_ARCHITECTURE_H */
