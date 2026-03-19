/*
 * Auto-generated from: 03_System_Technology_Domains\05_Wireless_Protocol\README.md
 * Line: 22
 * Language: c
 * Block ID: c566c4ad
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 无线信道模型与参数计算
 */
#include <math.h>
#include <complex.h>

// 自由空间路径损耗模型
double fspl(double distance_m, double frequency_hz)
{
    const double c = 299792458.0;
    double d_km = distance_m / 1000.0;
    double f_mhz = frequency_hz / 1e6;
    return 20.0 * log10(d_km) + 20.0 * log10(f_mhz) + 32.45;
}

// 接收信号强度计算
double rssi_calc(double tx_power_dbm, double distance_m,
                  double frequency_hz, double extra_loss_db)
{
    double path_loss = fspl(distance_m, frequency_hz);
    return tx_power_dbm - path_loss - extra_loss_db;
}
