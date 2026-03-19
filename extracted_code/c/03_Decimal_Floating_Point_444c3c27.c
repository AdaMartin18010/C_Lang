/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\IEEE_754_Floating_Point\03_Decimal_Floating_Point.md
 * Line: 3010
 * Language: c
 * Block ID: 444c3c27
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 编译: gcc -std=c23 migration_guide.c -o migration_guide
 *
 * 从其他系统迁移到 C 十进制浮点
 */

#include <stdio.h>

void cobol_migration(void) {
    printf("=== COBOL 到 C _Decimal 迁移指南 ===\n\n");

    printf("1. 数据类型映射:\n\n");

    printf("   COBOL                    C\n");
    printf("   ------------------------ -----------------------------\n");
    printf("   PIC 9(4)                 int / short\n");
    printf("   PIC 9(9)                 int / long\n");
    printf("   PIC 9(18)                long long\n");
    printf("   PIC 9(6)V99 COMP-3       _Decimal64 (限制精度)\n");
    printf("   PIC 9(16)V99 COMP-3      _Decimal64\n");
    printf("   PIC 9(33)V99 COMP-3      _Decimal128\n\n");

    printf("2. 计算语句转换:\n\n");

    printf("   COBOL:                    C:\n");
    printf("   COMPUTE A = B + C.   =>   _Decimal64 a = b + c;\n");
    printf("   COMPUTE A = B - C.   =>   _Decimal64 a = b - c;\n");
    printf("   COMPUTE A = B * C.   =>   _Decimal64 a = b * c;\n");
    printf("   COMPUTE A = B / C.   =>   _Decimal64 a = b / c;\n");
    printf("   ROUNDED                =>   自定义舍入函数\n\n");

    printf("3. 舍入处理:\n\n");

    printf("   /* COBOL: COMPUTE TOTAL ROUNDED = ... */\n");
    printf("   _Decimal64 calculate_rounded(_Decimal64 value, int decimals) {\n");
    printf("       _Decimal64 factor = 1.0DD;\n");
    printf("       for (int i = 0; i < decimals; i++) factor *= 10.0DD;\n");
    printf("       return round(value * factor) / factor;\n");
    printf("   }\n");
}

void java_migration(void) {
    printf("\n=== Java BigDecimal 到 C _Decimal 迁移指南 ===\n\n");

    printf("1. 构造方法转换:\n\n");

    printf("   Java:                                     C:\n");
    printf("   new BigDecimal(\"123.45\")          =>    123.45DD\n");
    printf("   new BigDecimal(123.45)             =>    避免! 使用字面量\n");
    printf("   BigDecimal.valueOf(123.45)         =>    (_Decimal64)123.45DD\n\n");

    printf("2. 算术方法转换:\n\n");

    printf("   Java:                          C:\n");
    printf("   a.add(b)                 =>    a + b\n");
    printf("   a.subtract(b)            =>    a - b\n");
    printf("   a.multiply(b)            =>    a * b\n");
    printf("   a.divide(b, scale, mode) =>    需要自定义除法函数\n");
    printf("   a.compareTo(b)           =>    (a > b) - (a < b)\n\n");

    printf("3. 精度控制:\n\n");

    printf("   Java:\n");
    printf("   result = value.setScale(2, RoundingMode.HALF_UP);\n\n");

    printf("   C:\n");
    printf("   _Decimal64 set_scale(_Decimal64 value, int scale) {\n");
    printf("       _Decimal64 factor = pow(10, scale);\n");
    printf("       value = value * factor + 0.5DD;\n");
    printf("       value = (_Decimal64)(long long)value;\n");
    printf("       return value / factor;\n");
    printf("   }\n");
}

void best_practices_summary(void) {
    printf("\n=== 迁移最佳实践 ===\n\n");

    printf("1. 逐步迁移:\n");
    printf("   - 先在非关键模块使用 _Decimal64\n");
    printf("   - 建立完整的测试用例\n");
    printf("   - 对比旧系统和新系统的输出\n\n");

    printf("2. 舍入一致性:\n");
    printf("   - 明确文档化舍入策略\n");
    printf("   - 确保与旧系统使用相同的舍入模式\n");
    printf("   - 考虑监管要求的舍入规则\n\n");

    printf("3. 精度检查:\n");
    printf("   - 验证 _Decimal64 (16位) 足够业务需求\n");
    printf("   - 如需更高精度，使用 _Decimal128\n");
    printf("   - 注意中间计算可能产生的精度丢失\n\n");

    printf("4. 性能优化:\n");
    printf("   - 利用硬件加速 (现代 Intel/AMD CPU)\n");
    printf("   - 避免频繁的十进制/二进制转换\n");
    printf("   - 批量处理以减少函数调用开销\n\n");

    printf("5. 测试策略:\n");
    printf("   - 包含边界值测试\n");
    printf("   - 验证舍入边界 (如 x.5)\n");
    printf("   - 大规模数据对比测试\n");
    printf("   - 长期累积误差测试\n");
}

int main(void) {
    cobol_migration();
    java_migration();
    best_practices_summary();

    return 0;
}
