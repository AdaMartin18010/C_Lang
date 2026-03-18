# Lab 1: Data Lab - 数据表示实验

## 1. 实验目标

### 1.1 总体目标

Data Lab旨在加深对整数和浮点数数据表示的理解，通过实现一系列位操作puzzle来掌握底层数据操作。

| 目标 | 描述 |
|------|------|
| **位操作** | 掌握C语言位级操作技巧 |
| **整数运算** | 理解补码表示和整数运算 |
| **浮点运算** | 理解IEEE 754浮点标准 |
| **约束编程** | 在限制条件下解决问题 |

### 1.2 实验要求

- 只能使用有限的C语言操作符
- 不能使用控制结构（if, while, for等）
- 不能使用宏、函数调用
- 只能使用int类型常量（0x0到0xFF）

### 1.3 评分标准

| 部分 | Puzzle数量 | 分值 |
|------|-----------|------|
| 位操作 | 8 | 36分 |
| 补码运算 | 4 | 24分 |
| 浮点运算 | 4 | 28分 |
| 总分 | 16 | 88分 |

---

## 2. 背景知识

### 2.1 位操作基础

```c
// 基本位操作符
&   // 按位与
|   // 按位或
^   // 按位异或
~   // 按位取反
<<  // 左移
>>  // 右移（算术右移对有符号数）

// 常用位操作技巧
// 1. 设置第n位：x | (1 << n)
// 2. 清除第n位：x & ~(1 << n)
// 3. 翻转第n位：x ^ (1 << n)
// 4. 检查第n位：(x >> n) & 1
```

### 2.2 整数表示

```c
// 补码表示（32位）
// 正数：原码
// 负数：反码 + 1

// 特殊值
INT_MIN = 0x80000000  // -2147483648
INT_MAX = 0x7FFFFFFF  // 2147483647

// 符号扩展
// 8位扩展到32位
int sign_extend(char x) {
    return (int)x;  // 自动符号扩展
}
```

### 2.3 IEEE 754浮点标准

```c
// 单精度浮点数（32位）
// | 符号(1) | 阶码(8) | 尾数(23) |

// 特殊值
// 0:      0x00000000
// Inf:    0x7F800000 (正无穷)
// -Inf:   0xFF800000 (负无穷)
// NaN:    0x7FC00000 (非数字)

// 规格化数计算公式
// value = (-1)^s * 1.mantissa * 2^(exponent-127)
```

---

## 3. Puzzle完整解答

### 3.1 位操作 Puzzles

#### Puzzle 1: bitAnd

**目标**: 只用 ~ 和 | 实现 x & y

```c
/*
 * bitAnd - x & y using only ~ and |
 *   Example: bitAnd(6, 5) = 4
 *   Legal ops: ~ |
 *   Max ops: 8
 *   Rating: 1
 */
int bitAnd(int x, int y) {
    return ~(~x | ~y);
}
```

**解题思路**: 使用德摩根定律

- x & y = ~(~x | ~y)
- 这是德摩根定律的直接应用

---

#### Puzzle 2: getByte

**目标**: 提取第n个字节（从0开始，小端序）

```c
/*
 * getByte - Extract byte n from word x
 *   Bytes numbered from 0 (LSB) to 3 (MSB)
 *   Examples: getByte(0x12345678,1) = 0x56
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */
int getByte(int x, int n) {
    return (x >> (n << 3)) & 0xFF;
}
```

**解题思路**:

1. n << 3 将字节索引转换为位偏移（*8）
2. x >> 将目标字节移动到最低位
3. & 0xFF 掩码提取低8位

---

#### Puzzle 3: logicalShift

**目标**: 实现逻辑右移（补0而非补符号位）

```c
/*
 * logicalShift - shift x to the right by n, using a logical shift
 *   Can assume that 0 <= n <= 31
 *   Examples: logicalShift(0x87654321,4) = 0x08765432
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 20
 *   Rating: 3
 */
int logicalShift(int x, int n) {
    return (x >> n) & (~(((1 << 31) >> n) << 1));
}
```

**解题思路**:

1. 先进行算术右移
2. 创建掩码清除高位的符号扩展
3. 掩码构造：((1 << 31) >> n) << 1 生成n个1，再取反得到n个0

