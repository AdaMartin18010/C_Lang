/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\CISA_2026_Mandate_Depth.md
 * Line: 1148
 * Language: c
 * Block ID: 87c57d12
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*@ requires \valid_read(arr + (0..len-1));
    requires len > 0;
    requires idx < len;
    assigns \nothing;
    ensures 0 <= \result;
*/
int safe_sum_positive(const int *arr, size_t len, size_t idx) {
    int sum = 0;
    /*@ loop invariant 0 <= i <= len;
        loop invariant sum >= 0;
        loop assigns i, sum;
    */
    for (size_t i = 0; i < len; i++) {
        if (arr[i] > 0) {
            sum += arr[i];
        }
    }
    return sum;
}
