#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> 
#include <string.h>
#include "dfa.h"
#include "parse.h"


#define var_baseaddr 100//��������ַ
#define MAX_VARS 100//��������
#define MAX_INSTRUCTIONS 1024//���ָ����


struct Instruction instructionList[MAX_INSTRUCTIONS];// ȫ��ָ���б�
int instructionCount = 0;//ָ�����
int current_instruction_address = 0;//��ǰָ���ַ


int index = 0;//token����ֵ
int token_num=0;//token����

struct Token* p;//token�ݴ�ָ��

struct Variable symbolTable[MAX_VARS];// ���ű�
int varCount = 0;//��������

int addr = 0;

void getNextToken() {//��ȡ��һ��token
    if (p->type != TOKEN_EOF) {
        p++;
    }
    else {
        printf("analyze end!\n\n");
    }
}


// ƥ�䵱ǰ token������ȡ��һ�� token
void match(enum TokenType expectedType) {
    if (p->type == expectedType) {
        getNextToken();
    }
    else {
        printf("Syntax error: unexpected token %s\n", p->text);
        exit(1);
    }
}



// ר�ŵ�ָ�����ɺ���
void generateVmInstruction(const char* instruction, const char* operand,int flag) {
    if (flag) {
        strcpy(instructionList[instructionCount].instruction, instruction);  // ���Ʋ�����
        strcpy(instructionList[instructionCount].operand ,operand);       // ���ò�����
        instructionList[instructionCount].flag = 1;
        instructionCount++;
        current_instruction_address = current_instruction_address + 2;
    }
    else {
        strcpy(instructionList[instructionCount].instruction, instruction);  // ���Ʋ�����
        instructionList[instructionCount].flag = 0;
        instructionCount++;
        current_instruction_address++;
    }
}



void updateJumpAddress(int instructionIndex, int targetAddress) {
    // ��Ŀ���ַת��Ϊ�ַ�����ʽ����Ϊ���������ַ���
    char targetAddressStr[10];
    sprintf(targetAddressStr, "%d", targetAddress);

    // ����֮ǰ���ɵ���תָ��Ĳ�����
    strcpy(instructionList[instructionIndex-1].operand, targetAddressStr);
}



// ��ӱ��������ű�
void addVariable(const char* name) {
    if (varCount < MAX_VARS) {
        strcpy(symbolTable[varCount].name, name);
        symbolTable[varCount].address = varCount; // ʹ�ñ�����������Ϊ��ַ
        varCount++;
    }
    else {
        printf("Symbol table full!\n");
    }
}

// ���ұ�����ַ
int getVariableAddress(const char* name) {
    for (int i = 0; i < varCount; i++) {
        if (strcmp(symbolTable[i].name, name) == 0) {
            return symbolTable[i].address;
        }
    }
    printf("Error: variable '%s' not declared\n", name);
    return -1; // δ�ҵ�
}

//��ȡ��ǰָ���ڴ��ַ
int getCurrentInstructionAddress() {
    return current_instruction_address;
}

// ��ָ���б�������ļ�
void writeInstructionsToFile(const char* filename) {
    int addr = 0;
    FILE* file = fopen(filename, "w");  // ���ļ���дģʽ
    if (file == NULL) {
        printf("�޷����ļ� %s\n", filename);
        return;
    }


    // ����ָ���б���ӡ
    printf("instruction:\n");
    for (int i = 0; i < instructionCount; i++) {
        if (instructionList[i].flag) {
            printf("%d:%s %s\n", addr, instructionList[i].instruction, instructionList[i].operand);
            addr = addr + 2;
        }
        else {
            printf("%d:%s\n", addr, instructionList[i].instruction);  // �޲�����ָ��
            addr++;
        }
    }

    addr = 0;
    // ����ָ���б�д���ļ�
    for (int i = 0; i < instructionCount; i++) {
        if (instructionList[i].flag) {
            fprintf(file, "%s %s\n", instructionList[i].instruction, instructionList[i].operand);
        }
        else {
            fprintf(file, "%s\n",instructionList[i].instruction);  // �޲�����ָ��
        }
    }

    fclose(file);  // �ر��ļ�
    printf("ָ����д���ļ� %s\n", filename);
}



//�ķ���ʼ

// <Program> ::= <FunctionDeclaration> <Program> | ��
void parseProgram(struct Token* token) {

    p = token;

    while (p->type != TOKEN_EOF) {//ͳ��token����
        p++;
        token_num++;
    }
    printf("token_num:%d\n", token_num);

    p = token;
    while (p->type != TOKEN_EOF) {
        parseFunctionDeclaration(token);
        getNextToken();  // ȷ��������һ������
    }

    writeInstructionsToFile("test.txt");

}



