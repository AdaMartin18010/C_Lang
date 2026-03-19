/*
 * Auto-generated from: 01_Core_Knowledge_System\01_Basic_Layer\IEEE_754_Floating_Point\03_Decimal_Floating_Point.md
 * Line: 2837
 * Language: c
 * Block ID: 3c7c88ed
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/*
 * 编译: gcc -std=c23 comparison_demo.c -o comparison_demo
 *
 * 与其他语言/系统的十进制计算对比
 */

#include <stdio.h>

/* 技术规格对比表 */
void print_technical_comparison(void) {
    printf("=== 十进制计算技术对比 ===\n\n");

    printf("| 特性 | C _Decimal64 | Java BigDecimal | COBOL COMP-3 |
");
    printf("|------|--------------|-----------------|--------------|
");
    printf("| 存储大小 | 8字节 | 可变 (最小 ~32字节) | 可变 |
");
    printf("| 精度 | 16位十进制 | 用户指定 (无限) | 用户指定 |
");
    printf("| 范围 | 10^(-383)~10^384 | 无限 | 取决于PIC |
");
    printf("| 速度 | 硬件加速 | 软件实现 (慢) | 软件 |
");
    printf("| 内存占用 | 低 | 高 | 中 |
");
    printf("| 精度控制 | 固定 | 完全可控 | 声明式 |
");
    printf("| 舍入模式 | 5种 | 8种 | 有限支持 |
");
    printf("| 硬件支持 | Intel/AMD | 无 | 无 |
");
    printf("| 标准 | IEEE 754-2008 | Java 标准 | COBOL 标准 |
");
    printf("\n");
}

/* COBOL 风格计算演示 */
void cobol_style_example(void) {
    printf("=== COBOL 风格计算 (C实现) ===\n\n");

    printf("COBOL 代码示例:\n");
    printf("  01  PRICE      PIC 9(6)V99 COMP-3.    /* 6位整数 + 2位小数 */\n");
    printf("  01  QUANTITY   PIC 9(4) COMP-3.\n");
    printf("  01  TOTAL      PIC 9(8)V99 COMP-3.\n");
    printf("  COMPUTE TOTAL = PRICE * QUANTITY.\n\n");

    printf("C _Decimal64 等价实现:\n");
    _Decimal64 price = 1234.56DD;    /* 对应 PIC 9(4)V99 */
    _Decimal64 quantity = 100.0DD;    /* 对应 PIC 9(3) */
    _Decimal64 total = price * quantity;

    /* COBOL 风格的舍入 */
    total = total + 0.005DD;  /* 四舍五入到分 */
    total = (_Decimal64)((long long)(total * 100.0DD)) / 100.0DD;

    printf("  _Decimal64 price = 1234.56DD;\n");
    printf("  _Decimal64 quantity = 100.0DD;\n");
    printf("  _Decimal64 total = price * quantity;\n");
    printf("  结果: %.2f\n\n", (double)total);
}

/* Java BigDecimal 风格计算演示 */
void java_style_example(void) {
    printf("=== Java BigDecimal 风格计算 (C实现) ===\n\n");

    printf("Java 代码示例:\n");
    printf("  BigDecimal price = new BigDecimal(\"19.99\");\n");
    printf("  BigDecimal taxRate = new BigDecimal(\"0.0875\");\n");
    printf("  BigDecimal tax = price.multiply(taxRate);\n");
    printf("  tax = tax.setScale(2, RoundingMode.HALF_UP);\n");
    printf("  BigDecimal total = price.add(tax);\n\n");

    printf("C _Decimal64 等价实现:\n");
    _Decimal64 price = 19.99DD;
    _Decimal64 tax_rate = 0.0875DD;

    /* 计算税费 */
    _Decimal64 tax = price * tax_rate;

    /* 手动实现 HALF_UP 舍入到2位小数 */
    tax = tax * 100.0DD;
    tax = tax + (tax >= 0 ? 0.5DD : -0.5DD);
    tax = (_Decimal64)((long long)tax);
    tax = tax / 100.0DD;

    _Decimal64 total = price + tax;

    printf("  _Decimal64 price = 19.99DD;\n");
    printf("  _Decimal64 tax_rate = 0.0875DD;\n");
    printf("  _Decimal64 tax = price * tax_rate;\n");
    printf("  /* 舍入到2位小数 */\n");
    printf("  _Decimal64 total = price + tax;\n");
    printf("  价格: $%.2f\n", (double)price);
    printf("  税费: $%.2f\n", (double)tax);
    printf("  总计: $%.2f\n\n", (double)total);
}

/* 性能对比概念 */
void performance_comparison(void) {
    printf("=== 性能对比 (概念性) ===\n\n");

    printf("相对执行时间 (以 C _Decimal64 = 1 为基准):\n\n");

    printf("操作类型 | C _Decimal64 | Java BigDecimal | COBOL\n");
    printf("---------|--------------|-----------------|-------\n");
    printf("加法     | 1x           | 50-100x         | 10-20x\n");
    printf("乘法     | 1x           | 100-200x        | 20-40x\n");
    printf("除法     | 1x           | 200-500x        | 50-100x\n");
    printf("内存分配 | 无           | 每次操作        | 预分配\n\n");

    printf("说明:\n");
    printf("  - C _Decimal64 使用硬件加速 (Intel/AMD 处理器)\n");
    printf("  - Java BigDecimal 是软件实现，涉及对象创建\n");
    printf("  - COBOL 编译器通常有优化，但仍是软件实现\n\n");
}

/* 互操作性考虑 */
void interoperability_notes(void) {
    printf("=== 互操作性考虑 ===\n\n");

    printf("与 Java BigDecimal 互操作:\n");
    printf("  1. 使用字符串作为交换格式\n");
    printf("  2. 确保精度信息传递\n");
    printf("  3. 注意舍入模式一致性\n");
    printf("  4. JSON/XML 中作为字符串传输\n\n");

    printf("与 COBOL 互操作:\n");
    printf("  1. COBOL COMP-3 是 packed decimal 格式\n");
    printf("  2. 需要字节级转换\n");
    printf("  3. 注意字节序和符号位\n");
    printf("  4. 精度声明要匹配\n\n");

    printf("与数据库 DECIMAL 类型:\n");
    printf("  1. MySQL/PostgreSQL DECIMAL 映射到 _Decimal64\n");
    printf("  2. Oracle NUMBER 类型需要精度检查\n");
    printf("  3. SQL Server decimal 直接兼容\n");
    printf("  4. 使用参数化查询避免字符串转换\n");
}

int main(void) {
    print_technical_comparison();
    cobol_style_example();
    java_style_example();
    performance_comparison();
    interoperability_notes();

    printf("\n=== 选择建议 ===\n\n");

    printf("使用 C _Decimal64 的场景:\n");
    printf("  ✓ 高性能财务计算\n");
    printf("  ✓ 嵌入式/实时系统\n");
    printf("  ✓ 需要与硬件紧密集成\n");
    printf("  ✓ 内存受限环境\n");
    printf("  ✓ 替代 COBOL 的现代方案\n\n");

    printf("使用 Java BigDecimal 的场景:\n");
    printf("  ✓ 需要超过16位精度\n");
    printf("  ✓ Java 生态系统\n");
    printf("  ✓ 需要精确的精度控制\n\n");

    printf("使用 COBOL 的场景:\n");
    printf("  ✓ 遗留系统维护\n");
    printf("  ✓ 严格的监管合规要求\n");
    printf("  ✓ 大型机环境\n");

    return 0;
}
