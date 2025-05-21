#include <stdio.h>
#include <string.h>

#define MAX_LEN 10
#define MAX_ITEMS 3  // 数组长度常量

typedef struct {
    char code[MAX_LEN];
    int values[MAX_ITEMS];  // 整型数组
    double rates[MAX_ITEMS]; // 浮点数组
} Data;

int main() {
    Data sample;
    
    // 初始化字符串
    strncpy(sample.code, "A1B2C3", MAX_LEN);

    // 初始化数组（混合进制）
    sample.values[0] = 0x2F;    // 十六进制
    sample.values[1] = 077;     // 八进制
    sample.values[2] = 42;      // 十进制

    // 浮点数组初始化
    sample.rates[0] = 5.75;
    sample.rates[1] = 3.14;
    sample.rates[2] = 0.618;

    // 带数组遍历的输出
    printf("Code: %s\n", sample.code);
    for (int i = 0; i < MAX_ITEMS; i++) {
        printf("Item %d: 0x%X | %04.2f\n", 
               i, sample.values[i], sample.rates[i]);
    }
    return 0;
}