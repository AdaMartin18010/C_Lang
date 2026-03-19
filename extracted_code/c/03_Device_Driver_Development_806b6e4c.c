/*
 * Auto-generated from: 04_Industrial_Scenarios\13_Linux_Kernel\03_Device_Driver_Development.md
 * Line: 7546
 * Language: c
 * Block ID: 806b6e4c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * File: test_gpio_keyboard.c
 * Description: GPIO键盘测试程序
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <sys/select.h>

#define INPUT_DEVICE "/dev/input/event0"

const char *keycode_to_name(int code)
{
    switch (code) {
    case KEY_1: return "1";
    case KEY_2: return "2";
    case KEY_3: return "3";
    case KEY_4: return "4";
    case KEY_5: return "5";
    case KEY_6: return "6";
    case KEY_7: return "7";
    case KEY_8: return "8";
    case KEY_9: return "9";
    case KEY_0: return "0";
    case KEY_A: return "A";
    case KEY_B: return "B";
    case KEY_C: return "C";
    case KEY_D: return "D";
    case KEY_ESC: return "ESC";
    case KEY_ENTER: return "ENTER";
    default: return "UNKNOWN";
    }
}

int main(void)
{
    int fd;
    struct input_event ev;
    fd_set readfds;
    int ret;

    printf("=== GPIO Keyboard Test ===\n");

    fd = open(INPUT_DEVICE, O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
        perror("Failed to open input device");
        printf("Try: sudo chmod 644 %s\n", INPUT_DEVICE);
        return 1;
    }

    printf("Reading from %s...\n", INPUT_DEVICE);
    printf("Press keys on the GPIO keyboard (Ctrl+C to exit)\n\n");

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(fd, &readfds);

        ret = select(fd + 1, &readfds, NULL, NULL, NULL);
        if (ret < 0) {
            perror("select");
            break;
        }

        if (FD_ISSET(fd, &readfds)) {
            ret = read(fd, &ev, sizeof(ev));
            if (ret < 0) {
                perror("read");
                break;
            }

            if (ev.type == EV_KEY) {
                printf("Key: %s (code %d) %s\n",
                       keycode_to_name(ev.code),
                       ev.code,
                       ev.value == 0 ? "released" :
                       ev.value == 1 ? "pressed" : "repeated");
            }
        }
    }

    close(fd);
    return 0;
}
