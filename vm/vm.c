#include "vm.h"


void Debug_Vm(struct Vm* vm) {
    int i;
    printf("\n");
    printf("\n");
    printf("Debug:\n");
    printf("pc:%d\n", vm->pc);
    printf("sp:%d\n", vm->sp);
    printf("\n");
    printf("\n");
    printf("mem:\n");
    printf("\n");
    printf("code:\n");
    for (i = 0; i < vm->program_size; i++) {
        printf("mem[%d]: %d\n", i, vm->mem[i]);
    }
    printf("\n");
    printf("var:\n");
    for (i = 50; i < 150; i++) {
        printf("mem[%d]: %d\n", i, vm->mem[i]);
    }
    printf("\n");
    printf("\n");
    printf("stack:\n");
    for (i = 0; i < 20; i++) {
        printf("stack[%d]: %d\n", i, vm->stack[i]);
    }
}

void Init_Vm(struct Vm* vm) {
    vm->pc = 0;
    vm->sp = -1;
    vm->fp = -1;
    vm->state = 1;
}

void Push_Vm(int value, struct Vm* vm) {
    vm->sp = vm->sp + 1;
    vm->stack[vm->sp] = value;
}

int Pop_Vm(struct Vm* vm) {
    int temp;
    temp = vm->stack[vm->sp];
    vm->stack[vm->sp] = 0;
    vm->sp = vm->sp - 1;
    return temp;
}

void Run_Vm(struct Vm* vm) {
    int i;
    while (vm->state) {
        int opcode = vm->mem[vm->pc];      // 获取操作码
        int operand;

        switch (opcode) {
        case PUSH: {
            operand = vm->mem[vm->pc + 1]; // 获取第一个操作数
            if (vm->sp < 255) {
                vm->Push(operand, vm);  // 将操作数1压栈
                vm->pc += 2;
                printf("PUSH %d\n", operand);
            }
            else {
                printf("Stack OverFlow!\n");
                vm->Debug(vm);
                vm->state = 0;
            }
            break;
        }

        case POP: {
            if (vm->sp >= 0) {
                vm->reg1 = vm->Pop(vm);  // 弹栈
                vm->pc += 1;  // 
                printf("POP\n");
            }
            else {
                printf("Stack underflow\n");
                vm->state = 0;
                vm->Debug(vm);
                return -1;
            }
            break;
        }

        case ADD: {
            vm->reg2 = vm->Pop(vm);
            vm->reg1 = vm->Pop(vm);
            vm->Push(vm->reg1 + vm->reg2, vm);  // 将相加结果压栈
            vm->pc += 1;  // 
            printf("ADD\n");
            break;
        }

        case SUB: {
            vm->reg2 = vm->Pop(vm);
            vm->reg1 = vm->Pop(vm);
            vm->Push(vm->reg1 - vm->reg2, vm);  // 将相减结果压栈
            vm->pc += 1;  // 
            printf("SUB\n");
            break;
        }
        case MUL: {
            vm->reg2 = vm->Pop(vm);
            vm->reg1 = vm->Pop(vm);
            vm->Push(vm->reg1 * vm->reg2, vm);  // 将相乘结果压栈
            vm->pc += 1;  // 
            printf("MUL\n");
            break;
        }
        case DIV: {
            vm->reg1 = vm->Pop(vm);
            vm->reg2 = vm->Pop(vm);
            if (vm->reg2!=0) {
                vm->Push(vm->reg1 / vm->reg2, vm);  // 将相除结果压栈
                vm->pc += 1;  // 
                printf("DIV\n");
            }else {
                vm->state = 0;  // 停止虚拟机
                vm->Debug(vm);
                printf("Division by zero error\n");
            }
            break;
        }
        case JMP: {
            operand = vm->mem[vm->pc + 1]; // 获取第一个操作数
            vm->pc = operand;  // 无条件跳转到操作数1指定的地址
            printf("JMP %d\n",operand);
            break;
        }

        case JZ: {
            operand = vm->mem[vm->pc + 1]; // 获取第一个操作数
            vm->reg1 = vm->Pop(vm);  // 弹出栈顶条件
            if (vm->reg1 == 0) {
                vm->pc = operand;  // 条件为0时跳转到操作数1指定的地址
                printf("JZ %d\n", operand);

            }
            else {
                vm->pc += 2;
            }
            break;
        }

        case CMP: {
            vm->reg2 = vm->Pop(vm);
            vm->reg1 = vm->Pop(vm);

            if (vm->reg1 == vm->reg2) {
                vm->Push(0,vm);
            }else if(vm->reg1 > vm->reg2){
                vm->Push(1,vm);
            }
            else if (vm->reg1 < vm->reg2) {
                vm->Push(-1, vm);
            }
            vm->pc += 1;
            printf("CMP\n");
            break;
        }

        case NOP: {
            vm->pc += 1;
            printf("NOP\n");
            break;
        }

        case HALT: {
            vm->state = 0;  // 停止虚拟机
            printf("HALT\n");
            vm->Debug(vm);
            break;
        }

        case STORE: {
            operand = vm->mem[vm->pc + 1];
            vm->mem[operand] = vm->Pop(vm);
            vm->pc += 2;
            printf("STORE %d\n", operand);
            break;
        }

        case LOAD: {
            operand = vm->mem[vm->pc + 1];
            vm->Push(vm->mem[operand], vm);
            vm->pc += 2;
            printf("LOAD %d\n", operand);
            break;
        }
        
        case CALL: {
            operand = vm->mem[vm->pc + 1]; // 获取函数地址
            if (vm->sp < 255) {
                vm->Push(vm->fp, vm);      // 保存当前栈帧指针
                vm->Push(vm->pc + 2, vm);  // 将当前pc+2入栈以便返回
                vm->fp = vm->sp;
                vm->pc = operand;           // 跳转到函数地址
                printf("CALL %d\n", operand);
            }
            else {
                printf("Stack Overflow!\n");
                vm->state = 0;
                vm->Debug(vm);
            }
            break;
        }
        

        case RET: {
            if (vm->sp >= 0) {
                vm->sp = vm->fp;              // 清理当前栈帧的局部变量
                vm->pc = vm->Pop(vm);       // 从堆栈中取出返回地址
                vm->fp = vm->Pop(vm);         // 恢复上一个栈帧指针
                printf("RET\n");
            }
            else {
                printf("Stack underflow\n");
                vm->state = 0;
                vm->Debug(vm);
            }
            break;
        }
        
        case PUSH_SP: {
            Push_Vm(vm->sp, vm);
            vm->pc += 1;
            printf("PUSH_SP\n");
            break;
        }

        case PUSH_FP: {
            Push_Vm(vm->fp, vm);
            vm->pc += 1;
            printf("PUSH_FP\n");
            break;
        }

        case POP_SP: {
            vm->sp = Pop_Vm(vm);
            vm->pc += 1;
            printf("POP_SP\n");
            break;
        }

        case POP_FP: {
            vm->fp = Pop_Vm(vm);
            vm->pc += 1;
            printf("POP_FP\n");
            break;
        }

        default: {
            printf("Unknown opcode: %d\n", opcode);
            vm->Debug(vm);
            vm->state = 0;  // 遇到未知指令时停止
            break;
        }
        }
    }
}


