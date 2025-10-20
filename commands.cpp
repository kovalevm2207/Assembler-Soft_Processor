#include "commands.h"
//-------------------------------------------------------------//
//                        ASM  FUNC                            //
//-------------------------------------------------------------//

//--------------------------------------------------------------NOTHING
int nothing_asm(translator_s* translator) {(void) translator; return 0;}
//--------------------------------------------------------------NOTHING
//--------------------------------------------------------------ARG
int arg_asm(translator_s* translator)
{
    assert(translator != NULL);

    int data = 0;

    if (sscanf(translator->lines[translator->count_line].ptr,"%*s %d", &data) != 1) {
        printf(CHANGE_ON RED TEXT_COLOR "Missing argument for a function that must have an argument\n" RESET);
        //do_help();
        return 1/*NOTFOUND_ARG*/;
    }
    translator->codes[translator->code_num++] = data;

    #ifdef DUMP
        printf("arg = %d", data);
        getchar();
    #endif
    return /*ASSEMBLER_OK*/0;
}
//-------------------------------------------------------------ARG
//-------------------------------------------------------------REG
int reg_asm(translator_s* translator)
{
    assert(translator != NULL);

    #ifdef DUMP
        const int REG = 0;
        const int MEM_I = 1;
        int MODE = REG;
    #endif

    char reg[MAX_REG_LENGTH] = {};
    if (sscanf(translator->lines[translator->count_line].ptr, "%*s [%[^]]", reg) != 1) {
        if (sscanf(translator->lines[translator->count_line].ptr,"%*s %s", reg) != 1) {
            printf(CHANGE_ON RED TEXT_COLOR "Missing reg for a function that must have an reg\n" RESET);
            //do_help();
            return 1/*NOTFOUND_REG*/;
        }
    } else {
        #ifdef DUMP
            printf("[");
            MODE = MEM_I
        #endif
        ;
    }

    for(int i = 0; i < REGS_NUM; i++) {
        if(reg[0] == 'A' + i) {
            translator->codes[(translator->code_num)++] = i;
        }
    }

    #ifdef DUMP
        printf("%s", reg);
        if (MODE == MEM_I) printf("]");
        getchar();
    #endif

    return 0/*ASSEMBLER_OK*/;
}
//---------------------------------------------------------------REG
//---------------------------------------------------------------LBL
int lbl_asm(translator_s* translator)
{
    int label = 0;
    sscanf(translator->lines[translator->count_line].ptr, "%*s :%d", &label);
    #ifdef DUMP
        printf(":%d or %d", label, translator->labels[label]); getchar();
        getchar();
    #endif
    translator->codes[(translator->code_num)++] = translator->labels[label];

    return 0/*ASSEMBLER_OK*/;
}
//--------------------------------------------------------------LBL
//-------------------------------------------------------------//
//                        ASM  FUNC                            //
//-------------------------------------------------------------//

//=================================================================
//=================================================================

//-------------------------------------------------------------//
//                        EXE  FUNC                            //
//-------------------------------------------------------------//

//-----------------------------------------------------------PUSH
int nothing_exe(int a, int b) {(void)a; (void)b; return 0;}

void push_exe(SPU* spu, const command_s* command)
{
    command->action_exe(0, 0);
    stack_t data = spu->code[++(spu->PC)];
    StackPush(&spu->stk, data);
}

void pushreg_exe(SPU* spu, const command_s* command)
{
    command->action_exe(0, 0);

    reg_t reg = (reg_t) spu->code[++(spu->PC)];
    StackPush(&spu->stk, spu->regs[reg]);
}

void pushm_exe(SPU* spu, const command_s* command)
{
    command->action_exe(0, 0);

    int mem_i = spu->code[++(spu->PC)];
    StackPush(&spu->stk, spu->RAM[mem_i]);
}
//-----------------------------------------------------------PUSH
//-----------------------------------------------------------POP
void popreg_exe(SPU* spu, const command_s* command)
{
    command->action_exe(0, 0);

    reg_t reg = (reg_t) spu->code[++(spu->PC)];
    StackPop(&spu->stk, &spu->regs[reg]);
}

void popm_exe(SPU* spu, const command_s* command)
{
    command->action_exe(0, 0);

    stack_t data = 0;
    StackPop(&spu->stk, &data);
    int mem_i = spu->regs[spu->code[++(spu->PC)]];
    spu->RAM[mem_i] = data;
}
//-----------------------------------------------------------POP
//-----------------------------------------------------------MATH
int add_exe(int a, int b) {return a + b; }
int sub_exe(int a, int b) {return b - a; }
int mod_exe(int a, int b) {return b % a; }
int my_div_exe(int a, int b) {return b / a; }
int mul_exe(int a, int b) {return a * b; }

