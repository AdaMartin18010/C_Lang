# C 语言国际化与本地化 (i18n/l10n)

## 概述

国际化 (Internationalization, i18n) 是设计软件使其能够适应不同语言和区域的过程，而本地化 (Localization, l10n) 是将软件适配到特定区域的过程。C 语言作为系统级编程语言，其国际化支持对于全球软件部署至关重要。

**术语定义**:

- **i18n**: Internationalization (i 和 n 之间有 18 个字母)
- **l10n**: Localization (l 和 n 之间有 10 个字母)
- **g11n**: Globalization - 全球化 (包含 i18n + l10n)
- **t9n**: Translation - 翻译 (l10n 的子集)

---

## 标准国际化架构

### POSIX Locale 模型

```
Locale 层次结构:
┌─────────────────────────────────────────────────────────────┐
│                    Locale 设置                               │
├─────────────┬───────────────────────────────────────────────┤
│   LC_ALL    │  覆盖所有类别 (最高优先级)                       │
├─────────────┼───────────────────────────────────────────────┤
│  LC_CTYPE   │  字符分类和转换                                 │
│  LC_COLLATE │  排序规则                                      │
│  LC_NUMERIC │  数字格式 (小数点、千位分隔符)                   │
│  LC_MONETARY│  货币格式                                      │
│  LC_TIME    │  日期和时间格式                                 │
│  LC_MESSAGES│  本地化消息                                    │
├─────────────┼───────────────────────────────────────────────┤
│    LANG     │  默认 Locale (最低优先级)                       │
└─────────────┴───────────────────────────────────────────────┘
```

### C 标准支持

```c
#include <locale.h>
#include <langinfo.h>
#include <wchar.h>

// 设置用户环境默认 locale
setlocale(LC_ALL, "");

// 或设置特定区域
setlocale(LC_ALL, "zh_CN.UTF-8");   // 简体中文
setlocale(LC_ALL, "ja_JP.UTF-8");   // 日语
setlocale(LC_ALL, "de_DE.UTF-8");   // 德语
setlocale(LC_ALL, "C");              // POSIX C 标准(默认)
```

---

## Unicode 与字符编码

### UTF-8 在 C 语言中的实现

```c
/* UTF-8 编码特性:
 * - ASCII 兼容 (0-127)
 * - 变长编码: 1-4 字节
 * - 最高位识别: 0xxxxxxx | 110xxxxx 10xxxxxx | 1110xxxx ...
 */

#include <stdint.h>
#include <stdbool.h>

// 检查是否为有效的 UTF-8 起始字节
bool is_utf8_start_byte(uint8_t byte) {
    return (byte & 0x80) == 0x00 ||  // ASCII
           (byte & 0xE0) == 0xC0 ||  // 2字节序列开始
           (byte & 0xF0) == 0xE0 ||  // 3字节序列开始
           (byte & 0xF8) == 0xF0;     // 4字节序列开始
}

// 计算 UTF-8 字符长度
size_t utf8_char_len(uint8_t first_byte) {
    if ((first_byte & 0x80) == 0x00) return 1;
    if ((first_byte & 0xE0) == 0xC0) return 2;
    if ((first_byte & 0xF0) == 0xE0) return 3;
    if ((first_byte & 0xF8) == 0xF0) return 4;
    return 0;  // 无效字节
}

// 安全遍历 UTF-8 字符串
void utf8_iterate(const char* str, void (*callback)(const char*, size_t)) {
    const uint8_t* p = (const uint8_t*)str;
    while (*p) {
        size_t len = utf8_char_len(*p);
        if (len == 0) { p++; continue; }  // 跳过无效字节
        callback((const char*)p, len);
        p += len;
    }
}
```

### 宽字符 (Wide Characters)

