/*
 * API 版本控制与 ABI 兼容示例
 * 编译: gcc -Wall -Wextra -o api_versioning api_versioning.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LIB_MAJOR 1
#define LIB_MINOR 2
#define LIB_PATCH 0

/* 版本信息 */
void lib_get_version(int *major, int *minor, int *patch) {
    if (major) *major = LIB_MAJOR;
    if (minor) *minor = LIB_MINOR;
    if (patch) *patch = LIB_PATCH;
}

/* V1 API */
typedef struct {
    int x;
    int y;
} PointV1;

double point_dist_v1(const PointV1 *a, const PointV1 *b) {
    int dx = a->x - b->x;
    int dy = a->y - b->y;
    return dx * dx + dy * dy;
}

/* V2 API - 扩展但不破坏 ABI */
typedef struct {
    int x;
    int y;
    int z;  /* 新增，但旧代码忽略它 */
} PointV2;

double point_dist_v2(const PointV2 *a, const PointV2 *b) {
    int dx = a->x - b->x;
    int dy = a->y - b->y;
    int dz = a->z - b->z;
    return dx*dx + dy*dy + dz*dz;
}

/* 兼容层：V1 调用 V2 */
double point_dist_compat(const void *a, const void *b) {
    return point_dist_v1((const PointV1 *)a, (const PointV1 *)b);
}

/* 特性检测 */
typedef enum {
    FEATURE_DIST_2D = 1,
    FEATURE_DIST_3D = 2,
} Feature;

int lib_has_feature(Feature f) {
    switch (f) {
        case FEATURE_DIST_2D: return 1;
        case FEATURE_DIST_3D: return LIB_MINOR >= 2;
        default: return 0;
    }
}

int main(void) {
    int ma, mi, pa;
    lib_get_version(&ma, &mi, &pa);
    printf("Library version: %d.%d.%d\n", ma, mi, pa);
    
    PointV1 p1 = {3, 4};
    PointV1 p2 = {0, 0};
    printf("2D distance squared: %.0f\n", point_dist_v1(&p1, &p2));
    
    if (lib_has_feature(FEATURE_DIST_3D)) {
        PointV2 p3 = {1, 2, 3};
        PointV2 p4 = {4, 5, 6};
        printf("3D distance squared: %.0f\n", point_dist_v2(&p3, &p4));
    }
    
    return 0;
}
