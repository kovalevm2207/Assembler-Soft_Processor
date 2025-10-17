#include "commands.h"


void push(SPU* spu) {
    stack_t data = spu->code[++(spu->PC)];
    StackPush(&spu->stk, data);

    #ifdef DUMP
        ProcessorDump(spu);
        getchar();
    #endif
}


void add(SPU* spu) {
    stack_t a = 0, b = 0;
    StackPop(&spu->stk, &a);
    StackPop(&spu->stk, &b);

    stack_t c = a + b;
    StackPush(&spu->stk, c);

    #ifdef DUMP
        ProcessorDump(spu);
        getchar();
    #endif
}


void sub(SPU* spu) {
    stack_t a = 0, b = 0;
    StackPop(&spu->stk, &a);
    StackPop(&spu->stk, &b);

    stack_t c = b - a;
    StackPush(&spu->stk, c);

    #ifdef DUMP
        ProcessorDump(spu);
        getchar();
    #endif
}


void mod(SPU* spu) {
    stack_t a = 0, b = 0;
    StackPop(&spu->stk, &a);
    StackPop(&spu->stk, &b);

    stack_t c = b % a;
    StackPush(&spu->stk, c);

    #ifdef DUMP
        ProcessorDump(spu);
        getchar();
    #endif
}


void div(SPU* spu) {
    stack_t a = 0, b = 0;
    StackPop(&spu->stk, &a);
    StackPop(&spu->stk, &b);

    stack_t c = b / a;
    StackPush(&spu->stk, c);

    #ifdef DUMP
        ProcessorDump(spu);
        getchar();
    #endif
}


void out(SPU* spu) {
    stack_t data = 0;
    StackPop(&spu->stk, &data);

    printf(CHANGE_ON BLUE TEXT_COLOR "answer = %10d\n" RESET, data);

    #ifdef DUMP
        ProcessorDump(spu);
        getchar();
    #endif
}


void mul(SPU* spu) {
    stack_t a = 0, b = 0;
    StackPop(&spu->stk, &a);
    StackPop(&spu->stk, &b);

    stack_t c = a * b;
    StackPush(&spu->stk, c);

    #ifdef DUMP
        ProcessorDump(spu);
        getchar();
    #endif
}


void pow(SPU* spu) {
    stack_t a = 0, n = spu->code[++(spu->PC)];
    StackPop(&spu->stk, &a);

    stack_t c = pow(a,n);
    StackPush(&spu->stk, c);

    #ifdef DUMP
        ProcessorDump(spu);
        getchar();
    #endif
}


void my_sqrt(SPU* spu) {
    stack_t a = 0;
    StackPop(&spu->stk, &a);

    stack_t b = sqrt(a);
    StackPush(&spu->stk, b);

    #ifdef DUMP
        ProcessorDump(spu);
        getchar();
    #endif
}


void reset_stk(SPU* spu) {
    StackDtor(&spu->stk);
    StackCtor(&spu->stk, CAPACITY);

    #ifdef DUMP
        ProcessorDump(spu);
        getchar();
    #endif
}


void pushreg(SPU* spu) {
    reg_t reg = (reg_t) spu->code[++(spu->PC)];

    StackPush(&spu->stk, spu->regs[reg]);

    #ifdef DUMP
        ProcessorDump(spu);
        getchar();
    #endif
}


void popreg(SPU* spu) {
    reg_t reg = (reg_t) spu->code[++(spu->PC)];

    StackPop(&spu->stk, &spu->regs[reg]);

    #ifdef DUMP
        ProcessorDump(spu);
        getchar();
    #endif
}


void call(SPU* spu) {
    StackPush(&spu->labels, spu->PC);
    int new_pc = spu->code[++(spu->PC)];
    spu->PC = new_pc - 1;

    #ifdef DUMP
        ProcessorDump(spu);
        getchar();
    #endif
}


