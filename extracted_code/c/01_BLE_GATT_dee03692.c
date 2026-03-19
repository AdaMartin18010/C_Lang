/*
 * Auto-generated from: 03_System_Technology_Domains\05_Wireless_Protocol\01_BLE_GATT.md
 * Line: 314
 * Language: c
 * Block ID: dee03692
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* 注册GATT服务到ATT数据库 */
int gatt_register_service(AttDatabase *db, GattService *service) {
    uint16_t handle = db->next_handle;
    service->start_handle = handle;

    /* 1. 服务声明 */
    AttAttribute *svc_decl = &db->attrs[db->attr_count++];
    svc_decl->handle = handle++;
    svc_decl->uuid16 = service->is_primary ?
                       ATT_UUID_PRIMARY_SERVICE : ATT_UUID_SECONDARY_SERVICE;
    svc_decl->permissions = ATT_PERM_READ;
    svc_decl->value_len = 2;
    svc_decl->value = (uint8_t[]){service->uuid16 & 0xFF,
                                   service->uuid16 >> 8};

    /* 2. 特征声明和值 */
    for (int i = 0; i < service->char_count; i++) {
        GattCharacteristic *chr = &service->characteristics[i];

        /* 特征声明 */
        AttAttribute *chr_decl = &db->attrs[db->attr_count++];
        chr_decl->handle = handle++;
        chr_decl->uuid16 = ATT_UUID_CHARACTERISTIC;
        chr_decl->permissions = ATT_PERM_READ;
        chr_decl->value_len = 3 + 2;  /* 属性(1) + 值句柄(2) + UUID(2) */
        chr_decl->value = malloc(5);
        chr_decl->value[0] = chr->properties;
        chr_decl->value[1] = (handle + 1) & 0xFF;
        chr_decl->value[2] = (handle + 1) >> 8;
        chr_decl->value[3] = chr->uuid16 & 0xFF;
        chr_decl->value[4] = chr->uuid16 >> 8;

        /* 特征值 */
        AttAttribute *chr_val = &db->attrs[db->attr_count++];
        chr_val->handle = handle++;
        chr_val->uuid16 = chr->uuid16;
        chr_val->permissions = chr->permissions;
        chr_val->value_max = chr->max_len;
        chr_val->value_len = chr->value_len;
        chr_val->value = chr->value;
        chr_val->on_read = (void*)chr->on_read;
        chr_val->on_write = (void*)chr->on_write;

        /* CCCD（如果支持notify/indicate） */
        if (chr->properties & (GATT_PROP_NOTIFY | GATT_PROP_INDICATE)) {
            AttAttribute *cccd = &db->attrs[db->attr_count++];
            cccd->handle = handle++;
            cccd->uuid16 = ATT_UUID_CLIENT_CHAR_CFG;
            cccd->permissions = ATT_PERM_READ | ATT_PERM_WRITE;
            cccd->value_max = 2;
            cccd->value_len = 2;
            cccd->value = (uint8_t*)&chr->cccd_value;
            chr->cccd_handle = cccd->handle;
        }
    }

    service->end_handle = handle - 1;
    db->next_handle = handle;

    return 0;
}