**简化版本**:

```c
int logicalShift(int x, int n) {
    int mask = ~(((0x1 << 31) >> n) << 1);
    return (x >> n) & mask;
}
```

---

#### Puzzle 4: bitCount

**目标**: 计算1的个数（汉明重量）

```c
/*
 * bitCount - returns count of number of 1's in word
 *   Examples: bitCount(5) = 2, bitCount(7) = 3
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 40
 *   Rating: 4
 */
int bitCount(int x) {
    int mask1 = 0x55 | (0x55 << 8);
    int mask2 = 0x33 | (0x33 << 8);
    int mask3 = 0x0F | (0x0F << 8);
    int mask4 = 0xFF | (0xFF << 8);
    int mask5 = 0xFF | (0xFF << 16);

    mask1 = mask1 | (mask1 << 16);  // 0x55555555
    mask2 = mask2 | (mask2 << 16);  // 0x33333333
    mask3 = mask3 | (mask3 << 16);  // 0x0F0F0F0F
    mask4 = mask4 | (mask4 << 16);  // 0x00FF00FF

    x = (x & mask1) + ((x >> 1) & mask1);
    x = (x & mask2) + ((x >> 2) & mask2);
    x = (x & mask3) + ((x >> 4) & mask3);
    x = (x & mask4) + ((x >> 8) & mask4);
    x = (x & mask5) + ((x >> 16) & mask5);

    return x;
}
```

**解题思路**: 分治并行计数

1. 每2位一组，计算1的个数
2. 每4位一组，相加
3. 每8位一组，相加
4. 每16位一组，相加
5. 最后32位相加得到结果

---

#### Puzzle 5: bang

**目标**: 实现 !x 而不使用 ! 操作符

```c
/*
 * bang - Compute !x without using !
 *   Examples: bang(3) = 0, bang(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4
 */
int bang(int x) {
    return ((x | (~x + 1)) >> 31) + 1;
}
```

**解题思路**:

1. ~x + 1 是 -x（补码取负）
2. 对于非零x，x和-x符号位必有一个为1
3. 对于x=0，符号位都是0
4. 右移31位后，非零为-1(0xFFFFFFFF)，零为0
5. +1后，非零为0，零为1

---

#### Puzzle 6: tmin

**目标**: 返回最小int值

```c
/*
 * tmin - return minimum two's complement integer
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) {
    return 1 << 31;
}
```

**解题思路**:

- 最小int是 0x80000000
- 即 1 << 31

---

#### Puzzle 7: fitsBits

**目标**: 判断x是否能用n位补码表示

```c
/*
 * fitsBits - return 1 if x can be represented as an
 *  n-bit, two's complement integer.
 *   1 <= n <= 32
 *   Examples: fitsBits(5,3) = 0, fitsBits(-4,3) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int fitsBits(int x, int n) {
    int shift = 32 + (~n + 1);  // 32 - n
    return !(x ^ ((x << shift) >> shift));
}
```

**解题思路**:

1. 左移(32-n)位再右移(32-n)位，进行符号扩展
2. 如果结果等于原值，说明能用n位表示
3. 异或后取非判断是否相等

---

#### Puzzle 8: divpwr2

**目标**: 计算 x / 2^n 向零取整

```c
/*
 * divpwr2 - Compute x/(2^n), for 0 <= n <= 30
 *  Round toward zero
 *   Examples: divpwr2(15,1) = 7, divpwr2(-33,4) = -2
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
int divpwr2(int x, int n) {
    int sign = x >> 31;
    int bias = sign & ((1 << n) + ~0);  // sign & ((1 << n) - 1)
    return (x + bias) >> n;
}
```

**解题思路**:

1. 正数直接右移即可
2. 负数需要加偏置(bias = 2^n - 1)再右移
3. 这样实现向零取整而非向下取整

---

### 3.2 补码运算 Puzzles

#### Puzzle 9: negate

**目标**: 返回 -x

```c
/*
 * negate - return -x
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
    return ~x + 1;
}
```

**解题思路**:

- 补码取负：按位取反再加1

---

#### Puzzle 10: isPositive

