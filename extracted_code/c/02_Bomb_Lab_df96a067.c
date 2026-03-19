/*
 * Auto-generated from: 04_Industrial_Scenarios\17_CMU_15_213_Labs\02_Bomb_Lab.md
 * Line: 402
 * Language: c
 * Block ID: df96a067
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

int func4(int target, int low, int high) {
    int mid = (low + high) / 2;
    if (mid > target)
        return 2 * func4(target, low, mid - 1);
    if (mid < target)
        return 2 * func4(target, mid + 1, high) + 1;
    return 0;  // found
}
