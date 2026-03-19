/*
 * Auto-generated from: 03_System_Technology_Domains\05_Wireless_Protocol\README.md
 * Line: 339
 * Language: c
 * Block ID: 73287a6d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 通用无线协议栈接口
typedef struct {
    int (*init)(void);
    int (*start)(void);
    int (*stop)(void);
    int (*send)(const void *data, size_t len);
    int (*register_recv_callback)(void (*cb)(const void *data, size_t len));
    int (*get_status)(void);
} WirelessStack;

// BLE 协议栈实例
WirelessStack ble_stack = {
    .init = ble_stack_init,
    .start = ble_stack_start,
    .stop = ble_stack_stop,
    .send = ble_stack_send,
    .register_recv_callback = ble_register_callback,
    .get_status = ble_get_status,
};

// 应用层使用
void wireless_app_init(void)
{
    ble_stack.init();
    ble_stack.register_recv_callback(on_ble_data_received);
    ble_stack.start();
}
