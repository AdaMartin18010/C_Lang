/*
 * Auto-generated from: 04_Industrial_Scenarios\15_Industrial_Communication\Physical_Layer\02_I2C_Driver.md
 * Line: 181
 * Language: c
 * Block ID: 7c3aaf97
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * I2C主机发送数据
 *
 * @param I2Cx I2C实例
 * @param slave_addr 从机地址 (7位，左对齐)
 * @param data 数据缓冲区
 * @param size 数据长度
 * @return 0=成功, 其他=错误码
 */
int I2C_Master_Transmit(I2C_TypeDef *I2Cx, uint16_t slave_addr,
                        const uint8_t *data, uint16_t size, uint32_t timeout)
{
    uint32_t tickstart = get_tick();

    /* 等待总线空闲 */
    while (I2Cx->SR2 & (1U << 1)) {
        if ((get_tick() - tickstart) > timeout) {
            return I2C_ERROR_BUSY;
        }
    }

    /* 发送起始条件 */
    I2Cx->CR1 |= I2C_CR1_START;

    /* 等待起始条件发送完成 */
    while (!(I2Cx->SR1 & I2C_SR1_SB)) {
        if ((get_tick() - tickstart) > timeout) {
            return I2C_ERROR_TIMEOUT;
        }
    }

    /* 发送从机地址 (写模式) */
    I2Cx->DR = (slave_addr << 1) & 0xFE;

    /* 等待地址发送完成 */
    while (!(I2Cx->SR1 & I2C_SR1_ADDR)) {
        if ((get_tick() - tickstart) > timeout) {
            return I2C_ERROR_ADDR;
        }
    }

    /* 清除ADDR标志 */
    (void)I2Cx->SR1;
    (void)I2Cx->SR2;

    /* 发送数据 */
    for (uint16_t i = 0; i < size; i++) {
        /* 等待发送寄存器空 */
        while (!(I2Cx->SR1 & I2C_SR1_TXE)) {
            if ((get_tick() - tickstart) > timeout) {
                return I2C_ERROR_TIMEOUT;
            }
        }

        I2Cx->DR = data[i];
    }

    /* 等待字节发送完成 */
    while (!(I2Cx->SR1 & I2C_SR1_BTF)) {
        if ((get_tick() - tickstart) > timeout) {
            return I2C_ERROR_TIMEOUT;
        }
    }

    /* 发送停止条件 */
    I2Cx->CR1 |= I2C_CR1_STOP;

    return I2C_OK;
}