// <FunctionDeclaration> ::= "int" <Identifier> "(" ")" "{" <StatementList> "}"
void parseFunctionDeclaration() {
    match(TOKEN_KEYWORD);  // ���� "int"
    match(TOKEN_IDENTIFIER);  // ����������
    match(TOKEN_SEPARATOR);  // ���� "("
    match(TOKEN_SEPARATOR);  // ���� ")"
    match(TOKEN_SEPARATOR);  // ���� "{"
    parseStatementList();  // ����������
    match(TOKEN_SEPARATOR);  // ���� "}"
    generateVmInstruction("HALT",0,0);

}

// <StatementList> ::= <Statement> <StatementList> | ��
void parseStatementList() {
    while (p->type != TOKEN_SEPARATOR || strcmp(p->text, "}") != 0) {
        parseStatement();
    }
}


// <Statement> ::= <DeclarationStatement> 
//               | <ExpressionStatement> 
//               | <ReturnStatement> 
//               | <IfStatement> 
//               | <WhileStatement>
// �����������
void parseStatement() {
     if (strcmp(p->text, "int")==0|| strcmp(p->text, "char")==0|| strcmp(p->text, "float") == 0|| strcmp(p->text, "double")==0) {
        parseDeclarationStatement();  // ���������������
    }
    else if (strcmp(p->text, "return") == 0) {
        parseReturnStatement();       // �����������
    }
    else if (strcmp(p->text, "if") == 0) {
        parseIfStatement();           // �����������
    }
    else if (strcmp(p->text, "while") == 0) {
        parseWhileStatement();        // ����ѭ�����
    }
    else if (p->type == TOKEN_IDENTIFIER) {
        parseExpressionStatement();   // �������ʽ���
    }
    else if (p->type == TOKEN_SEPARATOR) {
        
     }
    else {
        printf("Syntax error: unexpected statement\n");
        exit(1);
    }
}


// <IfStatement> ::= "if" "(" <Expression> ")" <Statement> ["else" <Statement>]
// �����������
void parseIfStatement() {
    int jmp1InstructionIndex=0;
    int jmp2InstructionIndex=0;
    int jzInstructionIndex=0;
    int jumpAddress=0;

    match(TOKEN_KEYWORD);      // ���� "if"
    match(TOKEN_SEPARATOR);    // ���� "("
    parseExpression();         // �����������ʽ

    // ����CMPָ����бȽ�
    generateVmInstruction("PUSH", "0", 1);  // ѹ�볣��0��ջ����Ϊ�Ƚϵĵڶ���������
    generateVmInstruction("CMP", NULL, 0);

    generateVmInstruction("JZ", "", 1);  // ��ʱ��֪����ת��ַ
    // ���� JZ ָ�����ס���ַ�Ա��Ժ����
    jzInstructionIndex = instructionCount;

    generateVmInstruction("JMP", "", 1);  // ��ʱ��֪����ת��ַ
    jmp1InstructionIndex = instructionCount;

    match(TOKEN_SEPARATOR);    // ���� ")"
    match(TOKEN_SEPARATOR);    // ���� "{"


    jumpAddress = getCurrentInstructionAddress();
    updateJumpAddress(jzInstructionIndex, jumpAddress);


    parseStatement();          // ���� if ���е����
    match(TOKEN_SEPARATOR);    // ���� "}"


    // �����ѡ�� else ��֧
    if (strcmp(p->text, "else") == 0) {

        generateVmInstruction("JMP", "", 1);  // ��ʱ��֪����ת��ַ
        jmp2InstructionIndex = instructionCount;

        jumpAddress = getCurrentInstructionAddress();
        updateJumpAddress(jmp1InstructionIndex, jumpAddress);

        getNextToken();        // ���� "else"
        match(TOKEN_SEPARATOR);    // ���� "{"
        parseStatement();      // ���� else ���е����
        match(TOKEN_SEPARATOR);    // ���� "}"
    }

    // �������� else �� JMP ָ��
    jumpAddress = getCurrentInstructionAddress();
    updateJumpAddress(jmp2InstructionIndex, jumpAddress);

}



