#ifndef VM_H
#define VM_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define PUSH 1  //压栈
#define POP 2   //弹栈
#define ADD 3   //加法
#define SUB 4   //减法
#define MUL 5   //乘法
#define DIV 6   //除法
#define JMP 7   // 无条件跳转
#define JZ  8   // 条件跳转 (如果栈顶为0，跳转)
#define CMP 9   //比较
#define NOP 10  //空操作
#define STORE 11 //将栈顶数据存储到内存
#define LOAD  12 //从内存加载数据到栈
#define CALL 13  // CALL指令代码
#define RET 14   // RET指令代码
#define PUSH_SP 15
#define PUSH_FP 16
#define POP_SP 17
#define POP_FP 18
#define HALT 19  //停机

struct Vm {
    int stack[256];     // 虚拟机的堆栈
    int sp;             // 堆栈指针
    int fp;             //栈帧
    int pc;             // 程序计数器
    int mem[256];       // 指令
    int state;          //虚拟机运行状态
    int reg1;           //寄存器1
    int reg2;           //寄存器2
    int program_size;   //程序尺寸

    void (*Load)(struct Vm*, const char*); // 从文件加载程序
    void (*Init)(struct Vm*);//初始化虚拟机
    void (*Push)(int, struct Vm*);//压栈
    int (*Pop)(struct Vm*);//弹栈
    void (*Run)(struct Vm*);//运行
    void (*Debug)(struct Vm*);//Debug
};

void Debug_Vm(struct Vm* vm);
void Init_Vm(struct Vm* vm);
void Push_Vm(int value, struct Vm* vm);
int Pop_Vm(struct Vm* vm);
void Run_Vm(struct Vm* vm);
void Load_Vm(struct Vm* vm, const char* filename);// 从文件加载程序

#endif 
