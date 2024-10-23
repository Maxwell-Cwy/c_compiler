#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> 
#include <string.h>
#include "dfa.h"
#include "parse.h"

#define locvar_baseaddr 120//变量基地址
#define var_baseaddr 100//变量基地址
#define MAX_VARS 10//最大变量数
#define MAX_INSTRUCTIONS 1024//最大指令数
#define RETURN_VALUE "200"

struct Instruction instructionList[MAX_INSTRUCTIONS];// 全局指令列表
int instructionCount = 0;//指令个数
int current_instruction_address = 0;//当前指令地址

struct FunctionTableEntry functionTable[100];//函数符号表
int functionCount = 0;//函数个数

struct Variable symbolTable[MAX_VARS];// 变量符号表

int varCount = 0;//变量个数

int index = 0;//token索引值
int token_num=0;//token总数
struct Token* p;//token暂存指针


int jmpMainAddress = 0;//main函数地址
int jmpMainIndex = 0;//main函数指令索引


void getNextToken() {//获取下一个token
    if (p->type != TOKEN_EOF) {
        p++;
    }
    else {
        printf("analyze end!\n\n");
    }
}


// 匹配当前 token，并获取下一个 token
void match(enum TokenType expectedType) {
    if (p->type == expectedType) {
        getNextToken();
    }
    else {
        printf("Syntax error: unexpected token %s\n", p->text);
        exit(1);
    }
}



// 专门的指令生成函数
void generateVmInstruction(const char* instruction, const char* operand,int flag) {
    if (flag) {
        strcpy(instructionList[instructionCount].instruction, instruction);  // 复制操作码
        strcpy(instructionList[instructionCount].operand ,operand);       // 设置操作数
        instructionList[instructionCount].flag = 1;
        instructionCount++;
        current_instruction_address = current_instruction_address + 2;
    }
    else {
        strcpy(instructionList[instructionCount].instruction, instruction);  // 复制操作码
        instructionList[instructionCount].flag = 0;
        instructionCount++;
        current_instruction_address++;
    }
}



void updateJumpAddress(int instructionIndex, int targetAddress) {
    // 将目标地址转换为字符串形式，因为操作数是字符型
    char targetAddressStr[10];
    sprintf(targetAddressStr, "%d", targetAddress);

    // 更新之前生成的跳转指令的操作数
    strcpy(instructionList[instructionIndex-1].operand, targetAddressStr);
}


//获取当前指令内存地址
int getCurrentInstructionAddress() {
    return current_instruction_address;
}

// 将指令列表输出到文件
void writeInstructionsToFile(const char* filename) {
    int addr = 0;
    FILE* file = fopen(filename, "w");  // 打开文件以写模式
    if (file == NULL) {
        printf("无法打开文件 %s\n", filename);
        return;
    }


    // 遍历指令列表并打印
    printf("instruction:\n");
    printf("\n");
    for (int i = 0; i < instructionCount; i++) {
        if (instructionList[i].flag) {
            printf("%d:%s %s\n", addr, instructionList[i].instruction, instructionList[i].operand);
            addr = addr + 2;
        }
        else {
            printf("%d:%s\n", addr, instructionList[i].instruction);  // 无操作数指令
            addr++;
        }
    }

    addr = 0;
    // 遍历指令列表并写入文件
    for (int i = 0; i < instructionCount; i++) {
        if (instructionList[i].flag) {
            fprintf(file, "%s %s\n", instructionList[i].instruction, instructionList[i].operand);
        }
        else {
            fprintf(file, "%s\n",instructionList[i].instruction);  // 无操作数指令
        }
    }

    fclose(file);  // 关闭文件
    printf("指令已写入文件 %s\n", filename);
}


// 注册函数地址
void addFunction(char* name) {
    int address= getCurrentInstructionAddress();
    strcpy(functionTable[functionCount].name, name);
    functionTable[functionCount].address = address;
    functionCount++;

}

// 查找函数地址
int getFunctionAddress(char* name) {
    for (int i = 0; i < functionCount; i++) {
        if (strcmp(functionTable[i].name, name) == 0) {
            return functionTable[i].address;
        }
    }
    printf("Function %s not found.\n", name);
    exit(1);
}