void ret(SPU* spu) {
    stack_t new_pc = 0;
    StackPop(&spu->labels, &new_pc);
    spu->PC = new_pc + 1;

    #ifdef DUMP
        ProcessorDump(spu);
        getchar();
    #endif
}


void jmp(SPU* spu) {
    int new_pc = spu->code[++(spu->PC)];
    spu->PC = new_pc - 1;

    #ifdef DUMP
        ProcessorDump(spu);
        getchar();
    #endif
}


void jb(SPU* spu) {
    stack_t a = 0, b = 0;
    StackPop(&spu->stk, &b);
    StackPop(&spu->stk, &a);

    if (a < b) {
        int new_pc = spu->code[++(spu->PC)];
        spu->PC = new_pc - 1;
    } else (spu->PC)++;

    #ifdef DUMP
        ProcessorDump(spu);
        getchar();
    #endif
}


void jbe(SPU* spu) {
    stack_t a = 0, b = 0;
    StackPop(&spu->stk, &b);
    StackPop(&spu->stk, &a);

    if (a <= b) {
        int new_pc = spu->code[++(spu->PC)];
        spu->PC = new_pc - 1;
    } else (spu->PC)++;

    #ifdef DUMP
        ProcessorDump(spu);
        getchar();
    #endif
}


void ja(SPU* spu) {
    stack_t a = 0, b = 0;
    StackPop(&spu->stk, &b);
    StackPop(&spu->stk, &a);

    if (a > b) {
        int new_pc = spu->code[++(spu->PC)];
        spu->PC = new_pc - 1;
    } else (spu->PC)++;

    #ifdef DUMP
        ProcessorDump(spu);
        getchar();
    #endif
}


void jae(SPU* spu) {
    stack_t a = 0, b = 0;
    StackPop(&spu->stk, &b);
    StackPop(&spu->stk, &a);

    if (a >= b) {
        int new_pc = spu->code[++(spu->PC)];
        spu->PC = new_pc - 1;
    } else (spu->PC)++;

    #ifdef DUMP
        ProcessorDump(spu);
        getchar();
    #endif
}


void je(SPU* spu) {
    stack_t a = 0, b = 0;
    StackPop(&spu->stk, &b);
    StackPop(&spu->stk, &a);

    if (a == b) {
        int new_pc = spu->code[++(spu->PC)];
        spu->PC = new_pc - 1;
    } else (spu->PC)++;

    #ifdef DUMP
        ProcessorDump(spu);
        getchar();
    #endif
}


void jne(SPU* spu) {
    stack_t a = 0, b = 0;
    StackPop(&spu->stk, &b);
    StackPop(&spu->stk, &a);

    if (a != b) {
        int new_pc = spu->code[++(spu->PC)];
        spu->PC = new_pc - 1;
    } else (spu->PC)++;

    #ifdef DUMP
        ProcessorDump(spu);
        getchar();
    #endif
}


void in(SPU* spu) {
    int a = 0;
    printf("Write integer: ");
    scanf("%d", &a);

    StackPush(&spu->stk, a);

    #ifdef DUMP
        ProcessorDump(spu);
        getchar();
    #endif
}


void popm(SPU* spu) {
    stack_t data = 0;
    StackPop(&spu->stk, &data);
    int mem_i = spu->regs[spu->code[++(spu->PC)]];
    spu->RAM[mem_i] = data;

    #ifdef DUMP
        ProcessorDump(spu);
        getchar();
    #endif
}


void pushm(SPU* spu) {
    int mem_i = spu->code[++(spu->PC)];
    StackPush(&spu->stk, spu->RAM[mem_i]);

    #ifdef DUMP
        ProcessorDump(spu);
        getchar();
    #endif
}


void draw(SPU* spu) {
    system ("clear");
    for (int i = 0; i < 4141; i++) {
        if (spu->RAM[i] == 0) printf(".");
        else printf ("%c", spu->RAM[i]);
        if ((i + 1) % 101 == 0) printf ("\n");
    }
    printf("\n");
}


void hlt() {
    printf("You end the program\n");
}




