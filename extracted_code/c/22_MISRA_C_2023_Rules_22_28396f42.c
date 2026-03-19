/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\22_MISRA_C_2023_Rules_22.md
 * Line: 285
 * Language: c
 * Block ID: 28396f42
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* ❌ 复杂函数（复杂度高）*/
void complex_func(int a, int b, int c)
{
    if (a) {
        if (b) {
            if (c) {
                /* ... */
            } else {
                /* ... */
            }
        } else {
            /* ... */
        }
    } else {
        /* ... */
    }
}

/* ✅ 拆分为小函数 */
static void handle_a_b_c(void) { /* ... */ }
static void handle_a_b_notc(void) { /* ... */ }
static void handle_a_notb(void) { /* ... */ }
static void handle_nota(void) { /* ... */ }

void simple_func(int a, int b, int c)
{
    if (!a) {
        handle_nota();
        return;
    }

    if (!b) {
        handle_a_notb();
        return;
    }

    if (c) {
        handle_a_b_c();
    } else {
        handle_a_b_notc();
    }
}
