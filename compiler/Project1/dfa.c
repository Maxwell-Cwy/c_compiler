#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> 
#include <string.h>
#include "dfa.h"

// 定义 C 语言中的关键词
const char* keywords[] = {
    "int", "return", "if", "else", "while", "for", "char", "float", "double"
};

int num_keywords = sizeof(keywords) / sizeof(keywords[0]);

int is_keyword(char* str) {
    for (int i = 0; i < num_keywords; i++) {
        if (strcmp(str, keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

/*
struct Token* dfa(char* ch, struct Token* token) {
    int i = 0, j = 0, index = 0;
    char buf[20];
    memset(buf, 0, sizeof(buf));
    enum State state = START;
    while (ch[i] != '\0') {
        switch (state) {
        case START:
            if (isalpha(ch[i]) || ch[i] == '_') {
                state = IDENTIFIER;
                buf[j] = ch[i];
                j++;
            }
            if (isdigit(ch[i])) {
                state = NUMBER;
                buf[j] = ch[i];
                j++;
            }
            if (ispunct(ch[i])) {
                if (ch[i] == '+' || ch[i] == '-' || ch[i] == '*' || ch[i] == '/'||ch[i]=='=') {
                    state = OPERATOR;
                    buf[j] = ch[i];
                    j++;
                }
                else {
                    state = SEPARATOR;
                    buf[j] = ch[i];
                    j++;
                }
            }
            break;

        case IDENTIFIER:
            if (isalnum(ch[i]) || ch[i] == '_') {
                buf[j] = ch[i];
                if (is_keyword(buf)) {
                    token[index].type = TOKEN_KEYWORD;
                    strcpy(token[index].text, buf);
                    printf("TOKEN_KEYWORD:");
                    printf("%s\n", token[index].text);
                    memset(buf, 0, sizeof(buf));
                    j = 0;
                    index++;
                    state = START;
                }
                else {
                    j++;
                }
            }
            else {
                token[index].type = TOKEN_IDENTIFIER;
                strcpy(token[index].text, buf);
                printf("TOKEN_IDENTIFIER:");
                printf("%s\n", token[index].text);
                memset(buf, 0, sizeof(buf));
                j = 0;
                index++;
                i--;
                state = START;
            }
            break;

        case NUMBER:
            if (isdigit(ch[i])) {
                state = NUMBER;
                buf[j] = ch[i];
                j++;
            }
            else {
                token[index].type = TOKEN_NUMBER;
                strcpy(token[index].text, buf);
                printf("TOKEN_NUMBER:");
                printf("%s\n", token[index].text);
                memset(buf, 0, sizeof(buf));
                j = 0;
                index++;
                i--;
                state = START;
            }
            break;

        case SEPARATOR:
            token[index].type = TOKEN_SEPARATOR;
            strcpy(token[index].text, buf);
            printf("TOKEN_SEPARATOR:");
            printf("%s\n", token[index].text);
            memset(buf, 0, sizeof(buf));
            j = 0;
            index++;
            i--;
            state = START;
            break;

        case OPERATOR:
            token[index].type = TOKEN_OPERATOR;
            strcpy(token[index].text, buf);
            printf("TOKEN_OPERATOR:");
            printf("%s\n", token[index].text);
            memset(buf, 0, sizeof(buf));
            j = 0;
            index++;
            i--;
            state = START;
            break;

        default:
            break;
        }
        i++;
    }
    strcpy(token[index].text, buf);
    token[index].type = TOKEN_SEPARATOR;
    printf("TOKEN_SEPARATOR:");
    printf("%s\n", token[index].text);
    index++;
    strcpy(token[index].text, "end");
    token[index].type = TOKEN_EOF;

    return token;
}
*/
struct Token* dfa(char* ch, struct Token* token) {
    int i = 0, j = 0, index = 0;
    char buf[20];
    memset(buf, 0, sizeof(buf));
    enum State state = START;
    while (ch[i] != '\0') {
        switch (state) {
        case START:
            if (isalpha(ch[i]) || ch[i] == '_') {
                state = IDENTIFIER;
                buf[j] = ch[i];
                j++;
            }
            if (isdigit(ch[i])) {
                state = NUMBER;
                buf[j] = ch[i];
                j++;
            }
            if (ispunct(ch[i])) {
                if (ch[i] == '=' && ch[i + 1] == '=') { // 处理 "=="
                    state = OPERATOR;
                    buf[j] = ch[i];
                    j++;
                    i++; // 跳过下一个 '='
                    buf[j] = ch[i];
                    j++;
                }
                else if (ch[i] == '+' || ch[i] == '-' || ch[i] == '*' || ch[i] == '/' || ch[i] == '=') {
                    state = OPERATOR;
                    buf[j] = ch[i];
                    j++;
                }
                else {
                    state = SEPARATOR;
                    buf[j] = ch[i];
                    j++;
                }
            }
            break;

        case IDENTIFIER:
            if (isalnum(ch[i]) || ch[i] == '_') {
                buf[j] = ch[i];
                if (is_keyword(buf)) {
                    token[index].type = TOKEN_KEYWORD;
                    strcpy(token[index].text, buf);
                    printf("TOKEN_KEYWORD:");
                    printf("%s\n", token[index].text);
                    memset(buf, 0, sizeof(buf));
                    j = 0;
                    index++;
                    state = START;
                }
                else {
                    j++;
                }
            }
            else {
                token[index].type = TOKEN_IDENTIFIER;
                strcpy(token[index].text, buf);
                printf("TOKEN_IDENTIFIER:");
                printf("%s\n", token[index].text);
                memset(buf, 0, sizeof(buf));
                j = 0;
                index++;
                i--;
                state = START;
            }
            break;

        case NUMBER:
            if (isdigit(ch[i])) {
                state = NUMBER;
                buf[j] = ch[i];
                j++;
            }
            else {
                token[index].type = TOKEN_NUMBER;
                strcpy(token[index].text, buf);
                printf("TOKEN_NUMBER:");
                printf("%s\n", token[index].text);
                memset(buf, 0, sizeof(buf));
                j = 0;
                index++;
                i--;
                state = START;
            }
            break;

        case SEPARATOR:
            token[index].type = TOKEN_SEPARATOR;
            strcpy(token[index].text, buf);
            printf("TOKEN_SEPARATOR:");
            printf("%s\n", token[index].text);
            memset(buf, 0, sizeof(buf));
            j = 0;
            index++;
            i--;
            state = START;
            break;

        case OPERATOR:
            token[index].type = TOKEN_OPERATOR;
            strcpy(token[index].text, buf);
            printf("TOKEN_OPERATOR:");
            printf("%s\n", token[index].text);
            memset(buf, 0, sizeof(buf));
            j = 0;
            index++;
            i--;
            state = START;
            break;

        default:
            break;
        }
        i++;
    }
    strcpy(token[index].text, buf);
    token[index].type = TOKEN_SEPARATOR;
    printf("TOKEN_SEPARATOR:");
    printf("%s\n", token[index].text);
    index++;
    strcpy(token[index].text, "end");
    token[index].type = TOKEN_EOF;

    return token;
}