```c
#include <wchar.h>
#include <locale.h>

// 宽字符字符串处理
void process_wide_string(void) {
    setlocale(LC_ALL, "");

    // 宽字符字面量 (L 前缀)
    wchar_t greeting[] = L"你好世界 🌍";

    // 计算宽字符数量
    size_t len = wcslen(greeting);
    wprintf(L"字符串长度: %zu 个宽字符\n", len);

    // 宽字符 I/O
    wprintf(L"问候: %ls\n", greeting);
}

// 多字节 <-> 宽字符转换
void convert_encoding(void) {
    const char* utf8_str = "Hello 世界";

    // 计算需要的宽字符缓冲区大小
    size_t wlen = mbstowcs(NULL, utf8_str, 0);
    if (wlen == (size_t)-1) {
        perror("转换失败");
        return;
    }

    // 执行转换
    wchar_t* wstr = malloc((wlen + 1) * sizeof(wchar_t));
    mbstowcs(wstr, utf8_str, wlen + 1);

    // 使用...
    free(wstr);
}
```

---

## GNU gettext 框架

### 基础架构

GNU gettext 是 C 语言中最广泛使用的国际化框架：

```
gettext 工作流程:
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   C 源代码       │    │   .pot 模板     │    │  .po 翻译文件    │
│  (标记可翻译文本) │ => │  (提取的字符串)  │ => │  (各语言翻译)    │
└─────────────────┘    └─────────────────┘    └─────────────────┘
                                                        ↓
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   运行时翻译     │ <= │   .mo 二进制    │ <= │   msgfmt 编译   │
│  gettext() 调用  │    │  (机器优化格式)  │    │                 │
└─────────────────┘    └─────────────────┘    └─────────────────┘
```

### 代码标记示例

```c
#include <libintl.h>
#include <locale.h>
#include <stdio.h>

#define _(STRING) gettext(STRING)
#define N_(STRING) (STRING)  // 仅标记，不翻译

int main(void) {
    // 初始化国际化
    setlocale(LC_ALL, "");
    bindtextdomain("myapp", "/usr/share/locale");
    textdomain("myapp");

    // 简单翻译
    printf("%s\n", _("Hello, World!"));

    // 带参数的翻译
    int count = 5;
    printf(ngettext("You have %d message.",
                    "You have %d messages.",
                    count),
           count);

    // 上下文翻译 (pgettext)
    printf("%s\n", pgettext("Menu|File", "Open"));   // 菜单项
    printf("%s\n", pgettext("Dialog|Action", "Open")); // 对话框按钮

    return 0;
}
```

### 复数形式处理

```c
/* 复数规则定义 (.po 文件中):
 *
 * nplurals=2; plural=(n != 1);
 *   - 英语、德语等: 单数(n=1) vs 复数
 *
 * nplurals=3; plural=(n%10==1 && n%100!=11 ? 0 : n%10>=2 && n%10<=4 && (n%100<10 || n%100>=20) ? 1 : 2);
 *   - 俄语、波兰语等复杂复数规则
 *
 * nplurals=1; plural=0;
 *   - 中文、日语: 无复数变化
 */

// 代码中使用
void show_item_count(int n) {
    // gettext 会根据当前 locale 的复数规则选择正确形式
    printf(ngettext(
        "%d item selected",    // 单数形式
        "%d items selected",   // 复数形式
        n                      // 计数
    ), n);
}
```

---

## ICU (International Components for Unicode)

### 工业级国际化库

ICU 是 IBM 维护的工业级 Unicode 和全球化库：

```c
#include <unicode/ustdio.h>
#include <unicode/ucol.h>
#include <unicode/udat.h>
#include <unicode/unum.h>

// ICU 字符串处理 (UChar 是 UTF-16 编码)
void icu_example(void) {
    UErrorCode status = U_ZERO_ERROR;

    // 创建 Unicode 字符串
    UChar str[256];
    u_uastrcpy(str, "Hello, 世界!");

    // 规范化 (NFC 组合形式)
    UChar normalized[256];
    unorm_normalize(str, -1, UNORM_NFC, 0, normalized, 256, &status);

    // 排序比较
    UCollator* coll = ucol_open("zh_CN", &status);
    UCollationResult result = ucol_strcoll(coll, str1, -1, str2, -1);
    ucol_close(coll);
}

// ICU 日期/数字格式化
void icu_formatting(void) {
    UErrorCode status = U_ZERO_ERROR;

    // 日期格式化
    UDateFormat* dfmt = udat_open(
        UDAT_LONG, UDAT_LONG,
        "de_DE",  // 德语
        NULL, -1,
        NULL, -1,
        &status
    );

    UChar date_str[256];
    udat_format(dfmt, ucal_getNow(), date_str, 256, NULL, &status);
    // 结果: "18. März 2026"

    udat_close(dfmt);

    // 货币格式化
    UNumberFormat* nfmt = unum_open(
        UNUM_CURRENCY,
        NULL, 0,
        "ja_JP",  // 日语
        NULL,
        &status
    );

    UChar currency_str[256];
    unum_formatDouble(nfmt, 1234567.89, currency_str, 256, NULL, &status);
    // 结果: "￥1,234,568"

    unum_close(nfmt);
}
```