// <WhileStatement> ::= "while" "(" <Expression> ")" <Statement>
// ����ѭ�����
void parseWhileStatement() {

    int jmp1InstructionIndex = 0;
    int jmp2InstructionIndex = 0;
    int jzInstructionIndex = 0;
    int jzAddress = 0;
    int jmp1Address = 0;
    int jmp2Address = 0;

    match(TOKEN_KEYWORD);      // ���� "while"
    match(TOKEN_SEPARATOR);    // ���� "("


    // ��¼��ǰָ���ַ�����������жϻ���
    jmp2Address = getCurrentInstructionAddress();


    parseExpression();         // �����������ʽ
    // ��ִ��CMP֮ǰ��ѹ�볣��0
    generateVmInstruction("PUSH", "0", 1);  // ѹ�볣��0��ջ����Ϊ�Ƚϵĵڶ���������
    generateVmInstruction("CMP", NULL, 0);

    match(TOKEN_SEPARATOR);    // ���� ")"

    // ����JZָ���������ѭ����
    generateVmInstruction("JZ", "", 1);  // ��ʱ��֪������λ�ã��Ժ����
    jzInstructionIndex = instructionCount;

    generateVmInstruction("JMP", "", 1);  // ��ʱ��֪������λ�ã��Ժ����
    jmp1InstructionIndex = instructionCount;


    match(TOKEN_SEPARATOR);    // ���� "{"

    jzAddress = getCurrentInstructionAddress();
    updateJumpAddress(jzInstructionIndex, jzAddress);

    parseStatement();          // ����ѭ�����е����
    match(TOKEN_SEPARATOR);    // ���� "}"


    generateVmInstruction("JMP", "", 1);  // ��ʱ��֪������λ�ã��Ժ����
    jmp2InstructionIndex = instructionCount;

    // ����JMP1��תλ��Ϊѭ���������ĵ�ַ
    updateJumpAddress(jmp2InstructionIndex, jmp2Address);

    jmp1Address = getCurrentInstructionAddress();
    // ����JMP1��תλ��Ϊѭ���������ĵ�ַ
    updateJumpAddress(jmp1InstructionIndex, jmp1Address);

}


void parseDeclarationStatement() {
    char addr_str[20];  // �ַ��������ڴ洢ת������ַ���
    match(TOKEN_KEYWORD);  // ���� "int"

    // �����һ������
    sprintf(addr_str, "%d", varCount + var_baseaddr);
    addVariable(p->text);
    match(TOKEN_IDENTIFIER);  // ����������

    if (strcmp(p->text, "=") == 0) {
        getNextToken();  // �����Ⱥ�
        // ���������ɸ�ֵ��ָ��������ʽ������ñ��ʽֻ����һ����ֵ��
        generateVmInstruction("PUSH", p->text, 1);  // ����ֵ��ֵѹջ
        getNextToken();  // ��ȡ��ֵ��ֵ
    }
    else {
        // ���û�и�ֵ����Ĭ�ϳ�ʼ��Ϊ 0
        generateVmInstruction("PUSH", "0", 1);  // ���轫������ʼ��Ϊ0��ѹջ
    }
    generateVmInstruction("STORE", addr_str, 1);  // �洢������ֵ

    // ����Ƿ���ڶ����������
    while (strcmp(p->text, ",") == 0) {
        getNextToken();  // ��������
        memset(addr_str, 0, sizeof(addr_str));
        sprintf(addr_str, "%d", varCount + var_baseaddr);
        addVariable(p->text);
        match(TOKEN_IDENTIFIER);  // ������һ��������

        if (strcmp(p->text, "=") == 0) {
            getNextToken();  // �����Ⱥ�
            // ���������ɸ�ֵ��ָ��
            generateVmInstruction("PUSH", p->text, 1);  // ����ֵ��ֵѹջ
            getNextToken();  // ��ȡ��ֵ��ֵ
        }
        else {
            generateVmInstruction("PUSH", "0", 1);  // Ĭ�ϳ�ʼ��Ϊ0
        }
        generateVmInstruction("STORE", addr_str, 1);  // �洢������ֵ
    }

    match(TOKEN_SEPARATOR);  // ���� ";"
}


// <ExpressionStatement> ::= <Identifier> "=" <Expression> ";"
void parseExpressionStatement() {
    int addr;
    char addr_str[20];  // �ַ��������ڴ洢ת������ַ���
    addr = getVariableAddress(p->text);

    sprintf(addr_str, "%d", addr + var_baseaddr);

    match(TOKEN_IDENTIFIER);  // ����������
    
    match(TOKEN_OPERATOR);  // ���� "="
    parseExpression();  // �������ʽ,������Ľ��ѹջ
 
    generateVmInstruction("STORE", addr_str,1);

    match(TOKEN_SEPARATOR);  // ���� ";"
}


