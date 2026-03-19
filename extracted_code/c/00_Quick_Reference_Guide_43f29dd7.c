/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\00_Quick_Reference_Guide.md
 * Line: 149
 * Language: c
 * Block ID: 43f29dd7
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

typedef struct {
    int32_t value_a;
    int32_t value_b;
    bool valid;
} DualChannelResult;

int32_t vote_dual_channel(const DualChannelResult *dc, int32_t *output)
{
    const int32_t TOLERANCE = 10;

    if (!dc->valid) {
        return ERROR_INVALID;
    }

    int32_t diff = (dc->value_a > dc->value_b) ?
                   (dc->value_a - dc->value_b) :
                   (dc->value_b - dc->value_a);

    if (diff > TOLERANCE) {
        /* 通道不一致 */
        return ERROR_MISMATCH;
    }

    /* 输出较保守值 */
    *output = (dc->value_a < dc->value_b) ? dc->value_a : dc->value_b;
    return SUCCESS;
}
