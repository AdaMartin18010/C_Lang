/*
 * Auto-generated from: 04_Industrial_Scenarios\15_Industrial_Communication\Data_Link_Layer\01_Data_Link_Protocols.md
 * Line: 4648
 * Language: c
 * Block ID: c4d21558
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/**
 * @file flow_control.c
 * @brief Data Link Layer Flow Control Implementations
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

/* ============================================================================
 * Sliding Window Protocol
 * ============================================================================ */

#define WINDOW_SIZE         8       /**< Window size */
#define MAX_SEQ_NUM         16      /**< Maximum sequence number */
#define FRAME_TIMEOUT_MS    1000    /**< Frame timeout in milliseconds */
#define MAX_FRAME_SIZE      256     /**< Maximum frame size */
#define BUFFER_SIZE         (WINDOW_SIZE * MAX_FRAME_SIZE)

/**
 * @brief Frame structure
 */
typedef struct {
    uint8_t seq_num;         /**< Sequence number */
    uint8_t ack_num;         /**< Acknowledgment number */
    uint16_t len;            /**< Data length */
    uint8_t data[MAX_FRAME_SIZE]; /**< Data payload */
} window_frame_t;

/**
 * @brief Sliding window transmitter state
 */
typedef struct {
    uint8_t base;            /**< Base of window */
    uint8_t next_seq;        /**< Next sequence number to use */
    uint8_t window_size;     /**< Current window size */
    bool acked[WINDOW_SIZE]; /**< Acknowledgment status */
    window_frame_t frames[WINDOW_SIZE]; /**< Buffered frames */
    clock_t send_time[WINDOW_SIZE]; /**< Send timestamps */
} sw_transmitter_t;

/**
 * @brief Sliding window receiver state
 */
typedef struct {
    uint8_t expected_seq;    /**< Next expected sequence number */
    uint8_t window_size;     /**< Window size */
    bool received[WINDOW_SIZE]; /**< Reception status */
    window_frame_t frames[WINDOW_SIZE]; /**< Received frames buffer */
} sw_receiver_t;

/**
 * @brief Initialize sliding window transmitter
 * @param tx Transmitter state
 * @param window_size Window size
 */
void sw_init_transmitter(sw_transmitter_t *tx, uint8_t window_size) {
    if (tx == NULL) return;

    memset(tx, 0, sizeof(sw_transmitter_t));
    tx->base = 0;
    tx->next_seq = 0;
    tx->window_size = window_size < WINDOW_SIZE ? window_size : WINDOW_SIZE;

    for (int i = 0; i < WINDOW_SIZE; i++) {
        tx->acked[i] = true;  /* Initially all slots are free */
    }
}

/**
 * @brief Initialize sliding window receiver
 * @param rx Receiver state
 * @param window_size Window size
 */
void sw_init_receiver(sw_receiver_t *rx, uint8_t window_size) {
    if (rx == NULL) return;

    memset(rx, 0, sizeof(sw_receiver_t));
    rx->expected_seq = 0;
    rx->window_size = window_size < WINDOW_SIZE ? window_size : WINDOW_SIZE;

    for (int i = 0; i < WINDOW_SIZE; i++) {
        rx->received[i] = false;
    }
}

/**
 * @brief Check if transmitter can send
 * @param tx Transmitter state
 * @return true if can send, false otherwise
 */
bool sw_can_send(const sw_transmitter_t *tx) {
    if (tx == NULL) return false;

    uint8_t available = (tx->base + tx->window_size - tx->next_seq) % MAX_SEQ_NUM;
    return available < tx->window_size;
}

/**
 * @brief Send frame using sliding window
 * @param tx Transmitter state
 * @param data Data to send
 * @param len Data length
 * @return Sequence number, or -1 on error
 */
