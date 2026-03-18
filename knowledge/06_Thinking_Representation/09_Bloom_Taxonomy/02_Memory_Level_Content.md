# 记忆层内容完整清单 (Remember Level)

> 本文档提供C语言学习中最基础的记忆层内容，包括关键字、运算符、标准库函数等核心要素的完整清单和记忆工具。

---

## 1. C关键字完整清单

### 1.1 C89/C90 关键字 (32个)

| 关键字 | 类别 | 记忆提示 |
|--------|------|----------|
| `auto` | 存储类 | 自动变量，现已少用 |
| `break` | 控制流 | 跳出循环/开关 |
| `case` | 控制流 | switch分支标签 |
| `char` | 类型 | 字符类型 |
| `const` | 限定符 | 常量限定 |
| `continue` | 控制流 | 继续下一次循环 |
| `default` | 控制流 | switch默认分支 |
| `do` | 控制流 | do-while循环 |
| `double` | 类型 | 双精度浮点 |
| `else` | 控制流 | if的否定分支 |
| `enum` | 类型 | 枚举类型 |
| `extern` | 存储类 | 外部链接 |
| `float` | 类型 | 单精度浮点 |
| `for` | 控制流 | for循环 |
| `goto` | 控制流 | 无条件跳转 |
| `if` | 控制流 | 条件判断 |
| `int` | 类型 | 整数类型 |
| `long` | 类型 | 长整型修饰符 |
| `register` | 存储类 | 建议寄存器存储 |
| `return` | 控制流 | 函数返回 |
| `short` | 类型 | 短整型修饰符 |
| `signed` | 类型 | 有符号修饰符 |
| `sizeof` | 运算符 | 获取大小 |
| `static` | 存储类 | 静态存储期 |
| `struct` | 类型 | 结构体类型 |
| `switch` | 控制流 | 多路分支 |
| `typedef` | 声明 | 类型别名 |
| `union` | 类型 | 联合体类型 |
| `unsigned` | 类型 | 无符号修饰符 |
| `void` | 类型 | 无类型/空类型 |
| `volatile` | 限定符 | 易变限定 |
| `while` | 控制流 | while循环 |

### 1.2 C99 新增关键字 (5个)

| 关键字 | 类别 | 记忆提示 |
|--------|------|----------|
| `_Bool` | 类型 | 布尔类型 |
| `_Complex` | 类型 | 复数类型 |
| `_Imaginary` | 类型 | 虚数类型 |
| `inline` | 函数 | 内联函数 |
| `restrict` | 限定符 | 限制指针别名 |

### 1.3 C11 新增关键字 (7个)

| 关键字 | 类别 | 记忆提示 |
|--------|------|----------|
| `_Alignas` | 对齐 | 指定对齐要求 |
| `_Alignof` | 对齐 | 查询对齐要求 |
| `_Atomic` | 原子 | 原子类型限定 |
| `_Generic` | 泛型 | 泛型选择 |
| `_Noreturn` | 函数 | 无返回函数 |
| `_Static_assert` | 断言 | 编译期断言 |
| `_Thread_local` | 存储类 | 线程本地存储 |

### 1.4 C23 新增关键字 (5个)

| 关键字 | 类别 | 记忆提示 |
|--------|------|----------|
| `alignas` | 对齐 | _Alignas的简化形式 |
| `alignof` | 对齐 | _Alignof的简化形式 |
| `bool` | 类型 | _Bool的简化形式 |
| `static_assert` | 断言 | _Static_assert的简化形式 |
| `thread_local` | 存储类 | _Thread_local的简化形式 |
| `nullptr` | 常量 | 空指针常量 |
| `typeof` | 类型 | 类型推导 |
| `typeof_unqual` | 类型 | 非限定类型推导 |
| `constexpr` | 函数 | 常量表达式函数 |

**总计**: C89(32) + C99(5) + C11(7) + C23(9) = **53个关键字**

---

## 2. Anki卡片模板

### 2.1 基础语法卡片模板

#### 卡片组1: 数据类型关键字

```
正面: C语言中用于声明整数的关键字是什么？
背面: int
补充: 可与short/long/signed/unsigned组合使用
记忆法: integer的缩写
```

