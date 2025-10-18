#include "commands.h"

//-----------------------------------------------------------PUSH
int nothing(int a, int b) {(void)a; (void)b; return 0;}

void push(SPU* spu, const command_s* command) {
    command->action_exe(0, 0);
    stack_t data = spu->code[++(spu->PC)];
    StackPush(&spu->stk, data);
}

void pushreg(SPU* spu, const command_s* command) {
    command->action_exe(0, 0);

    reg_t reg = (reg_t) spu->code[++(spu->PC)];
    StackPush(&spu->stk, spu->regs[reg]);
}

void pushm(SPU* spu, const command_s* command) {
    command->action_exe(0, 0);

    int mem_i = spu->code[++(spu->PC)];
    StackPush(&spu->stk, spu->RAM[mem_i]);
}
//-----------------------------------------------------------PUSH
//-----------------------------------------------------------POP
void popreg(SPU* spu, const command_s* command) {
    command->action_exe(0, 0);

    reg_t reg = (reg_t) spu->code[++(spu->PC)];
    StackPop(&spu->stk, &spu->regs[reg]);
}

void popm(SPU* spu, const command_s* command) {
    command->action_exe(0, 0);

    stack_t data = 0;
    StackPop(&spu->stk, &data);
    int mem_i = spu->regs[spu->code[++(spu->PC)]];
    spu->RAM[mem_i] = data;
}
//-----------------------------------------------------------POP
//-----------------------------------------------------------MATH
int add(int a, int b) {return a + b; }
int sub(int a, int b) {return b - a; }
int mod(int a, int b) {return b % a; }
int my_div(int a, int b) {return b / a; }
int mul(int a, int b) {return a * b; }

void math(SPU* spu, const command_s* command)
{
    stack_t a = 0, b = 0;
    StackPop(&spu->stk, &a);
    StackPop(&spu->stk, &b);

    stack_t c = command->action_exe(a, b);
    StackPush(&spu->stk, c);
}

void pow(SPU* spu, const command_s* command) {
    command->action_exe(0, 0);

    stack_t a = 0, n = spu->code[++(spu->PC)];
    StackPop(&spu->stk, &a);

    stack_t c = pow(a,n);
    StackPush(&spu->stk, c);
}

void my_sqrt(SPU* spu, const command_s* command) {
    command->action_exe(0, 0);

    stack_t a = 0;
    StackPop(&spu->stk, &a);

    stack_t b = sqrt(a);
    StackPush(&spu->stk, b);
}
//-----------------------------------------------------------MATH
//-----------------------------------------------------------JMPs
int jb(int a, int b) { return a < b; }
int jbe(int a, int b) { return a <= b; }
int ja(int a, int b) { return a > b; }
int jae(int a, int b) { return a >= b; }
int je(int a, int b) { return a == b; }
int jne(int a, int b) { return a != b; }

void jmp(SPU* spu, const command_s* command)
{
    stack_t a = 0, b = 0;
    StackPop(&spu->stk, &b);
    StackPop(&spu->stk, &a);

    if (command->action_exe(a, b)) {
        int new_pc = spu->code[++(spu->PC)];
        spu->PC = new_pc - 1;
    } else (spu->PC)++;
}

void call(SPU* spu, const command_s* command) {
    command->action_exe(0, 0);

    StackPush(&spu->labels, spu->PC);
    int new_pc = spu->code[++(spu->PC)];
    spu->PC = new_pc - 1;
}

void ret(SPU* spu, const command_s* command) {
    command->action_exe(0, 0);

    stack_t new_pc = 0;
    StackPop(&spu->labels, &new_pc);
    spu->PC = new_pc + 1;
}
//------------------------------------------------------------JMPs
//------------------------------------------------------------SYSTEM
void out(SPU* spu, const command_s* command) {
    command->action_exe(0, 0);

    stack_t data = 0;
    StackPop(&spu->stk, &data);

    printf(CHANGE_ON BLUE TEXT_COLOR "answer = %10d\n" RESET, data);
}

void reset_stk(SPU* spu, const command_s* command) {
    command->action_exe(0, 0);

    StackDtor(&spu->stk);
    StackCtor(&spu->stk, CAPACITY);
}

void in(SPU* spu, const command_s* command) {
    command->action_exe(0, 0);

    int a = 0;
    printf("Write integer: ");
    scanf("%d", &a);

    StackPush(&spu->stk, a);
}

void draw(SPU* spu, const command_s* command) {
    command->action_exe(0, 0);

    system ("clear");
    for (int i = 0; i < 9213; i++) {
        if (spu->RAM[i] == 0) printf(". ");
        else printf ("%c ", spu->RAM[i]);
        if ((i + 1) % 111 == 0) printf ("\n");
    }
    printf("\n");
    usleep(100000);
}

void hlt(SPU* spu, const command_s* command) {
    command->action_exe(0, 0);
    (void)spu;

    printf("You end the program\n");
}
//------------------------------------------------------------SYSTEM
