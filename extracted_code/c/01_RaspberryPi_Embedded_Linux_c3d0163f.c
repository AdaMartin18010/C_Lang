/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\RaspberryPi\01_RaspberryPi_Embedded_Linux.md
 * Line: 1310
 * Language: c
 * Block ID: c3d0163f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// spi_userspace.c - 用户空间SPI访问
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>
#include <linux/gpio.h>
#include <gpiod.h>

#define SPI_DEVICE      "/dev/spidev0.0"
#define SPI_SPEED       1000000
#define GPIO_CS         25

/* ADS1118配置 */
#define ADS1118_CONFIG_OS       (1 << 15)
#define ADS1118_CONFIG_MUX_SHIFT 12
#define ADS1118_CONFIG_PGA_SHIFT 9
#define ADS1118_CONFIG_MODE     (1 << 8)
#define ADS1118_CONFIG_DR_SHIFT 5
#define ADS1118_CONFIG_PULL_UP  (1 << 3)

static uint16_t ads1118_make_config(uint8_t channel, uint8_t pga, uint8_t dr)
{
    uint16_t config = 0;
    config |= ADS1118_CONFIG_OS;
    config |= ((channel & 0x07) << ADS1118_CONFIG_MUX_SHIFT);
    config |= ((pga & 0x07) << ADS1118_CONFIG_PGA_SHIFT);
    config |= ADS1118_CONFIG_MODE;
    config |= ((dr & 0x07) << ADS1118_CONFIG_DR_SHIFT);
    config |= ADS1118_CONFIG_PULL_UP;
    config |= 0x0003;
    return config;
}

int spi_transfer(int fd, uint8_t *tx, uint8_t *rx, size_t len, int gpio_cs)
{
    struct spi_ioc_transfer tr = {
        .tx_buf = (unsigned long)tx,
        .rx_buf = (unsigned long)rx,
        .len = len,
        .speed_hz = SPI_SPEED,
        .delay_usecs = 0,
        .bits_per_word = 8,
    };

    if (gpio_cs >= 0) {
        struct gpiod_chip *chip = gpiod_chip_open_by_name("gpiochip0");
        if (chip) {
            struct gpiod_line *line = gpiod_chip_get_line(chip, gpio_cs);
            if (line) {
                gpiod_line_request_output(line, "spi_cs", 0);
                gpiod_line_set_value(line, 0);
                usleep(10);
            }
        }
    }

    int ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);

    if (gpio_cs >= 0) {
        struct gpiod_chip *chip = gpiod_chip_open_by_name("gpiochip0");
        if (chip) {
            struct gpiod_line *line = gpiod_chip_get_line(chip, gpio_cs);
            if (line) {
                gpiod_line_set_value(line, 1);
            }
            gpiod_chip_close(chip);
        }
    }

    return ret;
}

int ads1118_read(int fd, uint8_t channel, int gpio_cs)
{
    uint16_t config = ads1118_make_config(channel, 0x01, 0x04);
    uint8_t tx_buf[4];
    uint8_t rx_buf[4];
    int16_t result;

    tx_buf[0] = (config >> 8) & 0xFF;
    tx_buf[1] = config & 0xFF;
    tx_buf[2] = tx_buf[0];
    tx_buf[3] = tx_buf[1];

    spi_transfer(fd, tx_buf, rx_buf, 4, gpio_cs);
    usleep(10000);

    tx_buf[0] = 0;
    tx_buf[1] = 0;
    tx_buf[2] = 0;
    tx_buf[3] = 0;

    spi_transfer(fd, tx_buf, rx_buf, 4, gpio_cs);

    result = ((int16_t)rx_buf[0] << 8) | rx_buf[1];
    return result;
}

int main(int argc, char *argv[])
{
    int fd;
    int ret;
    uint8_t mode = SPI_MODE_1;
    uint8_t bits = 8;
    uint32_t speed = SPI_SPEED;

    fd = open(SPI_DEVICE, O_RDWR);
    if (fd < 0) {
        perror("Failed to open SPI device");
        return 1;
    }

    ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
    ret |= ioctl(fd, SPI_IOC_RD_MODE, &mode);
    ret |= ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
    ret |= ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
    ret |= ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
    ret |= ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);

    if (ret < 0) {
        perror("Failed to configure SPI");
        close(fd);
        return 1;
    }

    printf("SPI User Space Access Demo\n");
    printf("==========================\n");
    printf("Mode: %d, Bits: %d, Speed: %d Hz\n\n", mode, bits, speed);

    printf("=== ADS1118 ADC Read ===\n");
    for (int i = 0; i < 4; i++) {
        int val = ads1118_read(fd, i, GPIO_CS);
        float voltage = (val * 4.096) / 32768.0;
        printf("Channel %d: %d (%.3f V)\n", i, val, voltage);
        usleep(1000);
    }

    close(fd);
    return 0;
}
