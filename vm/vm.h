#ifndef VM_H
#define VM_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define PUSH 1  //ѹջ
#define POP 2   //��ջ
#define ADD 3   //�ӷ�
#define SUB 4   //����
#define MUL 5   //�˷�
#define DIV 6   //����
#define JMP 7   // ��������ת
#define JZ  8   // ������ת (���ջ��Ϊ0����ת)
#define CMP 9   //�Ƚ�
#define NOP 10  //�ղ���
#define STORE 11 //��ջ�����ݴ洢���ڴ�
#define LOAD  12 //���ڴ�������ݵ�ջ
#define CALL 13  // CALLָ�����
#define RET 14   // RETָ�����
#define PUSH_SP 15
#define PUSH_FP 16
#define POP_SP 17
#define POP_FP 18
#define HALT 19  //ͣ��

struct Vm {
    int stack[256];     // ������Ķ�ջ
    int sp;             // ��ջָ��
    int fp;             //ջ֡
    int pc;             // ���������
    int mem[256];       // ָ��
    int state;          //���������״̬
    int reg1;           //�Ĵ���1
    int reg2;           //�Ĵ���2
    int program_size;   //����ߴ�

    void (*Load)(struct Vm*, const char*); // ���ļ����س���
    void (*Init)(struct Vm*);//��ʼ�������
    void (*Push)(int, struct Vm*);//ѹջ
    int (*Pop)(struct Vm*);//��ջ
    void (*Run)(struct Vm*);//����
    void (*Debug)(struct Vm*);//Debug
};

void Debug_Vm(struct Vm* vm);
void Init_Vm(struct Vm* vm);
void Push_Vm(int value, struct Vm* vm);
int Pop_Vm(struct Vm* vm);
void Run_Vm(struct Vm* vm);
void Load_Vm(struct Vm* vm, const char* filename);// ���ļ����س���

#endif 