// 添加变量到符号表
void addVariable(const char* name, int type) {
    if (varCount < MAX_VARS) {
        strcpy(symbolTable[varCount].name, name);
        symbolTable[varCount].scope = 0;
        symbolTable[varCount].address = varCount + var_baseaddr; // 使用变量的索引作为地址
        symbolTable[varCount].type = type;
        varCount++;
    }
    else {
        printf("Symbol table full!\n");
    }
}

// 查找变量地址
int getVariableAddress(const char* name) {
    for (int i = 0; i < varCount; i++) {
        if (strcmp(symbolTable[i].name, name) == 0) {
            return symbolTable[i].address;
        }
    }
    printf("Error: variable '%s' not declared\n", name);
    return -1; // 未找到
}


// 判断当前标记是否是赋值语句模式
int isNextTokenAssignment() {
    struct Token* originalToken = p;  // 保存当前标记指针

    // 预读下一个标记
    getNextToken();

    // 检查是否为赋值操作符 "="
    if (p->type == TOKEN_OPERATOR && strcmp(p->text, "=") == 0) {
        p = originalToken;  // 恢复标记指针
        return 1;        // 是赋值语句模式
    }

    p = originalToken;  // 恢复标记指针
    return 0;       // 不是赋值语句模式
}

int isNextTokenFunctionCall() {
    // 保存当前标记的状态，以便检查后恢复
    struct Token* originalToken = p;

    // 判断当前标记是否是标识符
    if (isNextTokenIdentifier()) {
        getNextToken();  // 前进到下一个标记

        // 检查下一个标记是否是左括号 "("
        if (p->type == TOKEN_SEPARATOR && strcmp(p->text, "(") == 0) {
            p = originalToken;  // 恢复原始标记状态
            return 1;  // 是函数调用
        }
    }

    // 如果不符合函数调用的格式，恢复标记状态并返回 false
    p = originalToken;
    return 0;
}

int isNextTokenIdentifier() {
    // 检查当前标记的类型是否是 TOKEN_IDENTIFIER
    return p->type == TOKEN_IDENTIFIER;
}


//文法开始
// <Program> ::= <FunctionDeclaration> <Program> | ε
void parseProgram(struct Token* token) {

    p = token;

    while (p->type != TOKEN_EOF) {//统计token个数
        p++;
        token_num++;
    }
    printf("token_num:%d\n", token_num);
    printf("\n");
    p = token;
    generateVmInstruction("JMP", "", 1);  // 暂时不知道跳转地址
    jmpMainIndex = instructionCount;
    while (p->type != TOKEN_EOF) {
        parseFunctionDeclaration(token);
    }
    generateVmInstruction("HALT", "", 1);  // 
    writeInstructionsToFile("test.txt");

}

