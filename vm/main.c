#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vm.h"


int main(int argc, char* argv[]) {

    struct Vm vm = {
        .Init = *(Init_Vm),
        .Pop = *(Pop_Vm),
        .Push = *(Push_Vm),
        .Run = *(Run_Vm),
        .Load = *(Load_Vm),
        .Debug = *(Debug_Vm),
    };

    vm.Init(&vm);

    if (argc != 2) { //判断命令行参数个数
        printf("Usage: %s <program_file>\n", argv[0]);
        return 1;
    }

    printf("vm:\n");
    vm.Load(&vm, argv[1]);  // 从文件加载程序
    vm.Run(&vm);

}