/*
 * Auto-generated from: 04_Industrial_Scenarios\15_Industrial_Communication\Physical_Layer\02_I2C_Driver.md
 * Line: 255
 * Language: c
 * Block ID: 1dcef6a2
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * I2C主机接收数据
 */
int I2C_Master_Receive(I2C_TypeDef *I2Cx, uint16_t slave_addr,
                       uint8_t *data, uint16_t size, uint32_t timeout)
{
    uint32_t tickstart = get_tick();

    /* 使能ACK */
    I2Cx->CR1 |= I2C_CR1_ACK;

    /* 发送起始条件 */
    I2Cx->CR1 |= I2C_CR1_START;
    while (!(I2Cx->SR1 & I2C_SR1_SB));

    /* 发送从机地址 (读模式) */
    I2Cx->DR = (slave_addr << 1) | 0x01;
    while (!(I2Cx->SR1 & I2C_SR1_ADDR));

    if (size == 1) {
        /* 单字节接收 */
        I2Cx->CR1 &= ~I2C_CR1_ACK;  /* 禁用ACK */
        __disable_irq();  /* 临界区 */
        (void)I2Cx->SR1;
        (void)I2Cx->SR2;
        I2Cx->CR1 |= I2C_CR1_STOP;  /* 发送停止 */
        __enable_irq();

        while (!(I2Cx->SR1 & I2C_SR1_RXNE));
        *data = I2Cx->DR;
    } else if (size == 2) {
        /* 双字节接收 */
        I2Cx->CR1 |= I2C_CR1_POS;   /* 设置POS位 */
        __disable_irq();
        (void)I2Cx->SR1;
        (void)I2Cx->SR2;
        I2Cx->CR1 &= ~I2C_CR1_ACK;  /* 清除ACK */
        __enable_irq();

        while (!(I2Cx->SR1 & I2C_SR1_BTF));
        __disable_irq();
        I2Cx->CR1 |= I2C_CR1_STOP;
        *data++ = I2Cx->DR;
        __enable_irq();
        *data = I2Cx->DR;
    } else {
        /* 多字节接收 */
        (void)I2Cx->SR1;
        (void)I2Cx->SR2;

        for (uint16_t i = size; i > 0; i--) {
            if (i == 3) {
                /* 倒数第三字节 */
                while (!(I2Cx->SR1 & I2C_SR1_BTF));
                I2Cx->CR1 &= ~I2C_CR1_ACK;
                __disable_irq();
                *data++ = I2Cx->DR;
                I2Cx->CR1 |= I2C_CR1_STOP;
                __enable_irq();
            } else if (i == 2) {
                /* 倒数第二字节 */
                while (!(I2Cx->SR1 & I2C_SR1_BTF));
            }

            while (!(I2Cx->SR1 & I2C_SR1_RXNE));
            *data++ = I2Cx->DR;
        }
    }

    return I2C_OK;
}