int sw_send(sw_transmitter_t *tx, const uint8_t *data, uint16_t len) {
    if (tx == NULL || data == NULL || len > MAX_FRAME_SIZE) {
        return -1;
    }

    if (!sw_can_send(tx)) {
        printf("Window full, cannot send\n");
        return -1;
    }

    uint8_t seq = tx->next_seq;
    uint8_t idx = seq % WINDOW_SIZE;

    tx->frames[idx].seq_num = seq;
    tx->frames[idx].len = len;
    memcpy(tx->frames[idx].data, data, len);
    tx->acked[idx] = false;
    tx->send_time[idx] = clock();

    tx->next_seq = (tx->next_seq + 1) % MAX_SEQ_NUM;

    printf("TX: Sent frame with seq=%d, len=%d\n", seq, len);
    return seq;
}

/**
 * @brief Process acknowledgment
 * @param tx Transmitter state
 * @param ack_num Acknowledgment number
 */
void sw_process_ack(sw_transmitter_t *tx, uint8_t ack_num) {
    if (tx == NULL) return;

    /* Cumulative acknowledgment - mark all up to ack_num as acked */
    while (tx->base != ack_num) {
        uint8_t idx = tx->base % WINDOW_SIZE;
        tx->acked[idx] = true;
        printf("TX: Frame %d acknowledged\n", tx->base);
        tx->base = (tx->base + 1) % MAX_SEQ_NUM;
    }
}

/**
 * @brief Check for timeouts and return timed out frames
 * @param tx Transmitter state
 * @param timed_out_seqs Output array for timed out sequence numbers
 * @param max_count Maximum number to return
 * @return Number of timed out frames
 */
int sw_check_timeouts(sw_transmitter_t *tx, uint8_t *timed_out_seqs, int max_count) {
    if (tx == NULL || timed_out_seqs == NULL) return 0;

    int count = 0;
    clock_t now = clock();

    for (uint8_t seq = tx->base; seq != tx->next_seq; seq = (seq + 1) % MAX_SEQ_NUM) {
        uint8_t idx = seq % WINDOW_SIZE;

        if (!tx->acked[idx]) {
            double elapsed_ms = ((double)(now - tx->send_time[idx]) / CLOCKS_PER_SEC) * 1000;

            if (elapsed_ms > FRAME_TIMEOUT_MS) {
                if (count < max_count) {
                    timed_out_seqs[count++] = seq;
                    printf("TX: Frame %d timed out (%.0f ms)\n", seq, elapsed_ms);
                }
            }
        }
    }

    return count;
}

/**
 * @brief Receive frame using sliding window
 * @param rx Receiver state
 * @param frame Received frame
 * @param output_data Output buffer for in-order data
 * @param output_max Maximum output buffer size
 * @return Number of bytes delivered, or -1 on error
 */
int sw_receive(sw_receiver_t *rx, const window_frame_t *frame,
               uint8_t *output_data, uint16_t output_max) {
    if (rx == NULL || frame == NULL || output_data == NULL) return -1;

    uint8_t seq = frame->seq_num;
    uint8_t idx = seq % WINDOW_SIZE;

    /* Check if frame is within receive window */
    uint8_t window_end = (rx->expected_seq + rx->window_size) % MAX_SEQ_NUM;
    bool in_window;

    if (rx->expected_seq < window_end) {
        in_window = (seq >= rx->expected_seq && seq < window_end);
    } else {
        in_window = (seq >= rx->expected_seq || seq < window_end);
    }

    if (!in_window) {
        printf("RX: Frame %d outside window (expected %d)\n", seq, rx->expected_seq);
        return 0;  /* Still need to send ACK */
    }

    /* Store frame */
    rx->frames[idx] = *frame;
    rx->received[idx] = true;
    printf("RX: Received frame %d\n", seq);

    /* Deliver in-order frames */
    uint16_t delivered = 0;
    while (rx->received[rx->expected_seq % WINDOW_SIZE]) {
        idx = rx->expected_seq % WINDOW_SIZE;
        window_frame_t *f = &rx->frames[idx];

        if (delivered + f->len <= output_max) {
            memcpy(&output_data[delivered], f->data, f->len);
            delivered += f->len;
        }

        rx->received[idx] = false;
        printf("RX: Delivered frame %d\n", rx->expected_seq);
        rx->expected_seq = (rx->expected_seq + 1) % MAX_SEQ_NUM;
    }

    return delivered;
}

