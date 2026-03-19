/*
 * Auto-generated from: 04_Industrial_Scenarios\14_Embedded_IoT\RaspberryPi\01_RaspberryPi_Embedded_Linux.md
 * Line: 963
 * Language: c
 * Block ID: 78134ad5
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// gpio_control.c - libgpiod GPIO控制
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <gpiod.h>
#include <pthread.h>

#define GPIO_CHIP       "gpiochip0"
#define GPIO_LED        16
#define GPIO_BUTTON     20
#define GPIO_INPUT      21
#define GPIO_RELAY1     23
#define GPIO_RELAY2     24

static volatile int running = 1;
static pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;

static void signal_handler(int sig)
{
    (void)sig;
    running = 0;
}

static void get_timestamp(char *buf, size_t len)
{
    struct timespec ts;
    struct tm tm_info;

    clock_gettime(CLOCK_REALTIME, &ts);
    localtime_r(&ts.tv_sec, &tm_info);

    snprintf(buf, len, "%04d-%02d-%02d %02d:%02d:%02d.%03ld",
             tm_info.tm_year + 1900, tm_info.tm_mon + 1, tm_info.tm_mday,
             tm_info.tm_hour, tm_info.tm_min, tm_info.tm_sec,
             ts.tv_nsec / 1000000);
}

int gpio_basic_example(void)
{
    struct gpiod_chip *chip;
    struct gpiod_line *led_line;
    struct gpiod_line *btn_line;
    int ret = 0;

    printf("=== Basic GPIO Control ===\n");

    chip = gpiod_chip_open_by_name(GPIO_CHIP);
    if (!chip) {
        fprintf(stderr, "Failed to open GPIO chip\n");
        return -1;
    }

    led_line = gpiod_chip_get_line(chip, GPIO_LED);
    if (!led_line) { ret = -1; goto close_chip; }

    ret = gpiod_line_request_output(led_line, "gpio_basic", 0);
    if (ret < 0) goto release_led;

    btn_line = gpiod_chip_get_line(chip, GPIO_BUTTON);
    if (!btn_line) { ret = -1; goto release_led; }

    ret = gpiod_line_request_input(btn_line, "gpio_basic");
    if (ret < 0) goto release_btn;

    printf("LED blinking (press Ctrl+C to stop)...\n");
    for (int i = 0; i < 10 && running; i++) {
        gpiod_line_set_value(led_line, 1);
        printf("LED ON, Button: %d\n", gpiod_line_get_value(btn_line));
        usleep(500000);

        gpiod_line_set_value(led_line, 0);
        printf("LED OFF, Button: %d\n", gpiod_line_get_value(btn_line));
        usleep(500000);
    }
    printf("\n");

release_btn:
    gpiod_line_release(btn_line);
release_led:
    gpiod_line_release(led_line);
close_chip:
    gpiod_chip_close(chip);
    return ret;
}

void *gpio_interrupt_thread(void *arg)
{
    struct gpiod_chip *chip;
    struct gpiod_line *line;
    struct gpiod_line_event event;
    struct timespec ts;
    char timestamp[32];
    int ret;
    int pin = *(int *)arg;

    chip = gpiod_chip_open_by_name(GPIO_CHIP);
    if (!chip) return NULL;

    line = gpiod_chip_get_line(chip, pin);
    if (!line) { gpiod_chip_close(chip); return NULL; }

    ret = gpiod_line_request_both_edges_events(line, "gpio_interrupt");
    if (ret < 0) {
        gpiod_line_release(line);
        gpiod_chip_close(chip);
        return NULL;
    }

    pthread_mutex_lock(&print_mutex);
    printf("Interrupt monitoring started on GPIO%d\n", pin);
    pthread_mutex_unlock(&print_mutex);

    while (running) {
        ts.tv_sec = 1;
        ts.tv_nsec = 0;

        ret = gpiod_line_event_wait(line, &ts);
        if (ret < 0) {
            if (errno == EINTR) continue;
            break;
        }
        if (ret == 0) continue;

        ret = gpiod_line_event_read(line, &event);
        if (ret < 0) continue;

        get_timestamp(timestamp, sizeof(timestamp));

        pthread_mutex_lock(&print_mutex);
        printf("[%s] GPIO%d: %s edge detected\n",
               timestamp, pin,
               event.event_type == GPIOD_LINE_EVENT_RISING_EDGE ? "Rising" : "Falling");
        pthread_mutex_unlock(&print_mutex);
    }

    gpiod_line_release(line);
    gpiod_chip_close(chip);
    return NULL;
}

int gpio_bulk_example(void)
{
    struct gpiod_chip *chip;
    struct gpiod_line_bulk lines;
    struct gpiod_line *line_list[4];
    int values[4] = {1, 0, 1, 0};
    int ret;
    int pins[] = {GPIO_RELAY1, GPIO_RELAY2, GPIO_LED, GPIO_INPUT};

    printf("=== Bulk GPIO Control ===\n");

    chip = gpiod_chip_open_by_name(GPIO_CHIP);
    if (!chip) return -1;

    gpiod_line_bulk_init(&lines);

    for (int i = 0; i < 4; i++) {
        line_list[i] = gpiod_chip_get_line(chip, pins[i]);
        if (!line_list[i]) { ret = -1; goto cleanup; }
        gpiod_line_bulk_add(&lines, line_list[i]);
    }

    ret = gpiod_line_request_bulk_output(&lines, "gpio_bulk", values);
    if (ret < 0) goto cleanup;

    printf("Bulk values set: [%d, %d, %d, %d]\n",
           values[0], values[1], values[2], values[3]);

    int new_values[4] = {0, 1, 0, 1};
    gpiod_line_set_value_bulk(&lines, new_values);
    printf("Bulk values updated: [%d, %d, %d, %d]\n",
           new_values[0], new_values[1], new_values[2], new_values[3]);

    sleep(1);

cleanup:
    gpiod_line_release_bulk(&lines);
    gpiod_chip_close(chip);
    return ret;
}

int main(int argc, char *argv[])
{
    pthread_t int_thread;
    int int_pin = GPIO_INPUT;

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    printf("Raspberry Pi GPIO Control Demo\n");
    printf("==============================\n\n");

    gpio_basic_example();
    gpio_bulk_example();

    pthread_create(&int_thread, NULL, gpio_interrupt_thread, &int_pin);

    printf("\nPress Ctrl+C to exit...\n");
    while (running) sleep(1);

    pthread_join(int_thread, NULL);

    printf("\nProgram exiting...\n");
    return 0;
}
