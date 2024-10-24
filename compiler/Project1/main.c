#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> 
#include <string.h>
#include "dfa.h"
#include "parse.h"
struct Token token[100];

int main(int argc, char* argv[]) {
    char* filename;
    FILE* file;
    char ch;
    char ch_buf[1000] = {0};  // 假设最多读取1000个字符
    int i = 0;

    if (argc != 2) {  // 判断命令行参数个数
        printf("Usage: %s <program_file>\n", argv[0]);
        return 1;
    }

    filename = argv[1];  // 获取文件名
    file = fopen(filename, "r");  // 打开文件

    if (file == NULL) {  // 检查文件是否成功打开
        printf("Failed to open file: %s\n", filename);
        return 1;
    }

    while ((ch = fgetc(file)) != EOF) {  // 使用 fgetc 逐个字符读取
        if (!isspace(ch)) {
            ch_buf[i] = ch;  // 存储字符
            i++;
        }
    }

    ch_buf[i] = '\0';  // 字符串结尾
    fclose(file);  // 关闭文件

    printf("File content:\n");
    printf("%s\n", ch_buf);  // 输出读取到的字符
    printf("start analyze!\n");
    parseProgram(dfa(ch_buf, token));

    return 0;
}
