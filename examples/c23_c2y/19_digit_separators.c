/*
 * C23 数字分隔符示例
 * 编译: gcc -std=c23 -o 19_digit_separators 19_digit_separators.c
 *       clang -std=c23 -o 19_digit_separators 19_digit_separators.c
 */

#include <stdio.h>
#include <stdint.h>
#include <limits.h>

int main(void) {
    printf("C23 Digit Separators Demo\n");
    printf("=========================\n\n");
    
    // 十进制数字分隔符
    printf("Decimal with separators:\n");
    int one_million = 1'000'000;
    int one_billion = 1'000'000'000;
    long long big_number = 9'223'372'036'854'775'807LL;  // LLONG_MAX
    
    printf("1'000'000 = %d\n", one_million);
    printf("1'000'000'000 = %d\n", one_billion);
    printf("9'223'372'036'854'775'807 = %lld\n", (long long)big_number);
    
    // 十六进制数字分隔符
    printf("\nHexadecimal with separators:\n");
    int color = 0xFF'AA'00;  // RGB颜色
    uint32_t mask = 0xFFFF'0000;
    uint64_t large_hex = 0x1234'5678'9ABC'DEF0ULL;
    
    printf("0xFF'AA'00 = 0x%06X (color)\n", color);
    printf("0xFFFF'0000 = 0x%08X (mask)\n", mask);
    printf("0x1234'5678'9ABC'DEF0 = 0x%016llX\n", (unsigned long long)large_hex);
    
    // 二进制数字分隔符
    printf("\nBinary with separators:\n");
    int flags = 0b1111'0000'1010'1010;
    int permissions = 0b000'111'101;  // rwxrwxrwx
    
    printf("0b1111'0000'1010'1010 = %d (0x%04X)\n", flags, flags);
    printf("0b000'111'101 = %d (permissions)\n", permissions);
    
    // 八进制数字分隔符
    printf("\nOctal with separators:\n");
    int file_mode = 0000'755;  // Unix文件权限
    int special = 0777'000;
    
    printf("0000'755 = 0%o (file mode)\n", file_mode);
    printf("0777'000 = 0%o\n", special);
    
    // 浮点数分隔符
    printf("\nFloating point with separators:\n");
    double pi = 3.141'592'653'589'793;
    double avogadro = 6.022'140'76e23;
    double small = 1.602'176'634e-19;
    
    printf("3.141'592'653'589'793 = %.15f\n", pi);
    printf("6.022'140'76e23 = %e\n", avogadro);
    printf("1.602'176'634e-19 = %e\n", small);
    
    // 常量定义示例
    printf("\n--- Useful Constants ---\n");
    
    // 时间相关
    const int seconds_per_hour = 3'600;
    const int seconds_per_day = 86'400;
    const long long seconds_per_year = 31'536'000LL;
    
    printf("Seconds per hour: %d\n", seconds_per_hour);
    printf("Seconds per day: %d\n", seconds_per_day);
    printf("Seconds per year: %lld\n", (long long)seconds_per_year);
    
    // 存储相关
    const size_t kb = 1'024;
    const size_t mb = 1'048'576;
    const size_t gb = 1'073'741'824;
    
    printf("\n1 KB = %zu bytes\n", kb);
    printf("1 MB = %zu bytes\n", mb);
    printf("1 GB = %zu bytes\n", gb);
    
    // 网络相关
    const int http_port = 80;
    const int https_port = 443;
    const int common_ports[] = {80, 443, 8080, 8'008, 30'000};
    
    printf("\nCommon ports: ");
    for (size_t i = 0; i < sizeof(common_ports)/sizeof(common_ports[0]); i++) {
        printf("%d ", common_ports[i]);
    }
    printf("\n");
    
    // 金融/货币
    const long cents_per_dollar = 100;
    const long cents_per_k = 100'000;
    const long cents_per_million = 100'000'000;
    
    printf("\nCurrency (in cents):\n");
    printf("$1 = %ld cents\n", cents_per_dollar);
    printf("$1,000 = %ld cents\n", cents_per_k);
    printf("$1,000,000 = %ld cents\n", cents_per_million);
    
    // 科学常数
    const double speed_of_light = 299'792'458.0;  // m/s
    const double planck_constant = 6.626'070'15e-34;  // J⋅s
    
    printf("\nScientific constants:\n");
    printf("Speed of light: %.0f m/s\n", speed_of_light);
    printf("Planck constant: %.9e J⋅s\n", planck_constant);
    
    printf("\nDigit separators demo complete!\n");
    
    return 0;
}
