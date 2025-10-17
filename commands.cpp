#include "commands.h"
#include <unistd.h>

void push(SPU* spu) {
    stack_t data = spu->code[++(spu->PC)];
    StackPush(&spu->stk, data);
}

void add(SPU* spu) {
    stack_t a = 0, b = 0;
    StackPop(&spu->stk, &a);
    StackPop(&spu->stk, &b);

    stack_t c = a + b;
    StackPush(&spu->stk, c);
}

void sub(SPU* spu) {
    stack_t a = 0, b = 0;
    StackPop(&spu->stk, &a);
    StackPop(&spu->stk, &b);

    stack_t c = b - a;
    StackPush(&spu->stk, c);
}


void mod(SPU* spu) {
    stack_t a = 0, b = 0;
    StackPop(&spu->stk, &a);
    StackPop(&spu->stk, &b);

    stack_t c = b % a;
    StackPush(&spu->stk, c);
}


void div(SPU* spu) {
    stack_t a = 0, b = 0;
    StackPop(&spu->stk, &a);
    StackPop(&spu->stk, &b);

    stack_t c = b / a;
    StackPush(&spu->stk, c);
}


void out(SPU* spu) {
    stack_t data = 0;
    StackPop(&spu->stk, &data);

    printf(CHANGE_ON BLUE TEXT_COLOR "answer = %10d\n" RESET, data);
}


void mul(SPU* spu) {
    stack_t a = 0, b = 0;
    StackPop(&spu->stk, &a);
    StackPop(&spu->stk, &b);

    stack_t c = a * b;
    StackPush(&spu->stk, c);
}


void pow(SPU* spu) {
    stack_t a = 0, n = spu->code[++(spu->PC)];
    StackPop(&spu->stk, &a);

    stack_t c = pow(a,n);
    StackPush(&spu->stk, c);
}


void my_sqrt(SPU* spu) {
    stack_t a = 0;
    StackPop(&spu->stk, &a);

    stack_t b = sqrt(a);
    StackPush(&spu->stk, b);
}


void reset_stk(SPU* spu) {
    StackDtor(&spu->stk);
    StackCtor(&spu->stk, CAPACITY);
}


void pushreg(SPU* spu) {
    reg_t reg = (reg_t) spu->code[++(spu->PC)];

    StackPush(&spu->stk, spu->regs[reg]);
}


void popreg(SPU* spu) {
    reg_t reg = (reg_t) spu->code[++(spu->PC)];
    StackPop(&spu->stk, &spu->regs[reg]);
}


void call(SPU* spu) {
    StackPush(&spu->labels, spu->PC);
    int new_pc = spu->code[++(spu->PC)];
    spu->PC = new_pc - 1;
}


void ret(SPU* spu) {
    stack_t new_pc = 0;
    StackPop(&spu->labels, &new_pc);
    spu->PC = new_pc + 1;
}


void jmp(SPU* spu) {
    int new_pc = spu->code[++(spu->PC)];
    spu->PC = new_pc - 1;
}


void jb(SPU* spu) {
    stack_t a = 0, b = 0;
    StackPop(&spu->stk, &b);
    StackPop(&spu->stk, &a);

    if (a < b) {
        int new_pc = spu->code[++(spu->PC)];
        spu->PC = new_pc - 1;
    } else (spu->PC)++;
}


void jbe(SPU* spu) {
    stack_t a = 0, b = 0;
    StackPop(&spu->stk, &b);
    StackPop(&spu->stk, &a);

    if (a <= b) {
        int new_pc = spu->code[++(spu->PC)];
        spu->PC = new_pc - 1;
    } else (spu->PC)++;
}


void ja(SPU* spu) {
    stack_t a = 0, b = 0;
    StackPop(&spu->stk, &b);
    StackPop(&spu->stk, &a);

    if (a > b) {
        int new_pc = spu->code[++(spu->PC)];
        spu->PC = new_pc - 1;
    } else (spu->PC)++;
}


void jae(SPU* spu) {
    stack_t a = 0, b = 0;
    StackPop(&spu->stk, &b);
    StackPop(&spu->stk, &a);

    if (a >= b) {
        int new_pc = spu->code[++(spu->PC)];
        spu->PC = new_pc - 1;
    } else (spu->PC)++;
}


void je(SPU* spu) {
    stack_t a = 0, b = 0;
    StackPop(&spu->stk, &b);
    StackPop(&spu->stk, &a);

    if (a == b) {
        int new_pc = spu->code[++(spu->PC)];
        spu->PC = new_pc - 1;
    } else (spu->PC)++;
}


void jne(SPU* spu) {
    stack_t a = 0, b = 0;
    StackPop(&spu->stk, &b);
    StackPop(&spu->stk, &a);

    if (a != b) {
        int new_pc = spu->code[++(spu->PC)];
        spu->PC = new_pc - 1;
    } else (spu->PC)++;
}


void in(SPU* spu) {
    int a = 0;
    printf("Write integer: ");
    scanf("%d", &a);

    StackPush(&spu->stk, a);
}


void popm(SPU* spu) {
    stack_t data = 0;
    StackPop(&spu->stk, &data);
    int mem_i = spu->regs[spu->code[++(spu->PC)]];
    spu->RAM[mem_i] = data;
}


void pushm(SPU* spu) {
    int mem_i = spu->code[++(spu->PC)];
    StackPush(&spu->stk, spu->RAM[mem_i]);
}


void draw(SPU* spu) {
    system ("clear");
    for (int i = 0; i < 9213; i++) {
        if (spu->RAM[i] == 0) printf(". ");
        else printf ("%c ", spu->RAM[i]);
        if ((i + 1) % 111 == 0) printf ("\n");
    }
    printf("\n");
    usleep(100000);
}


void hlt(SPU* spu) {
    (void)spu;
    printf("You end the program\n");
}