```
正面: 声明无符号整数的关键字组合是？
背面: unsigned int (可简写为unsigned)
补充: 取值范围0到2^n-1，n为位数
注意: 与signed int默认等价的区别
```

```
正面: C99新增的布尔类型关键字是？
背面: _Bool
补充: 包含头文件<stdbool.h>后可用bool宏
取值: 0为假，非0为真，存储为0或1
```

```
正面: 声明单精度浮点类型的关键字是？
背面: float
补充: 通常32位，IEEE 754单精度格式
精度: 约6-7位有效数字
```

```
正面: 声明双精度浮点类型的关键字是？
背面: double
补充: 通常64位，IEEE 754双精度格式
精度: 约15-17位有效数字
```

#### 卡片组2: 存储类关键字

```
正面: 声明外部链接的关键字是？
背面: extern
使用场景: 多文件共享全局变量/函数声明
注意: 声明不分配存储，定义才分配
```

```
正面: 声明静态存储期的两个关键字是？
背面: static, extern
区别:
- static: 内部链接，文件作用域
- extern: 外部链接，程序作用域
```

```
正面: 声明自动存储期的关键字是？
背面: auto
现状: C99后主要用于声明类型推导(auto在C23)
历史: C89中声明栈上自动变量
```

```
正面: C11新增的线程本地存储关键字是？
背面: _Thread_local (C23为thread_local)
头文件: <threads.h> 提供 thread_local 宏
作用: 每个线程拥有独立实例
```

#### 卡片组3: 类型限定符

```
正面: 声明只读变量的关键字是？
背面: const
注意: const变量必须在声明时初始化
技巧: 读作"指向常量的指针" vs "常量指针"
```

```
正面: 声明禁止编译器优化的关键字是？
背面: volatile
使用场景: 硬件寄存器、多线程共享变量
含义: 每次访问都必须从内存读取
```

```
正面: C99新增的限制指针别名关键字是？
背面: restrict
作用: 承诺指针是访问对象的唯一方式
优化: 允许编译器进行更激进的优化
```

```
正面: C11新增的原子操作关键字是？
背面: _Atomic (C23为atomic或_Atomic)
头文件: <stdatomic.h>
作用: 保证多线程环境下的原子访问
```

#### 卡片组4: 控制流关键字

```
正面: 跳出当前循环或switch的关键字是？
背面: break
注意: 只能跳出一层循环
对比: 与continue的区别
```

```
正面: 跳过本次循环剩余部分的关键字是？
背面: continue
作用: 立即开始下一次循环迭代
注意: 在for循环中仍执行迭代表达式
```

```
正面: 无条件跳转到标签的关键字是？
背面: goto
争议: 可能导致 spaghetti code
适用: 错误处理、跳出多层循环
```

```
正面: 从函数返回的关键字是？
背面: return
void函数: 可省略return或写return;
非void函数: 必须返回对应类型的值
```

```
正面: switch语句中默认分支的关键字是？
背面: default
位置: 可以放在任何case位置
建议: 总是加上default处理意外情况
```

#### 卡片组5: 复合类型关键字

```
正面: 声明结构体类型的关键字是？
背面: struct
内存: 成员按顺序分配，可能有填充
访问: 使用点(.)或箭头(->)运算符
```

```
正面: 声明联合体类型的关键字是？
背面: union
特点: 所有成员共享同一块内存
大小: 等于最大成员的大小
用途: 类型双关、节省内存
```

```
正面: 声明枚举类型的关键字是？
背面: enum
底层: 通常实现为int类型
优点: 提高代码可读性，编译器可检查
```

```
正面: 创建类型别名的关键字是？
背面: typedef
常见用法: typedef struct { } TypeName;
注意: 不创建新类型，只是别名
```

### 2.2 高级特性卡片模板

#### 卡片组6: C11/C23新特性

```
正面: 查询类型对齐要求的关键字是？
背面: _Alignof (C23为alignof)
返回值: size_t类型的对齐字节数
用途: 手动内存对齐、优化缓存
```

