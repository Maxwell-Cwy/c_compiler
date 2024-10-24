#ifndef _PARSE_H
#define _PARSE_H


// �����ṹ��
struct Variable {
    char name[32];
    int address;
};

struct Instruction {
    char instruction[10];      // ������
    char operand[10];     // ������
    int flag;
} ;



//cfg�ķ�

//��ȡ��һ��token
void getNextToken();

// ƥ�䵱ǰ token������ȡ��һ�� token
void match(enum TokenType expectedType);

// <Program> ::= <FunctionDeclaration> <Program> | ��
//������
void parseProgram(struct Token* token);

// <FunctionDeclaration> ::= "int" <Identifier> "(" ")" "{" <StatementList> "}"
//��������
void parseFunctionDeclaration();

// <StatementList> ::= <Statement> <StatementList> | ��
void parseStatementList();

// <Statement> ::= <DeclarationStatement> | <ExpressionStatement> | <ReturnStatement>
void parseStatement();

// <DeclarationStatement> ::= "int" <Identifier> ";"
//��������
void parseDeclarationStatement();

// <ExpressionStatement> ::= <Identifier> "=" <Expression> ";"
//���ʽ״̬
void parseExpressionStatement();

// <ReturnStatement> ::= "return" <Expression> ";"
//����״̬
void parseReturnStatement();

// <Expression> ::= <RelationalExpression>
// �������ʽ����չΪ�ܹ������ϵ�����
void parseExpression();

// <RelationalExpression> ::= <AdditiveExpression> | <AdditiveExpression> <RelationalOperator> <AdditiveExpression>
// ������ϵ���ʽ��֧�ֱȽ������
void parseRelationalExpression();

// <AdditiveExpression> ::= <Term> | <Term> "+" <AdditiveExpression> | <Term> "-" <AdditiveExpression>
// �����ӷ����ʽ
void parseAdditiveExpression();



// <Term> ::= <Factor> | <Factor> "*" <Term> | <Factor> "/" <Term>
// ������
void parseTerm();

// <Factor> ::= <Identifier> | <Number> | "(" <Expression> ")"
// ��������
void parseFactor();

// <WhileStatement> ::= "while" "(" <Expression> ")" <Statement>
// ����ѭ�����
void parseWhileStatement();

// <IfStatement> ::= "if" "(" <Expression> ")" <Statement> ["else" <Statement>]
// �����������
void parseIfStatement();

//�ķ�



// ��ӱ��������ű�
void addVariable(const char* name);

// ���ұ�����ַ
int getVariableAddress(const char* name);

void updateJumpAddress(int instructionIndex, int targetAddress);

void updateJumpAddress(int instructionIndex, int targetAddress);

//��ȡ��ǰָ���ڴ��ַ
int getCurrentInstructionAddress();

// ��ָ���б�������ļ�
void writeInstructionsToFile(const char* filename);


// ר�ŵ�ָ�����ɺ���
void generateVmInstruction(const char* instruction, const char* operand, int flag);

#endif