// <FunctionDeclaration> ::= "int" <Identifier> "(" ")" "{" <StatementList> "}"
void parseFunctionDeclaration() {
    char addr_str[20];  // 暂存地址
    char fun_addr[20];  // 暂存函数名
    char var_addr[20];  //暂存变量名
    int loc_count = 0;

    match(TOKEN_KEYWORD);  // 期望 "int"


    if (strcmp(p->text, "main") == 0) {
        jmpMainAddress = getCurrentInstructionAddress();
        updateJumpAddress(jmpMainIndex, jmpMainAddress);
    }

    strcpy(fun_addr, p->text);
    addFunction(fun_addr);//缓存函数名
    match(TOKEN_IDENTIFIER);  // 期望函数名


    match(TOKEN_SEPARATOR);  // 期望 "("

    if (strcmp(p->text, ")") != 0) {

        match(TOKEN_KEYWORD);//期望 "int"

        memset(addr_str, 0, sizeof(addr_str));//清空
        sprintf(addr_str, "%d", locvar_baseaddr+loc_count);//暂时存储参数的位置
        generateVmInstruction("LOAD", addr_str, 1);  // 存储变量的值

        strcpy(var_addr, p->text);
        addVariable(var_addr, 1);
        match(TOKEN_IDENTIFIER);//期待变量名

        memset(addr_str, 0, sizeof(addr_str));//清空
        sprintf(addr_str, "%d", getVariableAddress(var_addr));
        generateVmInstruction("STORE", addr_str, 1);  // 存储变量的值

        while (strcmp(p->text, ",") == 0) {
            loc_count++;
            getNextToken();
            match(TOKEN_KEYWORD);//期望 "int"

            memset(addr_str, 0, sizeof(addr_str));//清空
            sprintf(addr_str, "%d", locvar_baseaddr + loc_count);
            generateVmInstruction("LOAD", addr_str, 1);  // 存储变量的值

            strcpy(var_addr, p->text);
            addVariable(var_addr, 1);
            match(TOKEN_IDENTIFIER);//期待变量名

            memset(addr_str, 0, sizeof(addr_str));
            sprintf(addr_str, "%d", getVariableAddress(var_addr));
            generateVmInstruction("STORE", addr_str, 1);  // 存储变量的值
        }

    }

    match(TOKEN_SEPARATOR);  // 期望 ")"
    match(TOKEN_SEPARATOR);  // 期望 "{"

    parseStatementList();  // 解析函数体
    match(TOKEN_SEPARATOR);  // 期望 "}"

    if (strcmp(fun_addr, "main") != 0) {
        generateVmInstruction("RET", " ", 0);
    }

}

// <StatementList> ::= <Statement> <StatementList> | ε
void parseStatementList() {//语句列表
    while (p->type != TOKEN_SEPARATOR || strcmp(p->text, "}") != 0) {
        parseStatement();
    }
}


// <Statement> ::= <DeclarationStatement> 
//               | <ExpressionStatement> 
//               | <ReturnStatement> 
//               | <IfStatement> 
//               | <WhileStatement>
// 解析单个语句
void parseStatement() {

    char addr_str[20];
     if (strcmp(p->text, "int")==0|| strcmp(p->text, "char")==0|| strcmp(p->text, "float") == 0|| strcmp(p->text, "double")==0) {
        parseDeclarationStatement();  // 解析变量声明语句
    }
    else if (strcmp(p->text, "return") == 0) {
        parseReturnStatement();       // 解析返回语句
    }
    else if (strcmp(p->text, "if") == 0) {
        parseIfStatement();           // 解析条件语句
    }
    else if (strcmp(p->text, "while") == 0) {
        parseWhileStatement();        // 解析循环语句
    }
    else if (p->type == TOKEN_IDENTIFIER) {
          parseExpressionStatement();   // 解析表达式语句
    }
    else if (p->type == TOKEN_SEPARATOR) {
        
     }
    else {
        printf("Syntax error: unexpected statement\n");
        exit(1);
    }
}


// <IfStatement> ::= "if" "(" <Expression> ")" <Statement> ["else" <Statement>]
// 解析条件语句
void parseIfStatement() {
    int jmp1InstructionIndex=0;
    int jmp2InstructionIndex=0;
    int jzInstructionIndex=0;
    int jumpAddress=0;

    match(TOKEN_KEYWORD);      // 期望 "if"
    match(TOKEN_SEPARATOR);    // 期望 "("
    parseExpression();         // 解析条件表达式

    // 生成CMP指令进行比较
    generateVmInstruction("PUSH", "0", 1);  // 压入常量0到栈中作为比较的第二个操作数
    generateVmInstruction("CMP", NULL, 0);

    generateVmInstruction("JZ", "", 1);  // 暂时不知道跳转地址
    // 生成 JZ 指令，并记住其地址以便稍后更新
    jzInstructionIndex = instructionCount;

    generateVmInstruction("JMP", "", 1);  // 暂时不知道跳转地址
    jmp1InstructionIndex = instructionCount;

    match(TOKEN_SEPARATOR);    // 期望 ")"
    match(TOKEN_SEPARATOR);    // 期望 "{"


    jumpAddress = getCurrentInstructionAddress();
    updateJumpAddress(jzInstructionIndex, jumpAddress);


    parseStatement();          // 解析 if 体中的语句
    match(TOKEN_SEPARATOR);    // 期望 "}"


    // 处理可选的 else 分支
    if (strcmp(p->text, "else") == 0) {

        generateVmInstruction("JMP", "", 1);  // 暂时不知道跳转地址
        jmp2InstructionIndex = instructionCount;

        jumpAddress = getCurrentInstructionAddress();
        updateJumpAddress(jmp1InstructionIndex, jumpAddress);

        getNextToken();        // 跳过 "else"
        match(TOKEN_SEPARATOR);    // 期望 "{"
        parseStatement();      // 解析 else 体中的语句
        match(TOKEN_SEPARATOR);    // 期望 "}"
    }

    // 更新跳过 else 的 JMP 指令
    jumpAddress = getCurrentInstructionAddress();
    updateJumpAddress(jmp2InstructionIndex, jumpAddress);

}