```
正面: 指定变量/类型对齐要求的关键字是？
背面: _Alignas (C23为alignas)
语法: _Alignas(对齐值) 或 _Alignas(类型)
限制: 对齐值必须是2的幂
```

```
正面: 编译期静态断言的关键字是？
背面: _Static_assert (C23为static_assert)
语法: _Static_assert(常量表达式, "错误信息")
作用: 编译时检查，失败则编译错误
```

```
正面: 声明不返回的函数的关键字是？
背面: _Noreturn (C23为noreturn)
头文件: <stdnoreturn.h> 提供 noreturn 宏
示例: exit(), abort(), 无限循环函数
```

```
正面: C11泛型选择的关键字是？
背面: _Generic
语法: _Generic(表达式, 类型: 值, ...)
用途: 实现类型安全的泛型宏
示例: #define abs(x) _Generic((x), int: abs_int, ...)
```

```
正面: C23新增的typeof关键字的作用是？
背面: 类型推导，获取表达式的类型
对比: typeof保留限定符，typeof_unqual去除
用途: 泛型编程、宏定义
```

```
正面: C23新增的nullptr与NULL的区别是？
背面: nullptr是nullptr_t类型的空指针常量
NULL: 可能是(void*)0或0或0L
nullptr: 可转换为任何指针类型，不可转换为整数
```

```
正面: C23新增的constexpr关键字的作用是？
背面: 声明常量表达式函数/变量
作用: 保证在编译期求值
约束: 函数体内有限制，不能有任何副作用
```

---

## 3. 运算符优先级速查表

### 3.1 完整优先级表 (15级)

| 优先级 | 运算符 | 结合性 | 描述 |
|--------|--------|--------|------|
| 1 (最高) | `()` `[]` `->` `.` | 左到右 | 函数调用、数组下标、成员访问 |
| 2 | `++` `--` (后缀) | 左到右 | 后缀自增自减 |
| 2 | `++` `--` (前缀) `+` `-` `!` `~` `*` `&` `sizeof` `_Alignof` | 右到左 | 前缀运算符、单目运算符 |
| 2 | `(类型)` | 右到左 | 类型转换 |
| 3 | `*` `/` `%` | 左到右 | 乘除模 |
| 4 | `+` `-` | 左到右 | 加减 |
| 5 | `<<` `>>` | 左到右 | 位移 |
| 6 | `<` `<=` `>` `>=` | 左到右 | 关系比较 |
| 7 | `==` `!=` | 左到右 | 相等比较 |
| 8 | `&` | 左到右 | 按位与 |
| 9 | `^` | 左到右 | 按位异或 |
| 10 | `\|` | 左到右 | 按位或 |
| 11 | `&&` | 左到右 | 逻辑与 |
| 12 | `\|\|` | 左到右 | 逻辑或 |
| 13 | `?:` | 右到左 | 条件运算符 |
| 14 | `=` `+=` `-=` `*=` `/=` `%=` `&=` `^=` `\|=` `<<=` `>>=` | 右到左 | 赋值运算符 |
| 15 (最低) | `,` | 左到右 | 逗号运算符 |

### 3.2 记忆口诀

```
口诀: "初等单目算关位，逻条赋逗低'

初: 初等运算符 () [] -> .
等: 单目运算符 ++ -- + - ! ~ * & sizeof
算: 算术运算符 * / % + -
关: 关系运算符 < <= > >= == !=
位: 位运算符 & ^ | << >>
逻: 逻辑运算符 && ||
条: 条件运算符 ?:
赋: 赋值运算符 = += -= ...
逗: 逗号运算符 ,
```

### 3.3 常见优先级陷阱

| 陷阱表达式 | 实际解析 | 正确写法 |
|-----------|----------|----------|
| `a & b == c` | `a & (b == c)` | `(a & b) == c` |
| `a << b + c` | `a << (b + c)` | `(a << b) + c` |
| `*p++` | `*(p++)` | 取p指向的值，然后p++ |
| `*p.f` | `*(p.f)` | `(*p).f` 或 `p->f` |
| `a + b << c` | `(a + b) << c` | 加法优先级高于位移 |

### 3.4 易混淆运算符对比

