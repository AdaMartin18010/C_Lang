/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\ESP32\01_ESP32_Complete_Guide.md
 * Line: 3890
 * Language: c
 * Block ID: 9415ee73
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 优化 WiFi 连接稳定性 */

static void optimize_wifi_stability(void)
{
    /* 1. 启用 AMSDU (提高吞吐量) */
    esp_wifi_set_protocol(WIFI_IF_STA, WIFI_PROTOCOL_11B |
                          WIFI_PROTOCOL_11G | WIFI_PROTOCOL_11N);

    /* 2. 设置静态 IP (可选,加快连接速度) */
    esp_netif_ip_info_t ip_info = {
        .ip.addr = ipaddr_addr("192.168.1.100"),
        .netmask.addr = ipaddr_addr("255.255.255.0"),
        .gw.addr = ipaddr_addr("192.168.1.1"),
    };
    esp_netif_set_ip_info(sta_netif, &ip_info);

    /* 3. 启用自动重连 */
    esp_wifi_set_auto_connect(true);

    /* 4. 配置省电模式 (根据需要) */
    esp_wifi_set_ps(WIFI_PS_MIN_MODEM);
}
