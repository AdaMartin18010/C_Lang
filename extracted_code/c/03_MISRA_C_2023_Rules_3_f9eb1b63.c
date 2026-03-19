/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\03_MISRA_C_2023_Rules_3.md
 * Line: 468
 * Language: c
 * Block ID: f9eb1b63
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 文件头注释
 * ==========
 * 每个文件应该有文件头注释
 */

/**
 * @file example.c
 * @brief Brief description of this file
 * @copyright Copyright (c) 2024
 */

/*
 * 节注释（分隔不同功能区域）
 */
/*===========================================================================*/
/* Private constants                                                         */
/*===========================================================================*/

static const int MAX_BUFFER_SIZE = 1024;

/*===========================================================================*/
/* Public functions                                                          */
/*===========================================================================*/

/**
 * @brief Process input data
 *
 * Detailed description of what this function does,
 * its algorithm, and any important details.
 *
 * @param[in] input Input data buffer
 * @param[in] len Length of input data
 * @param[out] output Output buffer (must be at least len bytes)
 * @return Number of bytes written to output, or -1 on error
 *
 * @note This function modifies internal state
 * @warning Output buffer must not overlap with input
 */
int process_data(const uint8_t *input, size_t len, uint8_t *output);

/*
 * 行内注释
 */
int calculate(int x, int y)
{
    /* Validate inputs */
    if (x < 0 || y < 0) {
        return -1;  /* Invalid parameter */
    }

    /* Algorithm: use Kahan summation for precision */
    double sum = 0.0;
    double c = 0.0;  /* Compensation for lost low-order bits */

    /* ... */

    return (int)sum;
}