// <WhileStatement> ::= "while" "(" <Expression> ")" <Statement>
// 解析循环语句
void parseWhileStatement() {

    int jmp1InstructionIndex = 0;
    int jmp2InstructionIndex = 0;
    int jzInstructionIndex = 0;
    int jzAddress = 0;
    int jmp1Address = 0;
    int jmp2Address = 0;

    match(TOKEN_KEYWORD);      // 期望 "while"
    match(TOKEN_SEPARATOR);    // 期望 "("


    // 记录当前指令地址，用于条件判断回跳
    jmp2Address = getCurrentInstructionAddress();


    parseExpression();         // 解析条件表达式
    // 在执行CMP之前，压入常量0
    generateVmInstruction("PUSH", "0", 1);  // 压入常量0到栈中作为比较的第二个操作数
    generateVmInstruction("CMP", NULL, 0);

    match(TOKEN_SEPARATOR);    // 期望 ")"

    // 生成JZ指令，用于跳出循环体
    generateVmInstruction("JZ", "", 1);  // 暂时不知道跳出位置，稍后更新
    jzInstructionIndex = instructionCount;

    generateVmInstruction("JMP", "", 1);  // 暂时不知道跳出位置，稍后更新
    jmp1InstructionIndex = instructionCount;


    match(TOKEN_SEPARATOR);    // 期望 "{"

    jzAddress = getCurrentInstructionAddress();
    updateJumpAddress(jzInstructionIndex, jzAddress);

    parseStatement();          // 解析循环体中的语句
    match(TOKEN_SEPARATOR);    // 期望 "}"


    generateVmInstruction("JMP", "", 1);  // 暂时不知道跳出位置，稍后更新
    jmp2InstructionIndex = instructionCount;

    // 更新JMP1跳转位置为循环体结束后的地址
    updateJumpAddress(jmp2InstructionIndex, jmp2Address);

    jmp1Address = getCurrentInstructionAddress();
    // 更新JMP1跳转位置为循环体结束后的地址
    updateJumpAddress(jmp1InstructionIndex, jmp1Address);

}

//定义语句
void parseDeclarationStatement() {
    char var_addr[20];
    char addr_str[20];  // 字符数组用于存储转换后的字符串

    match(TOKEN_KEYWORD);  // 期望 "int"

    strcpy(var_addr,p->text);//缓存变量名
    addVariable(var_addr, 1);
    sprintf(addr_str, "%d", getVariableAddress(var_addr));
   
    match(TOKEN_IDENTIFIER);  // 期望变量名

    if (strcmp(p->text, "=") == 0) {
        getNextToken();  // 跳过等号
        // 解析并生成赋值的指令（解析表达式，假设该表达式只返回一个数值）
        generateVmInstruction("PUSH", p->text, 1);  // 将赋值的值压栈
        getNextToken();  // 读取赋值的值
    }
    else {
        // 如果没有赋值，则默认初始化为 0
        generateVmInstruction("PUSH", "0", 1);  // 假设将变量初始化为0并压栈
    }
    generateVmInstruction("STORE", addr_str, 1);  // 存储变量的值

    // 检查是否存在多个变量声明
    while (strcmp(p->text, ",") == 0) {
        getNextToken();  // 跳过逗号
        memset(addr_str, 0, sizeof(addr_str));
        addVariable(p->text, 1);
        sprintf(addr_str, "%d",getVariableAddress(p->text));

        match(TOKEN_IDENTIFIER);  // 期望下一个变量名

        if (strcmp(p->text, "=") == 0) {
            getNextToken();  // 跳过等号
            // 解析并生成赋值的指令
            generateVmInstruction("PUSH", p->text, 1);  // 将赋值的值压栈
            getNextToken();  // 读取赋值的值
        }
        else {
            generateVmInstruction("PUSH", "0", 1);  // 默认初始化为0
        }
        generateVmInstruction("STORE", addr_str, 1);  // 存储变量的值
    }

    match(TOKEN_SEPARATOR);  // 期望 ";"
}



