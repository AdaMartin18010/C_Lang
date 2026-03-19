/*
 * Auto-generated from: 04_Industrial_Scenarios\15_Industrial_Communication\Physical_Layer\02_I2C_Driver.md
 * Line: 461
 * Language: c
 * Block ID: 67b3bce7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * AT24Cxx EEPROM驱动
 * 容量: 1Kbit - 512Kbit
 */

#define AT24C_ADDR      0x50    /* 7位地址 */
#define AT24C_PAGE_SIZE 32      /* 页大小 */

/**
 * 写一个字节
 */
int AT24C_WriteByte(I2C_TypeDef *I2Cx, uint16_t mem_addr, uint8_t data)
{
    uint8_t tx_data[2];
    tx_data[0] = (mem_addr >> 8) & 0xFF;  /* 高地址 */
    tx_data[1] = mem_addr & 0xFF;          /* 低地址 */

    /* 发送地址 */
    int ret = I2C_Master_Transmit(I2Cx, AT24C_ADDR, tx_data, 2, 100);
    if (ret != I2C_OK) return ret;

    /* 发送数据 */
    ret = I2C_Master_Transmit(I2Cx, AT24C_ADDR, &data, 1, 100);

    /* 等待写入完成 */
    delay_ms(5);

    return ret;
}

/**
 * 页写
 */
int AT24C_PageWrite(I2C_TypeDef *I2Cx, uint16_t mem_addr,
                    const uint8_t *data, uint8_t size)
{
    if (size > AT24C_PAGE_SIZE) {
        size = AT24C_PAGE_SIZE;
    }

    uint8_t tx_data[AT24C_PAGE_SIZE + 2];
    tx_data[0] = (mem_addr >> 8) & 0xFF;
    tx_data[1] = mem_addr & 0xFF;
    memcpy(&tx_data[2], data, size);

    int ret = I2C_Master_Transmit(I2Cx, AT24C_ADDR, tx_data, size + 2, 100);

    delay_ms(5);

    return ret;
}

/**
 * 顺序读
 */
int AT24C_Read(I2C_TypeDef *I2Cx, uint16_t mem_addr, uint8_t *data, uint16_t size)
{
    uint8_t addr_data[2];
    addr_data[0] = (mem_addr >> 8) & 0xFF;
    addr_data[1] = mem_addr & 0xFF;

    /* 发送读地址 */
    int ret = I2C_Master_Transmit(I2Cx, AT24C_ADDR, addr_data, 2, 100);
    if (ret != I2C_OK) return ret;

    /* 接收数据 */
    ret = I2C_Master_Receive(I2Cx, AT24C_ADDR, data, size, 100);

    return ret;
}