| 运算符 | 名称 | 用法示例 | 结果 |
|--------|------|----------|------|
| `&` | 取地址 | `&a` | a的地址 |
| `&` | 按位与 | `a & b` | 按位与结果 |
| `&&` | 逻辑与 | `a && b` | 逻辑与结果 |
| `\|` | 按位或 | `a \| b` | 按位或结果 |
| `\|\|` | 逻辑或 | `a \|\| b` | 逻辑或结果 |
| `^` | 按位异或 | `a ^ b` | 按位异或结果 |
| `!` | 逻辑非 | `!a` | 逻辑取反 |
| `~` | 按位取反 | `~a` | 按位取反 |

---

## 4. 标准库函数原型索引

### 4.1 stdio.h 核心函数 (20+)

| 函数 | 原型 | 用途 |
|------|------|------|
| printf | `int printf(const char *format, ...);` | 格式化输出到stdout |
| fprintf | `int fprintf(FILE *stream, const char *format, ...);` | 格式化输出到流 |
| sprintf | `int sprintf(char *str, const char *format, ...);` | 格式化输出到字符串 |
| snprintf | `int snprintf(char *str, size_t size, const char *format, ...);` | 安全格式化输出到字符串 |
| scanf | `int scanf(const char *format, ...);` | 从stdin格式化输入 |
| fscanf | `int fscanf(FILE *stream, const char *format, ...);` | 从流格式化输入 |
| sscanf | `int sscanf(const char *str, const char *format, ...);` | 从字符串格式化输入 |
| getchar | `int getchar(void);` | 从stdin读取一个字符 |
| putchar | `int putchar(int c);` | 向stdout写入一个字符 |
| gets | `char *gets(char *s);` | 从stdin读取一行 (C11移除，不安全) |
| fgets | `char *fgets(char *s, int size, FILE *stream);` | 从流安全读取一行 |
| puts | `int puts(const char *s);` | 向stdout写入字符串+换行 |
| fputs | `int fputs(const char *s, FILE *stream);` | 向流写入字符串 |
| fopen | `FILE *fopen(const char *path, const char *mode);` | 打开文件 |
| fclose | `int fclose(FILE *stream);` | 关闭文件 |
| fread | `size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);` | 二进制读 |
| fwrite | `size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);` | 二进制写 |
| fseek | `int fseek(FILE *stream, long offset, int whence);` | 定位文件位置 |
| ftell | `long ftell(FILE *stream);` | 获取当前文件位置 |
| rewind | `void rewind(FILE *stream);` | 重置文件位置到开头 |
| fflush | `int fflush(FILE *stream);` | 刷新流缓冲区 |
| remove | `int remove(const char *pathname);` | 删除文件 |
| rename | `int rename(const char *oldpath, const char *newpath);` | 重命名文件 |
| perror | `void perror(const char *s);` | 打印错误信息 |

### 4.2 stdlib.h 核心函数 (20+)

| 函数 | 原型 | 用途 |
|------|------|------|
| malloc | `void *malloc(size_t size);` | 分配内存 |
| calloc | `void *calloc(size_t nmemb, size_t size);` | 分配并清零内存 |
| realloc | `void *realloc(void *ptr, size_t size);` | 重新分配内存 |
| free | `void free(void *ptr);` | 释放内存 |
| exit | `void exit(int status);` | 正常终止程序 |
| abort | `void abort(void);` | 异常终止程序 |
| atexit | `int atexit(void (*func)(void));` | 注册退出处理函数 |
| system | `int system(const char *command);` | 执行系统命令 |
| getenv | `char *getenv(const char *name);` | 获取环境变量 |
| setenv | `int setenv(const char *name, const char *value, int overwrite);` | 设置环境变量(POSIX) |
| atoi | `int atoi(const char *nptr);` | 字符串转int |
| atol | `long atol(const char *nptr);` | 字符串转long |
| atof | `double atof(const char *nptr);` | 字符串转double |
| strtol | `long strtol(const char *nptr, char **endptr, int base);` | 字符串转long，更安全 |
| strtoll | `long long strtoll(const char *nptr, char **endptr, int base);` | 字符串转long long |
| strtoul | `unsigned long strtoul(const char *nptr, char **endptr, int base);` | 字符串转unsigned long |
| strtod | `double strtod(const char *nptr, char **endptr);` | 字符串转double |
| rand | `int rand(void);` | 生成伪随机数 |
| srand | `void srand(unsigned int seed);` | 设置随机数种子 |
| qsort | `void qsort(void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *));` | 快速排序 |
| bsearch | `void *bsearch(const void *key, const void *base, size_t nmemb, size_t size, int (*compar)(const void *, const void *));` | 二分查找 |
| abs | `int abs(int j);` | 整数绝对值 |
| labs | `long labs(long j);` | long绝对值 |
| llabs | `long long llabs(long long j);` | long long绝对值(C99) |
| div | `div_t div(int numer, int denom);` | 整数除法 |
| ldiv | `ldiv_t ldiv(long numer, long denom);` | long除法 |

