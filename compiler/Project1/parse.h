#ifndef _PARSE_H
#define _PARSE_H


// 变量结构体
struct Variable {
    char name[32];//变量名
    int type;//类型
    int address;//变量地址
    int scope; // 变量作用域
};

// 存储函数地址和名称
struct FunctionTableEntry {
    char name[20];
    int address;
    int scope;
};

//指令
struct Instruction {
    char instruction[10];      // 操作码
    char operand[10];     // 操作数
    int flag;
} ;



//文法

//获取下一个token
void getNextToken();

// 匹配当前 token，并获取下一个 token
void match(enum TokenType expectedType);

// <Program> ::= <FunctionDeclaration> <Program> | ε
//程序定义
void parseProgram(struct Token* token);

// <FunctionDeclaration> ::= "int" <Identifier> "(" ")" "{" <StatementList> "}"
//函数定义
void parseFunctionDeclaration();

// <StatementList> ::= <Statement> <StatementList> | ε
void parseStatementList();

// <Statement> ::= <DeclarationStatement> | <ExpressionStatement> | <ReturnStatement>
void parseStatement();

// <DeclarationStatement> ::= "int" <Identifier> ";"
//变量定义
void parseDeclarationStatement();



// <WhileStatement> ::= "while" "(" <Expression> ")" <Statement>
// 解析循环语句
void parseWhileStatement();

// <IfStatement> ::= "if" "(" <Expression> ")" <Statement> ["else" <Statement>]
// 解析条件语句
void parseIfStatement();

// <ReturnStatement> ::= "return" <Expression> ";"
//返回语句
void parseReturnStatement();



// <ExpressionStatement> ::= <Identifier> "=" <Expression> ";"
//表达式状态
void parseExpressionStatement();

// <AssignmentStatement> ::= <Identifier> "=" (<Expression> | <FunctionCall>)
// c=((a+b)||fun(a,b))
void parseAssignmentStatement();

void parseArgumentList();

void parseFunctionCall();

// <Expression> ::= <RelationalExpression>
// 解析表达式，扩展为能够处理关系运算符
void parseExpression();

// <RelationalExpression> ::= <AdditiveExpression> | <AdditiveExpression> <RelationalOperator> <AdditiveExpression>
// 解析关系表达式，支持比较运算符
void parseRelationalExpression();

// <AdditiveExpression> ::= <Term> | <Term> "+" <AdditiveExpression> | <Term> "-" <AdditiveExpression>
// 解析加法表达式
void parseAdditiveExpression();

// <Term> ::= <Factor> | <Factor> "*" <Term> | <Factor> "/" <Term>
// 解析项
void parseTerm();

// <Factor> ::= <Identifier> | <Number> | "(" <Expression> ")"
// 解析因子
void parseFactor();



//文法






//语义动作

// 添加变量到符号表
void addVariable(const char* name, int type);

// 查找变量地址
int getVariableAddress(const char* name);

//更新变量地址
void updateJumpAddress(int instructionIndex, int targetAddress);

//获取当前指令内存地址
int getCurrentInstructionAddress();

// 将指令列表输出到文件
void writeInstructionsToFile(const char* filename);

// 专门的指令生成函数
void generateVmInstruction(const char* instruction, const char* operand, int flag);

// 注册函数地址
void addFunction(char* name);

// 查找函数地址
int getFunctionAddress(char* name);

int isNextTokenAssignment();

int isNextTokenFunctionCall();

int isNextTokenIdentifier();
//语义动作



#endif