/**
 * @brief Get expected sequence number for ACK
 * @param rx Receiver state
 * @return Expected sequence number
 */
uint8_t sw_get_ack_num(const sw_receiver_t *rx) {
    return rx ? rx->expected_seq : 0;
}

/* ============================================================================
 * Stop-and-Wait Protocol
 * ============================================================================ */

/**
 * @brief Stop-and-Wait transmitter state
 */
typedef struct {
    uint8_t seq_num;         /**< Current sequence number */
    bool waiting_for_ack;    /**< Waiting for acknowledgment */
    clock_t send_time;       /**< Send timestamp */
    window_frame_t last_frame; /**< Last sent frame for retransmission */
} saw_transmitter_t;

/**
 * @brief Stop-and-Wait receiver state
 */
typedef struct {
    uint8_t expected_seq;    /**< Expected sequence number */
} saw_receiver_t;

/**
 * @brief Initialize Stop-and-Wait transmitter
 * @param tx Transmitter state
 */
void saw_init_transmitter(saw_transmitter_t *tx) {
    if (tx == NULL) return;

    memset(tx, 0, sizeof(saw_transmitter_t));
    tx->seq_num = 0;
    tx->waiting_for_ack = false;
}

/**
 * @brief Initialize Stop-and-Wait receiver
 * @param rx Receiver state
 */
void saw_init_receiver(saw_receiver_t *rx) {
    if (rx == NULL) return;

    memset(rx, 0, sizeof(saw_receiver_t));
    rx->expected_seq = 0;
}

/**
 * @brief Send frame using Stop-and-Wait
 * @param tx Transmitter state
 * @param data Data to send
 * @param len Data length
 * @return true on success, false if waiting for ACK
 */
bool saw_send(saw_transmitter_t *tx, const uint8_t *data, uint16_t len) {
    if (tx == NULL || data == NULL || len > MAX_FRAME_SIZE) {
        return false;
    }

    if (tx->waiting_for_ack) {
        printf("S&W TX: Still waiting for ACK of frame %d\n", tx->seq_num ^ 1);
        return false;
    }

    tx->last_frame.seq_num = tx->seq_num;
    tx->last_frame.len = len;
    memcpy(tx->last_frame.data, data, len);
    tx->send_time = clock();
    tx->waiting_for_ack = true;

    printf("S&W TX: Sent frame with seq=%d, len=%d\n", tx->seq_num, len);
    return true;
}

/**
 * @brief Process ACK in Stop-and-Wait
 * @param tx Transmitter state
 * @param ack_num Acknowledgment number
 * @return true if valid ACK, false otherwise
 */
bool saw_process_ack(saw_transmitter_t *tx, uint8_t ack_num) {
    if (tx == NULL || !tx->waiting_for_ack) return false;

    /* In S&W, ACK contains expected sequence number */
    if (ack_num == ((tx->seq_num + 1) % 2)) {
        printf("S&W TX: ACK %d received, toggle sequence\n", ack_num);
        tx->seq_num = ack_num;
        tx->waiting_for_ack = false;
        return true;
    }

    printf("S&W TX: Unexpected ACK %d (expected %d)\n",
           ack_num, (tx->seq_num + 1) % 2);
    return false;
}

/**
 * @brief Check for timeout and retransmit if needed
 * @param tx Transmitter state
 * @return true if retransmission needed, false otherwise
 */
bool saw_check_timeout(saw_transmitter_t *tx) {
    if (tx == NULL || !tx->waiting_for_ack) return false;

    clock_t now = clock();
    double elapsed_ms = ((double)(now - tx->send_time) / CLOCKS_PER_SEC) * 1000;

    if (elapsed_ms > FRAME_TIMEOUT_MS) {
        printf("S&W TX: Timeout! Retransmitting frame %d\n", tx->seq_num);
        tx->send_time = now;
        return true;
    }

    return false;
}

/**
 * @brief Receive frame using Stop-and-Wait
 * @param rx Receiver state
 * @param frame Received frame
 * @param output_data Output buffer
 * @param output_max Maximum output size
 * @return Number of bytes delivered, or -1 on error
 */
