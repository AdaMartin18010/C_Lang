/*
 * Auto-generated from: 01_Core_Knowledge_System\09_Safety_Standards\MISRA_C_2023\tools\01_MISRA_Toolchain_Guide.md
 * Line: 939
 * Language: c
 * Block ID: 2d3eb56c
 * 
 * This file was automatically extracted from the C_Lang knowledge base.
 * DO NOT MODIFY - Changes will be overwritten on next extraction.
 */

/* =============================================================================
 * 常见MISRA C:2023违规及修复示例
 * ============================================================================= */

/* ---------------------------------------------------------------------------
 * 违规类型: Rule 9.1 - 使用未初始化变量
 * --------------------------------------------------------------------------- */

/* 违规代码 */
void bad_example(void)
{
    int x;          /* 未初始化 */
    if (condition) {
        x = 10;
    }
    use(x);         /* 可能使用未初始化的x */
}

/* 修复方案1: 始终初始化 */
void good_example_1(void)
{
    int x = 0;      /* 默认初始化 */
    if (condition) {
        x = 10;
    }
    use(x);
}

/* 修复方案2: 确保所有路径都初始化 */
void good_example_2(void)
{
    int x;
    if (condition) {
        x = 10;
    } else {
        x = 0;      /* else分支初始化 */
    }
    use(x);
}

/* 修复方案3: 重构代码 */
void good_example_3(void)
{
    if (condition) {
        int x = 10;
        use(x);
    } else {
        /* 不使用x */
    }
}

/* ---------------------------------------------------------------------------
 * 违规类型: Rule 15.6 - 循环体不是复合语句
 * --------------------------------------------------------------------------- */

/* 违规代码 */
if (condition)
    do_something();     /* 单行语句，没有大括号 */

/* 修复代码 */
if (condition)          /* 始终使用大括号 */
{
    do_something();
}

/* ---------------------------------------------------------------------------
 * 违规类型: Rule 10.1 - 不当的运算符使用
 * --------------------------------------------------------------------------- */

/* 违规代码 */
uint8_t u8 = 10U;
int8_t s8 = -5;
bool result = (u8 > s8);    /* 有符号与无符号比较 */

/* 修复代码 */
uint8_t u8 = 10U;
int16_t s16 = -5;           /* 提升为更大的类型 */
bool result = ((int16_t)u8 > s16);

/* 或 */
uint8_t u8 = 10U;
uint8_t s8_as_u8 = (uint8_t)(-5);  /* 如果确定范围安全 */
/* 但需注意：负数转换为无符号会产生大正数 */

/* ---------------------------------------------------------------------------
 * 违规类型: Rule 11.3 - 不兼容指针类型转换
 * --------------------------------------------------------------------------- */

/* 违规代码 */
void process(void)
{
    uint32_t data;
    uint8_t *ptr = (uint8_t *)&data;    /* 可能违反对齐 */
    *ptr = 0xFF;
}

/* 修复代码 */
void process_fixed(void)
{
    union {                             /* 使用union确保安全 */
        uint32_t data;
        uint8_t bytes[4];
    } converter;

    converter.bytes[0] = 0xFF;
}

/* 或 */
void process_fixed_2(void)
{
    uint32_t data;
    /* 使用memcpy确保类型安全 */
    uint8_t value = 0xFF;
    (void)memcpy(&data, &value, sizeof(value));
}

/* ---------------------------------------------------------------------------
 * 违规类型: Rule 16.3 - 无意的switch fallthrough
 * --------------------------------------------------------------------------- */

/* 违规代码 */
switch (state) {
    case STATE_A:
        do_a();         /* 可能忘记break */
    case STATE_B:
        do_b();
        break;
}

/* 修复方案1: 添加break */
switch (state) {
    case STATE_A:
        do_a();
        break;          /* 明确添加break */
    case STATE_B:
        do_b();
        break;
}

/* 修复方案2: 如果是故意fallthrough，添加注释 */
switch (state) {
    case STATE_A:
        do_a();
        /* Fall-through: 执行STATE_A后必须执行STATE_B */
        /*lint -fallthrough */
    case STATE_B:
        do_b();
        break;
}

/* ---------------------------------------------------------------------------
 * 违规类型: Rule 17.7 - 忽略函数返回值
 * --------------------------------------------------------------------------- */

/* 违规代码 */
strcpy(dest, src);      /* 忽略返回值 */

/* 修复方案1: 检查返回值 */
char *result = strcpy(dest, src);
if (result != dest) {
    /* 处理错误 */
}

/* 修复方案2: 使用更安全的函数 */
errno_t err = strcpy_s(dest, sizeof(dest), src);
if (err != 0) {
    /* 处理错误 */
}

/* 修复方案3: 显式转换void表示故意忽略（如果安全） */
(void)strcpy(dest, src);    /* 仅当确定不会溢出时 */

/* ---------------------------------------------------------------------------
 * 违规类型: Rule 2.7 - 未使用参数
 * --------------------------------------------------------------------------- */

/* 违规代码 */
void callback(void *param, int unused)    /* unused参数未被使用 */
{
    use(param);
}

/* 修复方案1: 移除参数名 */
void callback(void *param, int)           /* 仅保留类型 */
{
    use(param);
}

/* 修复方案2: 显式标记为未使用 */
void callback(void *param, int unused)
{
    (void)unused;       /* 显式转换为void */
    use(param);
}

/* 修复方案3: 使用宏 */
#define UNUSED_PARAM(x) ((void)(x))

void callback(void *param, int unused)
{
    UNUSED_PARAM(unused);
    use(param);
}

/* ---------------------------------------------------------------------------
 * 违规类型: Rule 11.4 - 指针与整数转换
 * --------------------------------------------------------------------------- */

/* 违规代码 */
uint32_t addr = 0x40000000U;
uint32_t *ptr = (uint32_t *)addr;    /* 整数转指针 */

/* 修复代码 */
#define REG_BASE_ADDR ((uintptr_t)0x40000000U)

void *reg_base = (void *)REG_BASE_ADDR;
volatile uint32_t *ptr = (volatile uint32_t *)reg_base;

/* 或使用内存映射结构体 */
typedef struct {
    volatile uint32_t CTRL;
    volatile uint32_t STATUS;
    /* ... */
} PERIPH_Type;

#define PERIPH_BASE ((PERIPH_Type *)0x40000000U)

void access_periph(void)
{
    PERIPH_BASE->CTRL = 0x1U;
}