// <ReturnStatement> ::= "return" <Expression> ";"
void parseReturnStatement() {
    match(TOKEN_KEYWORD);  // ���� "return"
    parseExpression();  // �������ʽ
    match(TOKEN_SEPARATOR);  // ���� ";"
}


// <Expression> ::= <RelationalExpression>
// �������ʽ����չΪ�ܹ������ϵ�����
void parseExpression() {    
    parseRelationalExpression();  // ������ϵ���ʽ
}


// <RelationalExpression> ::= <AdditiveExpression> | <AdditiveExpression> <RelationalOperator> <AdditiveExpression>
void parseRelationalExpression() {
    char operator[5];
    parseAdditiveExpression();  // �������ļӷ����ʽ

    // ����Ƿ���ڹ�ϵ�����
    while (strcmp(p->text, ">") == 0 || strcmp(p->text, "<") == 0 || strcmp(p->text, "==") == 0) {
        strcpy(operator, p->text);// ���浱ǰ�����
        getNextToken();  // ������ϵ�����
        parseAdditiveExpression();  // �����Ҳ�ļӷ����ʽ
        // ���ݲ�����������Ӧ�������ָ��

        if (strcmp(operator, ">") == 0) {
            // a > b���Ƚ� a �� b ��ֵѹ��ջ
            generateVmInstruction("CMP", NULL, 0); // �Ƚ� a �� b
            generateVmInstruction("PUSH", "1", 1); // ��� a > b��ѹ�� 1
            generateVmInstruction("CMP", NULL, 0);
        }
        else if (strcmp(operator, "<") == 0) {
            // a < b��ͬ������
            generateVmInstruction("CMP", NULL, 0); // �Ƚ� a �� b
            generateVmInstruction("PUSH", "-1", 1); // ��� a < b��ѹ�� -1
            generateVmInstruction("CMP", NULL, 0);
        }
        else if (strcmp(operator, "==") == 0) {
            // a == b
            generateVmInstruction("CMP", NULL, 0); // �Ƚ� a �� b
            generateVmInstruction("PUSH", "0", 1); // ��� a == b��ѹ�� 0
            generateVmInstruction("CMP", NULL, 0);
        }
    }
}


// <AdditiveExpression> ::= <Term> | <Term> "+" <AdditiveExpression> | <Term> "-" <AdditiveExpression>
// �����ӷ����ʽ
void parseAdditiveExpression() {
    struct Token temp;
    parseTerm();  // ������

    // ����Ƿ���ڼӷ�����������
    while (strcmp(p->text, "+") == 0 || strcmp(p->text, "-") == 0) {
        strcpy(temp.text , p->text);
        getNextToken();  // ���������
        parseTerm();     // �����Ҳ����
        if (strcmp(temp.text, "+") == 0) {
            generateVmInstruction("ADD", NULL,0);  // ���ɼӷ�ָ��
        }
        else {
            generateVmInstruction("SUB", NULL,0);  // ���ɼ���ָ��
        }
    }
}


// <Term> ::= <Factor> | <Factor> "*" <Term> | <Factor> "/" <Term>
void parseTerm() {
    struct Token temp;
    parseFactor();  // ��������
    while (strcmp(p->text, "*") == 0 || strcmp(p->text, "/") == 0) {
        strcpy(temp.text, p->text);
        getNextToken();  // ���������
        parseFactor();  // �����Ҳ������
        if (strcmp(temp.text, "*") == 0) {
            generateVmInstruction("MUL", NULL,0);  // ���ɳ˷�ָ��
        } else {
            generateVmInstruction("DIV", NULL,0);  // ���ɳ���ָ��
        }
    }
}

// <Factor> ::= <Identifier> | <Number> | "(" <Expression> ")"
void parseFactor() {
    int addr;
    char addr_str[20];  // �ַ��������ڴ洢ת������ַ���

    if (p->type == TOKEN_IDENTIFIER) {
        addr = getVariableAddress(p->text);
        sprintf(addr_str, "%d", addr + var_baseaddr);
        generateVmInstruction("LOAD", addr_str,1);  // ���ر���ֵ
        getNextToken();  //������ʶ��
    }
    else if(p->type == TOKEN_NUMBER){
        generateVmInstruction("PUSH", p->text,1);  // ������ѹջ
        getNextToken(); 
    }
    else if (strcmp(p->text, "(") == 0) {
        match(TOKEN_SEPARATOR);  // ���� "("
        parseExpression();  // �������ʽ
        match(TOKEN_SEPARATOR);  // ���� ")"
    }
    else {
        printf("Syntax error: unexpected factor\n");
        exit(1);
    }
}
//�ķ�����