// 从文件加载程序
void Load_Vm(struct Vm* vm, const char* filename) {
    FILE* file = fopen(filename, "r");
    int index = 0;
    char instruction[10];
    int operand;  // 获取操作数
    if (file == NULL) {
        printf("Failed to open file: %s\n", filename);
    }

    // 逐行读取并解析
    while (fscanf(file, "%s", instruction) != EOF) {
        // 使用strcmp处理不同的指令
        if (strcmp(instruction, "PUSH") == 0) {
            vm->mem[index++] = PUSH;
            fscanf(file, "%d", &operand); // 读取操作数
            vm->mem[index++] = operand;
        }
        else if (strcmp(instruction, "POP") == 0) {
            vm->mem[index++] = POP;
        }
        else if (strcmp(instruction, "ADD") == 0) {
            vm->mem[index++] = ADD;
        }
        else if (strcmp(instruction, "SUB") == 0) {
            vm->mem[index++] = SUB;
        }
        else if (strcmp(instruction, "MUL") == 0) {
            vm->mem[index++] = MUL;
        }
        else if (strcmp(instruction, "DIV") == 0) {
            vm->mem[index++] = DIV;
        }
        else if (strcmp(instruction, "CMP") == 0) {
            vm->mem[index++] = CMP;
        }
        else if (strcmp(instruction, "JMP") == 0) {
            vm->mem[index++] = JMP;
            fscanf(file, "%d", &operand); // 读取操作数
            vm->mem[index++] = operand;
        }
        else if (strcmp(instruction, "JZ") == 0) {
            vm->mem[index++] = JZ;
            fscanf(file, "%d", &operand); // 读取操作数
            vm->mem[index++] = operand;
        }
        else if (strcmp(instruction, "NOP") == 0) {
            vm->mem[index++] = NOP;
        }
        else if (strcmp(instruction, "STORE") == 0) {
            vm->mem[index++] = STORE;
            fscanf(file, "%d", &operand); // 读取操作数
            vm->mem[index++] = operand;
        }
        else if (strcmp(instruction, "LOAD") == 0) {
            vm->mem[index++] = LOAD;
            fscanf(file, "%d", &operand); // 读取操作数
            vm->mem[index++] = operand;
        }
        else if (strcmp(instruction, "CALL") == 0) {
            vm->mem[index++] = CALL;
            fscanf(file, "%d", &operand); // 读取操作数
            vm->mem[index++] = operand;
        }
        else if (strcmp(instruction, "RET") == 0) {
            vm->mem[index++] = RET;
        }
        else if (strcmp(instruction, "PUSH_SP") == 0) {
            vm->mem[index++] = PUSH_SP;
        }
        else if (strcmp(instruction, "PUSH_FP") == 0) {
            vm->mem[index++] = PUSH_FP;
        }
        else if (strcmp(instruction, "POP_SP") == 0) {
            vm->mem[index++] = POP_SP;
        }
        else if (strcmp(instruction, "POP_FP") == 0) {
            vm->mem[index++] = POP_FP;
        }
        else if (strcmp(instruction, "HALT") == 0) {
            vm->mem[index++] = HALT;
        }
        else {
            printf("Unknown instruction: %s\n", instruction);
        }
    }
    vm->program_size = index;
    fclose(file);
}