int saw_receive(saw_receiver_t *rx, const window_frame_t *frame,
                uint8_t *output_data, uint16_t output_max) {
    if (rx == NULL || frame == NULL || output_data == NULL) return -1;

    if (frame->seq_num == rx->expected_seq) {
        /* Correct frame received */
        uint16_t len = frame->len < output_max ? frame->len : output_max;
        memcpy(output_data, frame->data, len);

        printf("S&W RX: Received expected frame %d, delivering %d bytes\n",
               frame->seq_num, len);

        /* Toggle expected sequence */
        rx->expected_seq = (rx->expected_seq + 1) % 2;
        return len;
    } else {
        /* Duplicate or out-of-order frame */
        printf("S&W RX: Unexpected frame %d (expected %d)\n",
               frame->seq_num, rx->expected_seq);
        return 0;  /* Still need to ACK */
    }
}

/**
 * @brief Get expected sequence number for ACK in S&W
 * @param rx Receiver state
 * @return Expected sequence number
 */
uint8_t saw_get_ack_num(const saw_receiver_t *rx) {
    return rx ? rx->expected_seq : 0;
}

/* ============================================================================
 * Rate-Based Flow Control
 * ============================================================================ */

/**
 * @brief Token bucket for rate control
 */
typedef struct {
    uint32_t tokens;         /**< Current tokens */
    uint32_t bucket_size;    /**< Maximum bucket size */
    uint32_t rate;           /**< Token refill rate (tokens per second) */
    clock_t last_update;     /**< Last update time */
} token_bucket_t;

/**
 * @brief Initialize token bucket
 * @param bucket Token bucket
 * @param rate Token rate (tokens per second)
 * @param burst_size Maximum bucket size (burst capacity)
 */
void token_bucket_init(token_bucket_t *bucket, uint32_t rate, uint32_t burst_size) {
    if (bucket == NULL) return;

    bucket->tokens = burst_size;  /* Start with full bucket */
    bucket->bucket_size = burst_size;
    bucket->rate = rate;
    bucket->last_update = clock();
}

/**
 * @brief Add tokens to bucket based on elapsed time
 * @param bucket Token bucket
 */
void token_bucket_refill(token_bucket_t *bucket) {
    if (bucket == NULL) return;

    clock_t now = clock();
    double elapsed_sec = (double)(now - bucket->last_update) / CLOCKS_PER_SEC;

    uint32_t new_tokens = (uint32_t)(elapsed_sec * bucket->rate);
    bucket->tokens = (bucket->tokens + new_tokens > bucket->bucket_size) ?
                     bucket->bucket_size : bucket->tokens + new_tokens;
    bucket->last_update = now;
}

/**
 * @brief Try to consume tokens from bucket
 * @param bucket Token bucket
 * @param tokens Number of tokens to consume
 * @return true if tokens consumed, false otherwise
 */
bool token_bucket_consume(token_bucket_t *bucket, uint32_t tokens) {
    if (bucket == NULL) return false;

    token_bucket_refill(bucket);

    if (bucket->tokens >= tokens) {
        bucket->tokens -= tokens;
        return true;
    }

    return false;
}

/**
 * @brief Get current token count
 * @param bucket Token bucket
 * @return Current tokens
 */
uint32_t token_bucket_get_tokens(token_bucket_t *bucket) {
    if (bucket == NULL) return 0;
    token_bucket_refill(bucket);
    return bucket->tokens;
}

/* ============================================================================
 * Flow Control Test
 * ============================================================================ */

/**
 * @brief Flow control test
 */
