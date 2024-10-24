#ifndef _DFA_H
#define _DFA_H

enum TokenType {
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_KEYWORD,
    TOKEN_OPERATOR,
    TOKEN_SEPARATOR,
    TOKEN_UNKNOWN,
    TOKEN_EOF  // ��ʾ�ļ���β
};


struct Token {
    enum TokenType type;
    char text[32];  // ���� token ���ı�
};

enum State {
    START,
    IDENTIFIER,
    NUMBER,
    OPERATOR,
    SEPARATOR
};

int is_keyword(char* str);
struct Token* dfa(char* ch, struct Token* token);

#endif