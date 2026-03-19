/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\RaspberryPi\01_RaspberryPi_Embedded_Linux.md
 * Line: 1180
 * Language: c
 * Block ID: d4249bf6
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// i2c_userspace.c - 用户空间I2C访问
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <errno.h>

#define I2C_BUS     "/dev/i2c-1"
#define MCP23017_ADDR   0x20
#define BME280_ADDR     0x76

int i2c_smbus_read_byte(int file, uint8_t reg, uint8_t *val)
{
    union i2c_smbus_data data;
    struct i2c_smbus_ioctl_data args;

    args.read_write = I2C_SMBUS_READ;
    args.command = reg;
    args.size = I2C_SMBUS_BYTE_DATA;
    args.data = &data;

    if (ioctl(file, I2C_SMBUS, &args) < 0) return -1;

    *val = data.byte;
    return 0;
}

int i2c_smbus_write_byte(int file, uint8_t reg, uint8_t val)
{
    union i2c_smbus_data data;
    struct i2c_smbus_ioctl_data args;

    data.byte = val;
    args.read_write = I2C_SMBUS_WRITE;
    args.command = reg;
    args.size = I2C_SMBUS_BYTE_DATA;
    args.data = &data;

    return ioctl(file, I2C_SMBUS, &args);
}

int mcp23017_test(int file)
{
    uint8_t val;
    int ret;

    printf("=== MCP23017 Test ===\n");

    if (ioctl(file, I2C_SLAVE, MCP23017_ADDR) < 0) {
        perror("Failed to set I2C address");
        return -1;
    }

    ret = i2c_smbus_read_byte(file, 0x00, &val);
    if (ret < 0) { perror("Failed to read IODIR"); return -1; }
    printf("IODIRA: 0x%02X\n", val);

    i2c_smbus_read_byte(file, 0x12, &val);
    printf("GPIOA: 0x%02X\n", val);

    i2c_smbus_read_byte(file, 0x13, &val);
    printf("GPIOB: 0x%02X\n", val);

    return 0;
}

int i2c_scan(int file)
{
    int addr;
    int found = 0;

    printf("=== I2C Bus Scan ===\n");
    printf("     0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f\n");
    printf("00:          ");

    for (addr = 0x03; addr <= 0x77; addr++) {
        if (addr % 16 == 0) printf("\n%02x: ", addr);

        if (ioctl(file, I2C_SLAVE, addr) < 0) {
            printf("-- ");
            continue;
        }

        uint8_t buf;
        if (read(file, &buf, 1) == 1) {
            printf("%02X ", addr);
            found++;
        } else {
            printf("-- ");
        }
    }

    printf("\n\nFound %d device(s)\n", found);
    return found;
}

int main(int argc, char *argv[])
{
    int file;

    file = open(I2C_BUS, O_RDWR);
    if (file < 0) {
        perror("Failed to open I2C bus");
        printf("Try: sudo %s\n", argv[0]);
        return 1;
    }

    printf("I2C User Space Access Demo\n");
    printf("==========================\n\n");

    i2c_scan(file);
    printf("\n");
    mcp23017_test(file);

    close(file);
    return 0;
}