void flow_control_test(void) {
    printf("\n=== Flow Control Test ===\n");

    /* Test sliding window */
    printf("\n--- Sliding Window Protocol ---\n");
    sw_transmitter_t sw_tx;
    sw_receiver_t sw_rx;

    sw_init_transmitter(&sw_tx, 4);
    sw_init_receiver(&sw_rx, 4);

    /* Send frames */
    uint8_t data1[] = "Hello";
    uint8_t data2[] = "World";
    uint8_t data3[] = "Sliding";
    uint8_t data4[] = "Window";
    uint8_t data5[] = "Test";  /* This should fail (window full) */

    sw_send(&sw_tx, data1, sizeof(data1));
    sw_send(&sw_tx, data2, sizeof(data2));
    sw_send(&sw_tx, data3, sizeof(data3));
    sw_send(&sw_tx, data4, sizeof(data4));
    sw_send(&sw_tx, data5, sizeof(data5));  /* Window full */

    printf("TX: Window base=%d, next=%d\n", sw_tx.base, sw_tx.next_seq);

    /* Simulate frame reception and ACKs */
    window_frame_t frame;
    uint8_t output[256];
    int delivered;

    /* Process frame 0 */
    frame.seq_num = 0;
    frame.len = sizeof(data1);
    memcpy(frame.data, data1, sizeof(data1));
    delivered = sw_receive(&sw_rx, &frame, output, sizeof(output));
    printf("RX: Delivered %d bytes from frame 0\n", delivered);

    /* Send ACK for frame 1 (cumulative) */
    printf("RX: Sending ACK %d\n", sw_get_ack_num(&sw_rx));
    sw_process_ack(&sw_tx, sw_get_ack_num(&sw_rx));
    printf("TX: After ACK, window base=%d, next=%d\n", sw_tx.base, sw_tx.next_seq);

    /* Process frame 2 (out of order) */
    frame.seq_num = 2;
    frame.len = sizeof(data3);
    memcpy(frame.data, data3, sizeof(data3));
    delivered = sw_receive(&sw_rx, &frame, output, sizeof(output));
    printf("RX: Delivered %d bytes from frame 2 (buffered, out of order)\n", delivered);

    /* Process frame 1 (completes sequence) */
    frame.seq_num = 1;
    frame.len = sizeof(data2);
    memcpy(frame.data, data2, sizeof(data2));
    delivered = sw_receive(&sw_rx, &frame, output, sizeof(output));
    printf("RX: Delivered %d bytes total (frames 1,2 delivered)\n", delivered);

    /* Test Stop-and-Wait */
    printf("\n--- Stop-and-Wait Protocol ---\n");
    saw_transmitter_t saw_tx;
    saw_receiver_t saw_rx;

    saw_init_transmitter(&saw_tx);
    saw_init_receiver(&saw_rx);

    uint8_t saw_data1[] = "S&W Test 1";
    uint8_t saw_data2[] = "S&W Test 2";

    saw_send(&saw_tx, saw_data1, sizeof(saw_data1));
    saw_send(&saw_tx, saw_data2, sizeof(saw_data2));  /* Should fail, waiting for ACK */

    /* Simulate ACK */
    saw_process_ack(&saw_tx, 1);
    saw_send(&saw_tx, saw_data2, sizeof(saw_data2));  /* Now should succeed */

    /* Test token bucket */
    printf("\n--- Token Bucket Rate Control ---\n");
    token_bucket_t bucket;
    token_bucket_init(&bucket, 100, 200);  /* 100 tokens/sec, 200 burst */

    printf("Initial tokens: %u\n", token_bucket_get_tokens(&bucket));

    /* Consume tokens */
    for (int i = 0; i < 5; i++) {
        if (token_bucket_consume(&bucket, 50)) {
            printf("Consumed 50 tokens, remaining: %u\n", bucket.tokens);
        } else {
            printf("Not enough tokens (have %u)\n", bucket.tokens);
        }
    }

    /* Try to consume more than available */
    if (!token_bucket_consume(&bucket, 100)) {
        printf("Correctly rejected consumption (only %u tokens)\n", bucket.tokens);
    }

    printf("\nFlow control test completed\n");
}

/* Byte order helpers */
#ifndef htons
    #define htons(x) ((((x) >> 8) & 0xFF) | (((x) & 0xFF) << 8))
    #define ntohs(x) htons(x)
#endif

/* Main entry point */
int main(void) {
    flow_control_test();
    return 0;
}
