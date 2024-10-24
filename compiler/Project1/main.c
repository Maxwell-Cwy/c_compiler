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
    char ch_buf[1000] = {0};  // ��������ȡ1000���ַ�
    int i = 0;

    if (argc != 2) {  // �ж������в�������
        printf("Usage: %s <program_file>\n", argv[0]);
        return 1;
    }

    filename = argv[1];  // ��ȡ�ļ���
    file = fopen(filename, "r");  // ���ļ�

    if (file == NULL) {  // ����ļ��Ƿ�ɹ���
        printf("Failed to open file: %s\n", filename);
        return 1;
    }

    while ((ch = fgetc(file)) != EOF) {  // ʹ�� fgetc ����ַ���ȡ
        if (!isspace(ch)) {
            ch_buf[i] = ch;  // �洢�ַ�
            i++;
        }
    }

    ch_buf[i] = '\0';  // �ַ�����β
    fclose(file);  // �ر��ļ�

    printf("File content:\n");
    printf("%s\n", ch_buf);  // �����ȡ�����ַ�
    printf("start analyze!\n");
    parseProgram(dfa(ch_buf, token));

    return 0;
}
