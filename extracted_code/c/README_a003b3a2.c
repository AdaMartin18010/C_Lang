/*
 * Auto-generated from: 07_Modern_Toolchain\09_System_Architecture\README.md
 * Line: 116
 * Language: c
 * Block ID: a003b3a2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* layer_architecture.c - 分层架构实现 */
#include "layer_architecture.h"
#include <stdlib.h>
#include <string.h>

/* 数据层实现 */
layer_error_t data_layer_init(data_layer_t *layer, hal_interface_t *hal, size_t buf_size) {
    if (!layer || !hal || buf_size == 0) {
        return LAYER_ERROR_INVALID_PARAM;
    }

    layer->buffer = malloc(buf_size);
    if (!layer->buffer) {
        return LAYER_ERROR_MEMORY;
    }

    layer->buffer_size = buf_size;
    layer->hal = hal;
    layer->initialized = true;

    /* 初始化硬件抽象层 */
    if (hal->init(NULL) != 0) {
        free(layer->buffer);
        return LAYER_ERROR_IO;
    }

    return LAYER_OK;
}

layer_error_t data_layer_read(data_layer_t *layer, uint32_t offset, void *buf, size_t len) {
    if (!layer || !layer->initialized || !buf) {
        return LAYER_ERROR_INVALID_PARAM;
    }

    if (offset + len > layer->buffer_size) {
        return LAYER_ERROR_INVALID_PARAM;
    }

    /* 通过HAL层读取数据 */
    if (layer->hal->read(offset, buf, len) != 0) {
        return LAYER_ERROR_IO;
    }

    return LAYER_OK;
}

/* 服务层实现 */
layer_error_t service_layer_init(service_layer_t *service, data_layer_t *data) {
    if (!service || !data) {
        return LAYER_ERROR_INVALID_PARAM;
    }

    service->data = data;
    service->process = NULL; /* 由具体业务实现 */

    return LAYER_OK;
}

/* 应用层实现 */
layer_error_t app_layer_init(app_layer_t *app, service_layer_t *service) {
    if (!app || !service) {
        return LAYER_ERROR_INVALID_PARAM;
    }

    app->service = service;
    return LAYER_OK;
}
