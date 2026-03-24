/*
 * C23 #embed 指令示例
 * 编译: gcc -std=c23 -o 08_embed_directive 08_embed_directive.c
 *       clang -std=c23 -o 08_embed_directive 08_embed_directive.c
 * 
 * 注意：需要配套的数据文件才能编译成功
 */

#include <stdio.h>
#include <string.h>

// 嵌入二进制数据 - 替换传统的 xxd 或 bin2c 工具
// 创建一个示例数据文件
// echo -n "Hello, C23!" > data.txt

#if __STDC_EMBED_FOUND__ == 1
    // 成功找到文件时的嵌入
    static const unsigned char hello_data[] = {
        #embed "data.txt"
    };
    static const size_t hello_data_len = sizeof(hello_data);
#else
    // 回退实现
    static const unsigned char hello_data[] = "Hello, C23!";
    static const size_t hello_data_len = sizeof(hello_data) - 1;
#endif

// 嵌入时可使用参数控制
#if __STDC_EMBED_FOUND__ == 1
    // 只嵌入前4个字节
    static const unsigned char header[] = {
        #embed "data.txt" limit(4)
    };
    static const size_t header_len = sizeof(header);
    
    // 从偏移量2开始嵌入
    static const unsigned char suffix[] = {
        #embed "data.txt" prefix(2)
    };
    static const size_t suffix_len = sizeof(suffix);
#endif

// 使用嵌入数据的查找表
static const unsigned char sin_table[] = {
    // 这里可以嵌入预计算的sin表
    // #embed "sin_table.bin"
    
    // 示例：手动定义的简化的sin表（0-90度，10度步长）
    0, 17, 34, 50, 64, 77, 87, 94, 98, 100
};

// 嵌入图像数据的示例（需要实际图像文件）
#if 0  // 示例代码，需要实际文件
static const unsigned char logo_png[] = {
    #embed "logo.png"
};
static const size_t logo_png_size = sizeof(logo_png);
#endif

int main(void) {
    printf("C23 #embed Directive Demo\n\n");
    
    // 显示嵌入的数据
    printf("Embedded data (hello_data):\n");
    printf("Length: %zu bytes\n", hello_data_len);
    printf("Content: ");
    for (size_t i = 0; i < hello_data_len; i++) {
        printf("%c", hello_data[i]);
    }
    printf("\n");
    
    printf("\nHex dump:\n");
    for (size_t i = 0; i < hello_data_len; i++) {
        printf("%02X ", hello_data[i]);
        if ((i + 1) % 16 == 0) printf("\n");
    }
    printf("\n");
    
#if __STDC_EMBED_FOUND__ == 1
    // 显示限制后的数据
    printf("\nFirst 4 bytes (header):\n");
    for (size_t i = 0; i < header_len; i++) {
        printf("%02X ", header[i]);
    }
    printf("\n");
    
    printf("\nData from offset 2 (suffix):\n");
    for (size_t i = 0; i < suffix_len; i++) {
        printf("%02X ", suffix[i]);
    }
    printf("\n");
#endif
    
    // 使用查找表
    printf("\nSine table lookup:\n");
    for (int i = 0; i < 10; i++) {
        printf("sin(%d°) ≈ %d/100\n", i * 10, sin_table[i]);
    }
    
    printf("\n#embed demo complete!\n");
    printf("\nNote: This demo uses embedded data or fallback.\n");
    printf("To test actual #embed, create a 'data.txt' file.\n");
    
    return 0;
}