**目标**: 判断x是否为正数

```c
/*
 * isPositive - return 1 if x > 0, return 0 otherwise
 *   Example: isPositive(-1) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 3
 */
int isPositive(int x) {
    return !((x >> 31) | !x);
}
```

**解题思路**:

1. x >> 31 提取符号位（负数时为-1）
2. !x 判断是否为0（0时为1）
3. 或运算后，正数得0，负数和零得非零
4. 最后再取非

---

#### Puzzle 11: isLessOrEqual

**目标**: 判断 x <= y

```c
/*
 * isLessOrEqual - if x <= y  then return 1, else return 0
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
    int signX = (x >> 31) & 1;
    int signY = (y >> 31) & 1;
    int signDiff = signX ^ signY;  // 1 if signs differ

    // If signs differ, x <= y only if x is negative
    // If signs same, y - x >= 0
    int diff = y + (~x + 1);  // y - x
    int signDiffResult = signX;  // signs differ, result is sign of x
    int sameSignResult = !(diff >> 31);  // signs same, check y >= x

    return (signDiff & signDiffResult) | ((~signDiff) & sameSignResult);
}
```

**解题思路**:

1. 如果符号不同，x <= y 当且仅当x为负数
2. 如果符号相同，计算 y - x，检查符号位
3. 用符号差异作为选择器

---

#### Puzzle 12: ilog2

**目标**: 计算 log2(x) 向下取整

```c
/*
 * ilog2 - return floor(log base 2 of x), where x > 0
 *   Example: ilog2(16) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 90
 *   Rating: 4
 */
int ilog2(int x) {
    int result = 0;
    int mask;

    // Check if any bits in high 16 bits are set
    mask = !!(x >> 16);
    result = result + (mask << 4);
    x = x >> (mask << 4);

    // Check high 8 bits of remaining
    mask = !!(x >> 8);
    result = result + (mask << 3);
    x = x >> (mask << 3);

    // Check high 4 bits
    mask = !!(x >> 4);
    result = result + (mask << 2);
    x = x >> (mask << 2);

    // Check high 2 bits
    mask = !!(x >> 2);
    result = result + (mask << 1);
    x = x >> (mask << 1);

    // Check high 1 bit
    mask = !!(x >> 1);
    result = result + mask;

    return result;
}
```

**解题思路**:

1. 二分查找最高位1的位置
2. 依次检查高16位、高8位、高4位、高2位、高1位
3. 累计偏移量得到结果

---

### 3.3 浮点运算 Puzzles

#### Puzzle 13: float_neg

**目标**: 返回浮点数的相反数，处理NaN

```c
/*
 * float_neg - Return bit-level equivalent of expression -f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representations of
 *   single-precision floating point values.
 *   When argument is NaN, return argument.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 10
 *   Rating: 2
 */
unsigned float_neg(unsigned uf) {
    unsigned mask = 0x80000000;
    unsigned result = uf ^ mask;  // Flip sign bit
    unsigned exp = uf & 0x7F800000;
    unsigned frac = uf & 0x007FFFFF;

    // Check for NaN: exp all 1s and frac non-zero
    if (exp == 0x7F800000 && frac != 0) {
        return uf;
    }
    return result;
}
```

**解题思路**:

1. 翻转符号位得到相反数
2. 检查NaN：阶码全1且尾数非零
3. NaN返回原值

---

#### Puzzle 14: float_i2f

**目标**: 将int转换为float

```c
/*
 * float_i2f - Return bit-level equivalent of expression (float) x
 *   Result is returned as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point values.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_i2f(int x) {
    unsigned sign, exp, frac, bias = 127;
    unsigned result;
    int shift;

    if (x == 0) return 0;

    // Handle INT_MIN specially
    if (x == 0x80000000) {
        return 0xCF000000;  // -2^31
    }

    sign = (x < 0) ? 0x80000000 : 0;
    if (x < 0) x = -x;

    // Find position of highest 1 bit
    shift = 0;
    unsigned temp = x;
    while (temp >> shift) {
        shift++;
    }
    shift--;

    exp = shift + bias;

    // Extract fraction bits (23 bits)
    if (shift <= 23) {
        frac = x << (23 - shift);
    } else {
        // Need to round
        int round_shift = shift - 23;
        frac = (x >> round_shift) & 0x7FFFFF;
        unsigned round_bits = x & ((1 << round_shift) - 1);
        unsigned half = 1 << (round_shift - 1);

        // Round to nearest even
        if (round_bits > half || (round_bits == half && (frac & 1))) {
            frac++;
            if (frac == 0x800000) {  // Overflow to next exponent
                frac = 0;
                exp++;
            }
        }
    }

    result = sign | (exp << 23) | frac;
    return result;
}
```

