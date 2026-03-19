/*
 * Auto-generated from: 04_Industrial_Scenarios\04_5G_Baseband\03_MIMO_Detection.md
 * Line: 78
 * Language: c
 * Block ID: 3f42b52d
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

// ZF检测: x_hat = (H^H * H)^-1 * H^H * y
void zf_detect(cfloat* H, cfloat* y, int Nr, int Nt, cfloat* x_hat) {
    cfloat HH_H[Nt][Nt];

    // H^H * H
    for (int i = 0; i < Nt; i++) {
        for (int j = 0; j < Nt; j++) {
            HH_H[i][j] = 0;
            for (int k = 0; k < Nr; k++) {
                HH_H[i][j] += conjf(H[k*Nt + i]) * H[k*Nt + j];
            }
        }
    }

    // 矩阵求逆
    cfloat inv_HH_H[Nt][Nt];
    matrix_inverse((cfloat*)HH_H, (cfloat*)inv_HH_H, Nt);

    // x_hat = inv(H^H*H) * H^H * y
    cfloat HH_y[Nt];
    for (int i = 0; i < Nt; i++) {
        HH_y[i] = 0;
        for (int k = 0; k < Nr; k++) {
            HH_y[i] += conjf(H[k*Nt + i]) * y[k];
        }
    }

    for (int i = 0; i < Nt; i++) {
        x_hat[i] = 0;
        for (int j = 0; j < Nt; j++) {
            x_hat[i] += inv_HH_H[i][j] * HH_y[j];
        }
    }
}