---

## 跨平台文本处理

### 字符串外部化最佳实践

```c
/* i18n 设计原则:
 * 1. 所有用户可见字符串外部化
 * 2. 避免字符串拼接
 * 3. 支持占位符和参数化
 * 4. 考虑文本扩展 (英语→德语 +30%)
 */

// ❌ 错误做法 - 字符串拼接
char msg[256];
strcpy(msg, "You have ");
strcat(msg, count_str);
strcat(msg, " messages.");

// ✅ 正确做法 - 完整句子
printf(ngettext(
    "You have %d message in %s.",
    "You have %d messages in %s.",
    count
), count, folder_name);

// ❌ 错误做法 - 假设单词顺序
printf(_("Copying "));
printf(filename);
printf(_(" to "));
printf(destination);

// ✅ 正确做法 - 完整句子模板
printf(_("Copying %(source)s to %(dest)s"),
       "source", filename,
       "dest", destination, NULL);
```

### RTL (从右到左) 语言支持

```c
/* RTL 语言: 阿拉伯语、希伯来语、波斯语等
 * - 布局方向镜像
 * - 文本方向处理
 * - 双向文本算法 (BiDi)
 */

#include <fribidi/fribidi.h>

// 处理双向文本
void process_bidi_text(const FriBidiChar* input, int len) {
    FriBidiChar output[256];
    FriBidiParType base_dir = FRIBIDI_PAR_ON;

    // 逻辑视觉转换
    fribidi_log2vis(
        input, len, &base_dir,
        output,           /* 视觉顺序 */
        NULL,             /* 逻辑位置 */
        NULL,             /* 视觉位置 */
        NULL              /* 嵌入级别 */
    );

    // base_dir 会被更新为实际段落方向
}
```

---

## 本地化格式处理

### 数字和货币格式

```c
#include <locale.h>
#include <monetary.h>

void format_locale_numbers(void) {
    setlocale(LC_ALL, "de_DE.UTF-8");

    // 数字格式
    // 德语: 1.234.567,89
    printf("%'0.2f\n", 1234567.89);

    // 货币格式 (strfmon)
    char buf[256];
    strfmon(buf, sizeof(buf), "%n", 1234.56);
    // 结果: "1.234,56 EUR"

    // 不同 locale 对比
    struct {
        const char* locale;
        const char* name;
    } locales[] = {
        {"en_US.UTF-8", "US English"},
        {"de_DE.UTF-8", "German"},
        {"fr_FR.UTF-8", "French"},
        {"ja_JP.UTF-8", "Japanese"},
        {NULL, NULL}
    };

    for (int i = 0; locales[i].locale; i++) {
        setlocale(LC_NUMERIC, locales[i].locale);
        setlocale(LC_MONETARY, locales[i].locale);
        printf("%-15s: %12.2f  ", locales[i].name, 1234567.89);
        strfmon(buf, sizeof(buf), "%(n", 1234.56);
        printf("%s\n", buf);
    }
    /* 输出:
     * US English    : 1,234,567.89  $1,234.56
     * German        : 1.234.567,89  1.234,56 EUR
     * French        : 1 234 567,89  1 234,56 EUR
     * Japanese      : 1234567.89    ￥1,235
     */
}
```

### 日期时间本地化