**解题思路**:

1. 处理0和INT_MIN特殊情况
2. 确定符号位
3. 找到最高位1的位置确定阶码
4. 提取尾数并处理舍入
5. 组装结果

---

#### Puzzle 15: float_twice

**目标**: 浮点数乘以2

```c
/*
 * float_twice - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned float_twice(unsigned uf) {
    unsigned sign = uf & 0x80000000;
    unsigned exp = (uf >> 23) & 0xFF;
    unsigned frac = uf & 0x007FFFFF;

    // NaN or Infinity
    if (exp == 0xFF) {
        return uf;
    }

    // Denormalized: shift fraction left
    if (exp == 0) {
        frac = frac << 1;
        // Check if becomes normalized
        if (frac & 0x00800000) {
            exp = 1;
            frac = frac & 0x007FFFFF;
        }
    } else {
        // Normalized: increment exponent
        exp++;
        // Check for overflow to infinity
        if (exp == 0xFF) {
            frac = 0;
        }
    }

    return sign | (exp << 23) | frac;
}
```

**解题思路**:

1. NaN或无穷大直接返回
2. 非规格化数：尾数左移（可能变为规格化）
3. 规格化数：阶码加1（可能溢出到无穷）

---

## 4. 测试方法

### 4.1 本地测试

```bash
# 编译测试
make

# 运行所有测试
./btest

# 测试特定函数
./btest -f bitAnd
./btest -f getByte

# 详细输出
./btest -v
```

### 4.2 检查操作符数量

```bash
# 使用dlc检查是否符合限制
./dlc bits.c

# 显示每个函数的操作数
./dlc -z bits.c
```

### 4.3 完整测试脚本

```bash
#!/bin/bash

echo "=== Data Lab Test Script ==="

# Check syntax
echo "Checking syntax..."
./dlc bits.c
if [ $? -ne 0 ]; then
    echo "Syntax check failed!"
    exit 1
fi

# Compile
echo "Compiling..."
make clean && make

# Run tests
echo "Running tests..."
./btest

echo "Test complete!"
```

---

## 5. 常见错误与调试

### 5.1 常见错误

| 错误 | 原因 | 解决 |
|------|------|------|
| 使用了非法操作符 | 使用了&&, ||等 | 改用&, \| |
| 操作数超限 | 实现太复杂 | 寻找更优算法 |
| 移位溢出 | 移位数>=字长 | 检查移位范围 |
| 符号扩展问题 | 右移时符号位扩展 | 使用掩码 |

### 5.2 调试技巧

```c
// 打印二进制表示
void print_binary(int x) {
    for (int i = 31; i >= 0; i--) {
        printf("%d", (x >> i) & 1);
        if (i % 8 == 0) printf(" ");
    }
    printf("\n");
}

// 检查中间结果
int test_function() {
    int step1 = ...;
    printf("step1 = 0x%08x\n", step1);
    int step2 = ...;
    printf("step2 = 0x%08x\n", step2);
    return step2;
}
```

---

## 6. 性能优化建议

### 6.1 算法选择

| Puzzle | 推荐算法 | 操作数 |
|--------|---------|--------|
| bitCount | 并行分治 | ~20 |
| ilog2 | 二分查找 | ~20 |
| float_i2f | 位操作 | ~25 |

### 6.2 优化技巧

1. **预计算常量**: 复杂的掩码可以分步构造
2. **避免分支**: 使用位操作替代if语句
3. **复用中间结果**: 减少重复计算
4. **利用符号扩展**: 巧妙的移位可以简化代码

---

**创建日期**: 2026-03-19