// <ReturnStatement> ::= "return" <Expression> ";"
//返回语句
void parseReturnStatement() {
    match(TOKEN_KEYWORD);  // 期望 "return"
    parseExpression();  // 解析表达式
    generateVmInstruction("STORE", RETURN_VALUE, 1);  // 存储变量的值
    match(TOKEN_SEPARATOR);  // 期望 ";"
}


// <ExpressionStatement> ::= <AssignmentStatement> | <FunctionCall> ";"
// 表达式语句
//判断语句是赋值还是函数调用语句
//判断 c=((a+b)||fun(a,b))||fun(a,b)
void parseExpressionStatement() {
    if (isNextTokenAssignment()) {  // 判断是否存在赋值
        parseAssignmentStatement();
    }
    else {
        parseFunctionCall();//判断是否是函数调用

    }
    match(TOKEN_SEPARATOR);  // 期望 ";"
}

// <AssignmentStatement> ::= <Identifier> "=" (<Expression> | <FunctionCall>)
// 处理赋值语句
// c=((a+b)||fun(a,b))
void parseAssignmentStatement() {
    char var_str[20];
    char addr_str[20];
    strcpy(var_str,p->text);
    match(TOKEN_IDENTIFIER);  // 期望变量名
    sprintf(addr_str, "%d", getVariableAddress(var_str)); //转换变量地址为字符串形式

    match(TOKEN_OPERATOR);    // 期望 "="

    if (isNextTokenFunctionCall()) {  // 判断赋值语句右侧是否是函数调用
        parseFunctionCall();  // 解析函数调用
    }
    else {
        parseExpression();    // 解析表达式
    }
    generateVmInstruction("STORE", addr_str,1);
}

// <FunctionCall> ::= <Identifier> "(" <ArgumentList> ")"
//参数列表
void parseFunctionCall() {
    char addr_str[20];
    char fun_str[20];

    strcpy(fun_str,p->text);
    match(TOKEN_IDENTIFIER);  // 期望函数名

    match(TOKEN_SEPARATOR);   // 期望 "("
    parseArgumentList();      // 解析参数列表
    match(TOKEN_SEPARATOR);   // 期望 ")"

    // 获取函数地址并生成 CALL 指令
    memset(addr_str, 0, sizeof(addr_str));
    sprintf(addr_str, "%d", getFunctionAddress(fun_str));
    generateVmInstruction("CALL", addr_str, 1);
    generateVmInstruction("LOAD", RETURN_VALUE, 1);  // 返回值
}

// <ArgumentList> ::= <Expression> ("," <Expression>)*
//参数列表
void parseArgumentList() {
    int loc_count = 0;
    char addr_str[20];
    if (strcmp(p->text, ")") != 0) {  // 检查是否为空参数列表
        parseExpression();  // 解析第一个参数
        while (strcmp(p->text, ",") == 0) {  // 检查是否有更多参数
            match(TOKEN_SEPARATOR);  // 匹配 ","
            parseExpression();  // 解析下一个参数
            loc_count++;
        }
        for (int i = 0; i < loc_count+1; i++) {
            memset(addr_str, 0, sizeof(addr_str));
            sprintf(addr_str, "%d", locvar_baseaddr+i);
            generateVmInstruction("STORE", addr_str, 1);
        }
    }
}

// <Expression> ::= <RelationalExpression>
// 解析表达式，扩展为能够处理关系运算符
void parseExpression() {    
    parseRelationalExpression();  // 解析关系表达式
}


