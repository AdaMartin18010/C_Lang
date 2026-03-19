/*
 * Auto-generated from: 03_System_Technology_Domains\06_Security_Boot\03_Measured_Boot.md
 * Line: 355
 * Language: c
 * Block ID: 84185709
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// 读取TCG事件日志（Linux特定）
int tpm_read_event_log(uint8_t *log_buffer, size_t *log_size) {
    const char *log_path = "/sys/kernel/security/tpm0/binary_bios_measurements";

    FILE *fp = fopen(log_path, "rb");
    if (!fp) {
        // 尝试替代路径
        log_path = "/sys/kernel/security/integrity/ima/binary_runtime_measurements";
        fp = fopen(log_path, "rb");
        if (!fp) {
            perror("Failed to open event log");
            return -1;
        }
    }

    size_t read = fread(log_buffer, 1, TPM_EVENT_LOG_MAX, fp);
    fclose(fp);

    *log_size = read;
    return 0;
}

// 解析并显示事件日志
void tpm_dump_event_log(const uint8_t *log_buffer, size_t log_size) {
    printf("\n=== TCG Event Log ===\n");
    printf("Total size: %zu bytes\n\n", log_size);

    size_t offset = 0;
    int event_count = 0;

    while (offset < log_size) {
        // 解析TCG PCClient事件（简化版本）
        if (log_size - offset < sizeof(TCGEvent)) {
            break;
        }

        TCGEvent *event = (TCGEvent *)(log_buffer + offset);

        printf("Event %d:\n", event_count++);
        printf("  PCR Index: %u\n", event->pcr_index);
        printf("  Event Type: 0x%08x\n", event->event_type);
        printf("  Digest: ");
        for (int i = 0; i < SHA256_DIGEST_SIZE; i++) {
            printf("%02x", event->digest[i]);
        }
        printf("\n");

        if (event->event_data_size > 0 && event->event_data_size < 256) {
            printf("  Event Data (%u bytes): ", event->event_data_size);
            for (uint32_t i = 0; i < event->event_data_size && i < 32; i++) {
                printf("%02x ", event->event_data[i]);
            }
            printf("...\n");
        }
        printf("\n");

        offset += sizeof(TCGEvent) + event->event_data_size;
    }

    printf("Total events: %d\n", event_count);
}