void math_exe(SPU* spu, const command_s* command)
{
    stack_t a = 0, b = 0;
    StackPop(&spu->stk, &a);
    StackPop(&spu->stk, &b);

    stack_t c = command->action_exe(a, b);
    StackPush(&spu->stk, c);
}

void pow_exe(SPU* spu, const command_s* command)
{
    command->action_exe(0, 0);

    stack_t a = 0, n = spu->code[++(spu->PC)];
    StackPop(&spu->stk, &a);

    stack_t c = pow(a,n);
    StackPush(&spu->stk, c);
}

void my_sqrt_exe(SPU* spu, const command_s* command)
{
    command->action_exe(0, 0);

    stack_t a = 0;
    StackPop(&spu->stk, &a);

    stack_t b = sqrt(a);
    StackPush(&spu->stk, b);
}
//-----------------------------------------------------------MATH
//-----------------------------------------------------------JMPs
int jb_exe(int a, int b) { return a < b; }
int jbe_exe(int a, int b) { return a <= b; }
int ja_exe(int a, int b) { return a > b; }
int jae_exe(int a, int b) { return a >= b; }
int je_exe(int a, int b) { return a == b; }
int jne_exe(int a, int b) { return a != b; }

void jmp_exe(SPU* spu, const command_s* command)
{
    stack_t a = 0, b = 0;
    StackPop(&spu->stk, &b);
    StackPop(&spu->stk, &a);

    if (command->action_exe(a, b)) {
        int new_pc = spu->code[++(spu->PC)];
        spu->PC = new_pc - 1;
    } else (spu->PC)++;
}

void call_exe(SPU* spu, const command_s* command)
{
    command->action_exe(0, 0);

    StackPush(&spu->labels, spu->PC);
    int new_pc = spu->code[++(spu->PC)];
    spu->PC = new_pc - 1;
}

void ret_exe(SPU* spu, const command_s* command)
{
    command->action_exe(0, 0);

    stack_t new_pc = 0;
    StackPop(&spu->labels, &new_pc);
    spu->PC = new_pc + 1;
}
//------------------------------------------------------------JMPs
//------------------------------------------------------------SYSTEM
void out_exe(SPU* spu, const command_s* command)
{
    command->action_exe(0, 0);

    stack_t data = 0;
    StackPop(&spu->stk, &data);

    printf(CHANGE_ON BLUE TEXT_COLOR "answer = %10d\n" RESET, data);
}

void reset_stk_exe(SPU* spu, const command_s* command)
{
    command->action_exe(0, 0);

    StackDtor(&spu->stk);
    StackCtor(&spu->stk, CAPACITY);
}

void in_exe(SPU* spu, const command_s* command)
{
    command->action_exe(0, 0);

    int a = 0;
    printf("Write integer: ");
    scanf("%d", &a);

    StackPush(&spu->stk, a);
}

void draw_exe(SPU* spu, const command_s* command)
{
    command->action_exe(0, 0);

    system ("clear");
    for (int i = 0; i < RAM_SIZE; i+=2) {
        if (spu->RAM[i] == 0) printf(CHANGE_ON BLACK TEXT_COLOR ". " RESET);
        else printf (CHANGE_ON "%d" TEXT_COLOR "%c " RESET, spu->RAM[i + 1], spu->RAM[i]);
        if ((i + 2) % OX_SIZE == 0) printf ("\n");
    }
    printf("\n");
    usleep(DELAY_MS * 1000);
}