// <RelationalExpression> ::= <AdditiveExpression> | <AdditiveExpression> <RelationalOperator> <AdditiveExpression>
//关系表达式
void parseRelationalExpression() {
    char operator[5];
    parseAdditiveExpression();  // 解析左侧的加法表达式

    // 检查是否存在关系运算符
    while (strcmp(p->text, ">") == 0 || strcmp(p->text, "<") == 0 || strcmp(p->text, "==") == 0) {
        strcpy(operator, p->text);// 保存当前运算符
        getNextToken();  // 跳过关系运算符
        parseAdditiveExpression();  // 解析右侧的加法表达式
        // 根据操作符生成相应的虚拟机指令

        if (strcmp(operator, ">") == 0) {
            // a > b，先将 a 和 b 的值压入栈
            generateVmInstruction("CMP", NULL, 0); // 比较 a 和 b
            generateVmInstruction("PUSH", "1", 1); // 如果 a > b，压入 1
            generateVmInstruction("CMP", NULL, 0);
        }
        else if (strcmp(operator, "<") == 0) {
            // a < b，同样处理
            generateVmInstruction("CMP", NULL, 0); // 比较 a 和 b
            generateVmInstruction("PUSH", "-1", 1); // 如果 a < b，压入 -1
            generateVmInstruction("CMP", NULL, 0);
        }
        else if (strcmp(operator, "==") == 0) {
            // a == b
            generateVmInstruction("CMP", NULL, 0); // 比较 a 和 b
            generateVmInstruction("PUSH", "0", 1); // 如果 a == b，压入 0
            generateVmInstruction("CMP", NULL, 0);
        }
    }
}

// <AdditiveExpression> ::= <Term> | <Term> "+" <AdditiveExpression> | <Term> "-" <AdditiveExpression>
// 解析加法表达式 (a+b)||(a-b)
void parseAdditiveExpression() {
    struct Token temp;
    parseTerm();  // 解析项

    // 检查是否存在加法或减法运算符
    while (strcmp(p->text, "+") == 0 || strcmp(p->text, "-") == 0) {
        strcpy(temp.text , p->text);
        getNextToken();  // 跳过运算符
        parseTerm();     // 解析右侧的项
        if (strcmp(temp.text, "+") == 0) {
            generateVmInstruction("ADD", NULL,0);  // 生成加法指令
        }
        else {
            generateVmInstruction("SUB", NULL,0);  // 生成减法指令
        }
    }
}


// <Term> ::= <Factor> | <Factor> "*" <Term> | <Factor> "/" <Term>
//除法
void parseTerm() {
    struct Token temp;
    parseFactor();  // 解析因子
    while (strcmp(p->text, "*") == 0 || strcmp(p->text, "/") == 0) {
        strcpy(temp.text, p->text);
        getNextToken();  // 跳过运算符
        parseFactor();  // 解析右侧的因子
        if (strcmp(temp.text, "*") == 0) {
            generateVmInstruction("MUL", NULL,0);  // 生成乘法指令
        } else {
            generateVmInstruction("DIV", NULL,0);  // 生成除法指令
        }
    }
}

// <Factor> ::= <Identifier> | <Number> | "(" <Expression> ")"
//项因子
void parseFactor() {
    int addr;
    char var_str[20];
    char addr_str[20];  // 字符数组用于存储转换后的字符串

    if (p->type == TOKEN_IDENTIFIER) {
        strcpy(var_str,p->text);
        sprintf(addr_str, "%d", getVariableAddress(var_str));
        generateVmInstruction("LOAD", addr_str,1);  // 加载变量值
        getNextToken();  //解析标识符
    }
    else if(p->type == TOKEN_NUMBER){
        generateVmInstruction("PUSH", p->text,1);  // 将数字压栈
        getNextToken(); 
    }
    else if (strcmp(p->text, "(") == 0) {
        match(TOKEN_SEPARATOR);  // 期望 "("
        parseExpression();  // 解析表达式
        match(TOKEN_SEPARATOR);  // 期望 ")"
    }
    else {
        printf("Syntax error: unexpected factor\n");
        exit(1);
    }
}
//文法结束