```c
#include <time.h>
#include <langinfo.h>

void format_locale_date(void) {
    setlocale(LC_ALL, "");

    time_t now = time(NULL);
    struct tm* local = localtime(&now);

    // 获取本地化日期格式
    const char* date_fmt = nl_langinfo(D_FMT);    // %m/%d/%y
    const char* time_fmt = nl_langinfo(T_FMT);    // %H:%M:%S
    const char* ampm_fmt = nl_langinfo(T_FMT_AMPM); // %I:%M:%S %p

    char buf[256];
    strftime(buf, sizeof(buf), date_fmt, local);
    printf("Date: %s\n", buf);

    // 完整本地化日期时间
    strftime(buf, sizeof(buf), "%c", local);  // 首选日期时间表示
    printf("DateTime: %s\n", buf);

    // 各组件
    strftime(buf, sizeof(buf), "%A, %d %B %Y", local);
    // 结果示例: "星期二, 18 三月 2026" (中文)
}
```

---

## 工具链集成

### xgettext 提取流程

```bash
# 1. 提取可翻译字符串
cd project_root
xgettext --keyword=_ --keyword=N_ --keyword=ngettext:1,2 \
         --output=po/messages.pot \
         src/*.c

# 2. 初始化新语言翻译
msginit --input=po/messages.pot \
        --output-file=po/de.po \
        --locale=de_DE.UTF-8

# 3. 更新现有翻译
msgmerge --update po/de.po po/messages.pot

# 4. 编译为二进制
msgfmt --output-file=locale/de/LC_MESSAGES/myapp.mo po/de.po
```

### 现代构建系统集成

```cmake
# CMake i18n 支持示例
find_package(Gettext REQUIRED)
find_package(Intl REQUIRED)

# 翻译文件
set(PO_FILES
    po/de.po
    po/fr.po
    po/ja.po
    po/zh_CN.po
)

# 编译 .po -> .mo
foreach(po_file ${PO_FILES})
    get_filename_component(lang ${po_file} NAME_WE)
    set(mo_dir ${CMAKE_BINARY_DIR}/locale/${lang}/LC_MESSAGES)
    file(MAKE_DIRECTORY ${mo_dir})

    add_custom_command(
        OUTPUT ${mo_dir}/myapp.mo
        COMMAND ${GETTEXT_MSGFMT_EXECUTABLE}
                -o ${mo_dir}/myapp.mo ${po_file}
        DEPENDS ${po_file}
    )
    list(APPEND MO_FILES ${mo_dir}/myapp.mo)
endforeach()

add_custom_target(translations ALL DEPENDS ${MO_FILES})
```

---

## 测试与验证

### 伪本地化 (Pseudolocalization)

```c
/* 伪本地化用于测试 UI 扩展:
 * - 添加前缀和后缀标记
 * - 扩展字符宽度
 * - 测试 RTL 布局
 */

// 伪本地化转换示例
const char* pseudolocalize(const char* original) {
    // "Hello" -> "[Ħḗḗŀŀōō]"
    // 带标记的扩展字符串
}

// 用于检测:
// 1. 硬编码字符串 (未翻译)
// 2. 截断问题
// 3. 布局问题
```

### 国际化测试清单

| 测试项 | 英文 | 德文 | 日文 | 阿拉伯文 |
|--------|------|------|------|----------|
| 字符串渲染 | ✅ | ✅ | ✅ | ✅ (RTL) |
| 日期格式 | 03/18/26 | 18.03.26 | 2026/03/18 | ٢٦/٠٣/١٨ |
| 数字格式 | 1,234.56 | 1.234,56 | 1,234.56 | ١٬٢٣٤٫٥٦ |
| 货币格式 | $1,234.56 | 1.234,56 € | ￥1,235 | ١٬٢٣٤٫٥٦ US$ |
| 排序 | abc... | abc... (ä 在 a 后) | あいう... | العربية |
| 复数处理 | item/items | Element/Elemente | アイテム | عنصر/عناصر |

---

## 参考资源

- [GNU gettext Manual](https://www.gnu.org/software/gettext/manual/)
- [ICU User Guide](https://unicode-org.github.io/icu/userguide/)
- [W3C Internationalization](https://www.w3.org/International/)
- [CLDR - Unicode Common Locale Data Repository](https://cldr.unicode.org/)
- [POSIX Locale](https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap07.html)

---

*最后更新: 2026-03-18 | 涵盖: Unicode 15.1, ICU 75, gettext 0.22*