### 4.3 string.h 核心函数 (15+)

| 函数 | 原型 | 用途 |
|------|------|------|
| strcpy | `char *strcpy(char *dest, const char *src);` | 复制字符串 |
| strncpy | `char *strncpy(char *dest, const char *src, size_t n);` | 复制指定长度字符串 |
| strcat | `char *strcat(char *dest, const char *src);` | 连接字符串 |
| strncat | `char *strncat(char *dest, const char *src, size_t n);` | 连接指定长度字符串 |
| strcmp | `int strcmp(const char *s1, const char *s2);` | 比较字符串 |
| strncmp | `int strncmp(const char *s1, const char *s2, size_t n);` | 比较指定长度字符串 |
| strlen | `size_t strlen(const char *s);` | 获取字符串长度 |
| strchr | `char *strchr(const char *s, int c);` | 查找字符首次出现 |
| strrchr | `char *strrchr(const char *s, int c);` | 查找字符最后出现 |
| strstr | `char *strstr(const char *haystack, const char *needle);` | 查找子字符串 |
| strtok | `char *strtok(char *str, const char *delim);` | 分割字符串 |
| memcpy | `void *memcpy(void *dest, const void *src, size_t n);` | 复制内存块 |
| memmove | `void *memmove(void *dest, const void *src, size_t n);` | 安全复制重叠内存块 |
| memcmp | `int memcmp(const void *s1, const void *s2, size_t n);` | 比较内存块 |
| memset | `void *memset(void *s, int c, size_t n);` | 填充内存块 |
| memchr | `void *memchr(const void *s, int c, size_t n);` | 在内存块中查找字符 |
| strerror | `char *strerror(int errnum);` | 获取错误描述字符串 |

### 4.4 ctype.h 字符处理函数 (11个)

| 函数 | 原型 | 用途 |
|------|------|------|
| isalnum | `int isalnum(int c);` | 是否为字母或数字 |
| isalpha | `int isalpha(int c);` | 是否为字母 |
| isdigit | `int isdigit(int c);` | 是否为数字 |
| isspace | `int isspace(int c);` | 是否为空白字符 |
| islower | `int islower(int c);` | 是否为小写字母 |
| isupper | `int isupper(int c);` | 是否为大写字母 |
| isprint | `int isprint(int c);` | 是否为可打印字符 |
| isgraph | `int isgraph(int c);` | 是否为图形字符(不含空格) |
| tolower | `int tolower(int c);` | 转为小写 |
| toupper | `int toupper(int c);` | 转为大写 |
| isxdigit | `int isxdigit(int c);` | 是否为十六进制数字 |

### 4.5 math.h 核心函数 (20+)

