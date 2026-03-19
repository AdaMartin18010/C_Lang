/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\RaspberryPi\README.md
 * Line: 374
 * Language: c
 * Block ID: 46e0b44f
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// libgpiod C 示例
gpio.c
#include <gpiod.h>
#include <stdio.h>

int main() {
    struct gpiod_chip *chip = gpiod_chip_open("/dev/gpiochip0");
    struct gpiod_line *line = gpiod_chip_get_line(chip, 17);

    gpiod_line_request_output(line, "myapp", 0);
    gpiod_line_set_value(line, 1);
    sleep(1);
    gpiod_line_set_value(line, 0);

    gpiod_line_release(line);
    gpiod_chip_close(chip);
    return 0;
}