void hlt_exe(SPU* spu, const command_s* command)
{
    command->action_exe(0, 0);
    (void)spu;

    printf("You end the program\n");
}
void help_exe(SPU* spu, const command_s* command)
{
    (void) spu;
    (void) command;
    printf("\nusage: main_calc " CHANGE_ON BLUE TEXT_COLOR "[HELP] [HLT] [PUSH] [OUT]\n"
           "                 [SUB] [DIV] [MUL] [POW] [SQRT]\n"
           "                 [RESET_STK] [PUSHREG] [POPREG]\n"
           "                 [JB] [JBE] [JA] [JAE] [JE]\n"
           "                 [JNE] [CALL] [RET] [IN]\n"
           "                 [PUSHM] [POPM] [DRAW] [HLT]\n"
           RESET "\n"
	       "These are common main_calc commands used in various situations:\n"
           "You can use " CHANGE_ON PURPLE TEXT_COLOR "only integers" RESET " value\n"
	       "\n"
           CHANGE_ON GREEN TEXT_COLOR "HELP        " RESET "         cli flags help message\n"
	       CHANGE_ON GREEN TEXT_COLOR "HLT         " RESET "         HaLT (end the program)\n"
           CHANGE_ON GREEN TEXT_COLOR "PUSH     num" RESET "         save the value (num) to calculator memory\n"
           CHANGE_ON GREEN TEXT_COLOR "OUT         " RESET "         retrieve the last saved value from calculator memory\n"
           CHANGE_ON GREEN TEXT_COLOR "ADD         " RESET "         add the two last numbers stored in memory\n"
           CHANGE_ON GREEN TEXT_COLOR "SUB         " RESET "         subtract the last number in memory from the second last number in memory\n"
           CHANGE_ON GREEN TEXT_COLOR "DIV         " RESET "         divide the last number stored in memory by the second last number stored in memory\n"
           CHANGE_ON GREEN TEXT_COLOR "MUL         " RESET "         multiply the two last numbers stored in memory\n"
           CHANGE_ON GREEN TEXT_COLOR "POW      num" RESET "         raise the last number stored in memory to a power (num)\n"
           CHANGE_ON GREEN TEXT_COLOR "SQRT        " RESET "         calculate the square root of the last number in memory\n"
           CHANGE_ON GREEN TEXT_COLOR "RESET_STK   " RESET "         Update the stack pointer\n"
           CHANGE_ON GREEN TEXT_COLOR "PUSHREG  reg" RESET "         Push the register value onto the stack (reg)\n"
           CHANGE_ON GREEN TEXT_COLOR "POPREG   reg" RESET "         Pop the value from the stack into the register (reg)\n"
           CHANGE_ON GREEN TEXT_COLOR "JB    :(lbl)" RESET "         Jump to the label (lbl) if the second-to-last stack value is less than the last stack value.\n"
           CHANGE_ON GREEN TEXT_COLOR "JBE   :(lbl)" RESET "         Jump to the label (lbl) if the second-to-last stack value is less than or equal to the last stack value.\n"
           CHANGE_ON GREEN TEXT_COLOR "JA    :(lbl)" RESET "         Jump to the label (lbl) if the second-to-last value on the stack is greater than the last value on the stack\n"
           CHANGE_ON GREEN TEXT_COLOR "JAE   :(lbl)" RESET "         Jump to the label (lbl) if the stack's second-to-last value is greater than or equal to the last value\n"
           CHANGE_ON GREEN TEXT_COLOR "JE    :(lbl)" RESET "         Jump to the label (lbl) if the second-to-last stack value equals the last stack value\n"
           CHANGE_ON GREEN TEXT_COLOR "JNE   :(lbl)" RESET "         Jump to the label (lbl) if the second-to-last stack value is not equal to the last stack value.\n"
           CHANGE_ON GREEN TEXT_COLOR "CALL  :(lbl)" RESET "         Call the function at the label (lbl)\n"
           CHANGE_ON GREEN TEXT_COLOR "RET         " RESET "         Return from the function\n"
           CHANGE_ON GREEN TEXT_COLOR "IN          " RESET "         Read a value from the keyboard and push it onto the stack\n"
           CHANGE_ON GREEN TEXT_COLOR "PUSHM    reg" RESET "         Push the value from the memory address specified by the register onto the stack.\n"
           CHANGE_ON GREEN TEXT_COLOR "POPM     reg" RESET "         Store the value from the stack into the memory cell at the address specified by the register value.\n"
           CHANGE_ON GREEN TEXT_COLOR "DRAW        " RESET "         Display a graphical representation of the RAM contents.\n"
           CHANGE_ON GREEN TEXT_COLOR "HLT         " RESET "         Terminate the program.\n"
           "\n"
           "Valid register names: " CHANGE_ON GREEN TEXT_COLOR "AX, BX , CX, DX, EX, FX" RESET "\n"
           "Examples of commands:\n"
           "                     PUSH 1\n"
           "                     PUSHREG AX\n"
           "                     CALL :1\n"
           "                     :1\n"
           "                     PUSHM BX\n"
           "                     RET\n"
           "                     HLT\n");
}
//------------------------------------------------------------SYSTEM

//-------------------------------------------------------------//
//                        EXE  FUNC                            //
//-------------------------------------------------------------//