| 函数 | 原型 | 用途 |
|------|------|------|
| sqrt | `double sqrt(double x);` | 平方根 |
| pow | `double pow(double x, double y);` | 幂运算 |
| exp | `double exp(double x);` | e的x次幂 |
| log | `double log(double x);` | 自然对数 |
| log10 | `double log10(double x);` | 常用对数 |
| sin | `double sin(double x);` | 正弦 |
| cos | `double cos(double x);` | 余弦 |
| tan | `double tan(double x);` | 正切 |
| asin | `double asin(double x);` | 反正弦 |
| acos | `double acos(double x);` | 反余弦 |
| atan | `double atan(double x);` | 反正切 |
| atan2 | `double atan2(double y, double x);` | 四象限反正切 |
| sinh | `double sinh(double x);` | 双曲正弦 |
| cosh | `double cosh(double x);` | 双曲余弦 |
| tanh | `double tanh(double x);` | 双曲正切 |
| ceil | `double ceil(double x);` | 向上取整 |
| floor | `double floor(double x);` | 向下取整 |
| fabs | `double fabs(double x);` | 浮点绝对值 |
| fmod | `double fmod(double x, double y);` | 浮点模运算 |
| round | `double round(double x);` | 四舍五入(C99) |
| trunc | `double trunc(double x);` | 向零取整(C99) |
| isnan | `int isnan(double x);` | 检查NaN(C99) |
| isinf | `int isinf(double x);` | 检查无穷(C99) |

---

## 5. 记忆技巧

### 5.1 关键字分类记忆法

```
按功能分组记忆:

类型相关(13个): char, short, int, long, float, double,
                void, signed, unsigned, _Bool/_Complex/_Imaginary, enum, struct, union

存储类(5个): auto, static, extern, register, _Thread_local

控制流(12个): if, else, switch, case, default, for, while, do,
              break, continue, goto, return

限定符(5个): const, volatile, restrict, _Atomic, _Alignas/_Alignof

其他(6个): sizeof, typedef, inline, _Noreturn, _Static_assert, _Generic
```

### 5.2 运算符优先级记忆技巧

```
场景联想记忆:

1. 函数调用最优先 - 就像紧急电话优先接通
2. 单目运算符其次 - 一个人做事最干脆
3. 算术运算在中间 - 日常计算最常用
4. 关系比较随后 - 比较才有判断
5. 逻辑运算靠后 - 综合考虑后决定
6. 赋值运算倒数 - 最后才保存结果
7. 逗号运算最后 - 最不重要的事情最后做
```

### 5.3 标准库函数前缀记忆

| 前缀 | 含义 | 示例 |
|------|------|------|
| `f` | file/文件操作 | fopen, fread, fwrite |
| `s` | string/字符串 | strcpy, strcat, strlen |
| `n` | n个/指定数量 | strncpy, strncat, snprintf |
| `is` | is/判断 | isdigit, isspace, isalpha |
| `to` | to/转换 | tolower, toupper |
| `str` | string/字符串操作 | strcpy, strcmp, strchr |
| `mem` | memory/内存操作 | memcpy, memset, memcmp |
| `at` | ASCII to/转换 | atoi, atol, atof |
| `abs` | absolute/绝对值 | abs, labs, fabs |

### 5.4 Anki学习建议

```
最佳实践:
1. 每天学习20-30张新卡片
2. 坚持每日复习，不要堆积
3. 主动回忆，不要偷看答案
4. 卡片要简洁，一个概念一张卡
5. 定期整理，删除已掌握的简单卡片
6. 结合代码实践，加深记忆

记忆曲线:
- 第1次复习: 学习后1天
- 第2次复习: 学习后3天
- 第3次复习: 学习后7天
- 第4次复习: 学习后14天
- 第5次复习: 学习后30天
```

---

## 6. 与知识库的关联

| 内容 | 关联文档 |
|------|----------|
| 关键字详解 | `01_Core_Knowledge_System/01_Basic_Layer/01_Keywords_Detailed.md` |
| 运算符详解 | `01_Core_Knowledge_System/01_Basic_Layer/02_Operators_Complete.md` |
| 标准库文档 | `01_Core_Knowledge_System/03_Standard_Library/` |
| 记忆技巧 | `06_Thinking_Representation/03_Metaphors/` |
| Anki卡片源文件 | `scripts/anki_cards/` |

---

> **记忆层目标**: 能够不借助参考资料，准确回忆C语言的所有关键字、运算符优先级和标准库核心函数原型。

---

**文档版本**: 1.0
**创建日期**: 2026-03-19
**关联文档**: [01_Bloom_Overview.md](01_Bloom_Overview.md)
