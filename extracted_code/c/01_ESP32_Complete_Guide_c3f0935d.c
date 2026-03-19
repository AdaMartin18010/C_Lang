/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\ESP32\01_ESP32_Complete_Guide.md
 * Line: 1215
 * Language: c
 * Block ID: c3f0935d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * http_client_example.c
 * ESP32 HTTP 客户端示例
 */

#include "esp_http_client.h"
#include "cJSON.h"

#define HTTP_TAG            "HTTP_CLIENT"
#define MAX_HTTP_OUTPUT_BUFFER 512

/**
 * @brief HTTP 事件处理回调
 */
static esp_err_t http_event_handler(esp_http_client_event_t *evt)
{
    static char *output_buffer;  /* 响应缓冲区 */
    static int output_len;       /* 已接收数据长度 */

    switch (evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGI(HTTP_TAG, "HTTP_EVENT_ERROR");
            break;

        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(HTTP_TAG, "HTTP_EVENT_ON_CONNECTED");
            break;

        case HTTP_EVENT_HEADER_SENT:
            ESP_LOGI(HTTP_TAG, "HTTP_EVENT_HEADER_SENT");
            break;

        case HTTP_EVENT_ON_HEADER:
            ESP_LOGI(HTTP_TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s",
                     evt->header_key, evt->header_value);
            break;

        case HTTP_EVENT_ON_DATA:
            ESP_LOGI(HTTP_TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);

            /* 累积响应数据 */
            if (!esp_http_client_is_chunked_response(evt->client)) {
                if (evt->user_data) {
                    memcpy(evt->user_data + output_len, evt->data, evt->data_len);
                }
                output_len += evt->data_len;
            }
            break;

        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(HTTP_TAG, "HTTP_EVENT_ON_FINISH");
            output_len = 0;
            break;

        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(HTTP_TAG, "HTTP_EVENT_DISCONNECTED");
            output_len = 0;
            break;

        default:
            break;
    }
    return ESP_OK;
}

/**
 * @brief HTTP GET 请求
 */
static void http_get_request(const char *url)
{
    char local_response_buffer[MAX_HTTP_OUTPUT_BUFFER] = {0};

    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_GET,
        .event_handler = http_event_handler,
        .user_data = local_response_buffer,
        .disable_auto_redirect = true,
        .timeout_ms = 10000,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);

    /* 执行请求 */
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        int status_code = esp_http_client_get_status_code(client);
        int content_length = esp_http_client_get_content_length(client);

        ESP_LOGI(HTTP_TAG, "HTTP 状态码: %d, 内容长度: %d",
                 status_code, content_length);
        ESP_LOGI(HTTP_TAG, "响应: %s", local_response_buffer);
    } else {
        ESP_LOGE(HTTP_TAG, "HTTP GET 请求失败: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
}

/**
 * @brief HTTP POST 请求 (JSON)
 */
static void http_post_json(const char *url, const char *json_data)
{
    char local_response_buffer[MAX_HTTP_OUTPUT_BUFFER] = {0};

    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_POST,
        .event_handler = http_event_handler,
        .user_data = local_response_buffer,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);

    /* 设置请求头 */
    esp_http_client_set_header(client, "Content-Type", "application/json");

    /* 设置 POST 数据 */
    esp_http_client_set_post_field(client, json_data, strlen(json_data));

    /* 执行请求 */
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        ESP_LOGI(HTTP_TAG, "HTTP POST 成功, 状态码: %d",
                 esp_http_client_get_status_code(client));
    } else {
        ESP_LOGE(HTTP_TAG, "HTTP POST 请求失败: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
}
