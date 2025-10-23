#include "commands.h"
//-------------------------------------------------------------//
//                        ASM  FUNC                            //
//-------------------------------------------------------------//

//--------------------------------------------------------------NOTHING
cmdErr_t nothing_asm(translator_s* translator)
{
    assert(translator != NULL);
    (void) translator;
    return CMD_OK;
}
//--------------------------------------------------------------NOTHING
//--------------------------------------------------------------ARG
cmdErr_t arg_asm(translator_s* translator)
{
    assert(translator != NULL);

    int data = 0;

    if (sscanf(translator->lines[translator->count_line].ptr,"%*s %d", &data) != 1) {
        printf(RED_COLOR "Missing argument for a function that must have an argument\n" RESET);
        //do_help();
        return NFIND_ARG;
    }
    translator->codes[translator->code_num++] = data;

    #ifdef DUMP
        printf("arg = %d", data);
        getchar();
    #endif
    return CMD_OK;
}
//-------------------------------------------------------------ARG
//-------------------------------------------------------------REG
cmdErr_t reg_asm(translator_s* translator)
{
    assert(translator != NULL);

    #ifdef DUMP
        const int REG = 0;
        const int MEM_I = 1;                                                                            //mb define KoV
        int MODE = REG;
    #endif

    char reg[MAX_REG_LENGTH] = {};
    if (sscanf(translator->lines[translator->count_line].ptr, "%*s [%[^]]", reg) != 1) {
        if (sscanf(translator->lines[translator->count_line].ptr,"%*s %s", reg) != 1) {
            printf(RED_COLOR "Missing reg for a function that must have an reg\n" RESET);
            // help_exe();
            return NFIND_REG;
        }
    } else {
        #ifdef DUMP
            printf("[");
            MODE = MEM_I
        #endif
        ;
    }
    if (reg[1] != 'X' || reg[2] != '\0') {
        printf(RED_COLOR "INvalid reg for a function\n" RESET);
        //do_help();
        return INVALID_REG;
    }

    for(int i = 0; i < REGS_NUM; i++) {
        if(reg[0] == 'A' + i) {
            translator->codes[(translator->code_num)++] = i;
            return CMD_OK;
        }
    }

    #ifdef DUMP
        printf("%s", reg);
        if (MODE == MEM_I) printf("]");
        getchar();
    #endif

    printf(RED_COLOR "INVALID REG FOR THE FUNCTION\n" RESET);
    return INVALID_REG;
}
//---------------------------------------------------------------REG
//---------------------------------------------------------------LBL
cmdErr_t lbl_asm(translator_s* translator)
{
    assert(translator != NULL);

    int label = 0;
    if (sscanf(translator->lines[translator->count_line].ptr, " :%d", &label) != 1) {
        if (sscanf(translator->lines[translator->count_line].ptr, "%*s :%d", &label) != 1) return NFIND_LBL;
        translator->codes[(translator->code_num)++] = translator->labels[label];
        return CMD_OK;
    }
    #ifdef DUMP
        printf(":%d or %d", label, translator->labels[label]); getchar();
        getchar();
    #endif
    translator->labels[label] = translator->code_num;

    return CMD_OK;                                                //enum for results KoV  сделал Мика
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
cmdErr_t push_exe(SPU* spu, const command_s* command)
{
    assert(spu != NULL);
    assert(command != NULL);
    (void) command;

    stack_t data = spu->code[++(spu->PC)];
    StackPush(&spu->stk, data);

    return CMD_OK;
}

cmdErr_t pushreg_exe(SPU* spu, const command_s* command)
{
    assert(spu != NULL);
    assert(command != NULL);
    (void) command;

    reg_t reg = (reg_t) spu->code[++(spu->PC)];
    StackPush(&spu->stk, spu->regs[reg]);

    return CMD_OK;
}

cmdErr_t pushm_exe(SPU* spu, const command_s* command)
{
    assert(spu != NULL);
    assert(command != NULL);
    (void) command;

    int mem_i = spu->code[++(spu->PC)];
    StackPush(&spu->stk, spu->RAM[mem_i]);

    return CMD_OK;
}
//-----------------------------------------------------------PUSH
//-----------------------------------------------------------POP
cmdErr_t popreg_exe(SPU* spu, const command_s* command)
{
    assert(spu != NULL);
    assert(command != NULL);
    (void) command;

    reg_t reg = (reg_t) spu->code[++(spu->PC)];
    StackPop(&spu->stk, &spu->regs[reg]);

    return CMD_OK;
}

cmdErr_t popm_exe(SPU* spu, const command_s* command)
{
    assert(spu != NULL);
    assert(command != NULL);
    (void) command;

    stack_t data = 0;
    StackPop(&spu->stk, &data);
    int mem_i = spu->regs[spu->code[++(spu->PC)]];
    spu->RAM[mem_i] = data;

    return CMD_OK;
}
//-----------------------------------------------------------POP
//-----------------------------------------------------------MATH
cmdErr_t math_exe(SPU* spu, const command_s* command)
{
    assert(spu != NULL);
    assert(command != NULL);

    stack_t a = 0, b = 0;
    StackPop(&spu->stk, &a);
    StackPop(&spu->stk, &b);

    int c = 0;

    switch(command->num)
    {
        case ADD:
            c = b + a;
            break;
        case SUB:
            c = b - a;
            break;
        case MOD:
            c = b % a;
            break;
        case MUL:
            c = b * a;
            break;
        case DIV:
            c = b / a;
            break;
        default:
            printf(RED_COLOR "COMMANDS STRUCT INIT ERROR,\n"
                   "please check the commands arr of structs init\n" RESET);
            return TOTAL_ERR;
    }
    StackPush(&spu->stk, c);

    return CMD_OK;
}

cmdErr_t pow_exe(SPU* spu, const command_s* command)
{
    assert(spu != NULL);
    assert(command != NULL);
    (void) command;

    stack_t a = 0, n = spu->code[++(spu->PC)];
    StackPop(&spu->stk, &a);

    stack_t c = pow(a,n);
    StackPush(&spu->stk, c);

    return CMD_OK;
}

cmdErr_t my_sqvrt_exe(SPU* spu, const command_s* command) //sqrt -> sqvrt KoV
{
    assert(spu != NULL);
    assert(command != NULL);
    (void) command;

    stack_t a = 0;
    StackPop(&spu->stk, &a);

    stack_t b = sqrt(a);
    StackPush(&spu->stk, b);

    return CMD_OK;
}
//-----------------------------------------------------------MATH
//-----------------------------------------------------------JMPs
cmdErr_t jmp_exe(SPU* spu, const command_s* command)
{
    assert(spu != NULL);
    assert(command != NULL);

    stack_t a = 0, b = 0;
    StackPop(&spu->stk, &b);
    StackPop(&spu->stk, &a);

    int compare_result = -1;
    switch(command->num)
    {
        case JB:
            compare_result = (a < b);
            break;
        case JBE:
            compare_result = (a <= b);
            break;
        case JA:
            compare_result = (a > b);
            break;
        case JAE:
            compare_result = (a >= b);
            break;
        case JE:
            compare_result = (a == b);
            break;
        case JNE:
            compare_result = (a != b);
            break;
        default:
            printf(RED_COLOR "COMMANDS STRUCT INIT ERROR,\n"
                   "please check the commands arr of structs init\n" RESET);
            return TOTAL_ERR;
    }
    if (compare_result == 1) {
        int new_pc = spu->code[++(spu->PC)];
        spu->PC = new_pc - 1;
    } else (spu->PC)++;

    return CMD_OK;
}

cmdErr_t call_exe(SPU* spu, const command_s* command)
{
    assert(spu != NULL);
    assert(command != NULL);
    (void) command;

    StackPush(&spu->labels, spu->PC);
    int new_pc = spu->code[++(spu->PC)];
    spu->PC = new_pc - 1;

    return CMD_OK;
}

cmdErr_t ret_exe(SPU* spu, const command_s* command)
{
    assert(spu != NULL);
    assert(command != NULL);
    (void) command;

    stack_t new_pc = 0;
    StackPop(&spu->labels, &new_pc);
    spu->PC = new_pc + 1;

    return CMD_OK;
}
//------------------------------------------------------------JMPs
//------------------------------------------------------------SYSTEM
cmdErr_t out_exe(SPU* spu, const command_s* command)
{
    assert(spu != NULL);
    assert(command != NULL);
    (void) command;

    stack_t data = 0;
    StackPop(&spu->stk, &data);

    printf(BLUE_COLOR "answer = %10d\n" RESET, data);

    return CMD_OK;
}

cmdErr_t reset_stk_exe(SPU* spu, const command_s* command)
{
    assert(spu != NULL);
    assert(command != NULL);
    (void) command;

    StackDtor(&spu->stk);
    StackCtor(&spu->stk, CAPACITY);

    return CMD_OK;
}

cmdErr_t in_exe(SPU* spu, const command_s* command)
{
    assert(spu != NULL);
    assert(command != NULL);
    (void) command;

    int a = 0;
    printf("Write integer: ");
    scanf("%d", &a);

    StackPush(&spu->stk, a);

    return CMD_OK;
}

cmdErr_t draw_exe(SPU* spu, const command_s* command)
{
    assert(spu != NULL);
    assert(command != NULL);
    (void) command;

    printf("\033[H");
    for (int i = 0; i < RAM_SIZE; i+=2) {
        if (spu->RAM[i] == 0) printf(CHANGE_ON BLACK TEXT_COLOR ". " RESET);
        else printf (CHANGE_ON "%d" TEXT_COLOR "%c " RESET, spu->RAM[i + 1], spu->RAM[i]);
        if ((i + 2) % OX_SIZE == 0) printf ("\n");
    }
    printf("\n");
    usleep(DELAY_MS * 1000);

    return CMD_OK;
}

cmdErr_t hlt_exe(SPU* spu, const command_s* command)
{
    assert(spu != NULL);
    assert(command != NULL);
    (void) command;
    (void)spu;

    printf("You end the program\n");

    return CMD_OK;
}
cmdErr_t help_exe(SPU* spu, const command_s* command)
{
    assert(spu != NULL);
    assert(command != NULL);
    (void) spu;
    (void) command;

    printf("\nusage: main_calc " BLUE_COLOR "[HELP] [HLT] [PUSH] [OUT]\n"
           "                 [SUB] [DIV] [MUL] [POW] [SQRT]\n"
           "                 [RESET_STK] [PUSHREG] [POPREG]\n"
           "                 [JB] [JBE] [JA] [JAE] [JE]\n"
           "                 [JNE] [CALL] [RET] [IN]\n"
           "                 [PUSHM] [POPM] [DRAW] [HLT]\n"
           RESET "\n"
	       "These are common main_calc commands used in various situations:\n"
           "You can use " PURPLE_COLOR "only integers" RESET " value\n"
	       "\n"
           GREEN_COLOR "HELP        " RESET "         cli flags help message\n"
	       GREEN_COLOR "HLT         " RESET "         HaLT (end the program)\n"
           GREEN_COLOR "PUSH     num" RESET "         save the value (num) to calculator memory\n"
           GREEN_COLOR "OUT         " RESET "         retrieve the last saved value from calculator memory\n"
           GREEN_COLOR "ADD         " RESET "         add the two last numbers stored in memory\n"
           GREEN_COLOR "SUB         " RESET "         subtract the last number in memory from the second last number in memory\n"
           GREEN_COLOR "DIV         " RESET "         divide the last number stored in memory by the second last number stored in memory\n"
           GREEN_COLOR "MUL         " RESET "         multiply the two last numbers stored in memory\n"
           GREEN_COLOR "POW      num" RESET "         raise the last number stored in memory to a power (num)\n"
           GREEN_COLOR "SQRT        " RESET "         calculate the square root of the last number in memory\n"
           GREEN_COLOR "RESET_STK   " RESET "         Update the stack pointer\n"
           GREEN_COLOR "PUSHREG  reg" RESET "         Push the register value onto the stack (reg)\n"
           GREEN_COLOR "POPREG   reg" RESET "         Pop the value from the stack into the register (reg)\n"
           GREEN_COLOR "JB    :(lbl)" RESET "         Jump to the label (lbl) if the second-to-last stack value is less than the last stack value.\n"
           GREEN_COLOR "JBE   :(lbl)" RESET "         Jump to the label (lbl) if the second-to-last stack value is less than or equal to the last stack value.\n"
           GREEN_COLOR "JA    :(lbl)" RESET "         Jump to the label (lbl) if the second-to-last value on the stack is greater than the last value on the stack\n"
           GREEN_COLOR "JAE   :(lbl)" RESET "         Jump to the label (lbl) if the stack's second-to-last value is greater than or equal to the last value\n"
           GREEN_COLOR "JE    :(lbl)" RESET "         Jump to the label (lbl) if the second-to-last stack value equals the last stack value\n"
           GREEN_COLOR "JNE   :(lbl)" RESET "         Jump to the label (lbl) if the second-to-last stack value is not equal to the last stack value.\n"
           GREEN_COLOR "CALL  :(lbl)" RESET "         Call the function at the label (lbl)\n"
           GREEN_COLOR "RET         " RESET "         Return from the function\n"
           GREEN_COLOR "IN          " RESET "         Read a value from the keyboard and push it onto the stack\n"
           GREEN_COLOR "PUSHM    reg" RESET "         Push the value from the memory address specified by the register onto the stack.\n"
           GREEN_COLOR "POPM     reg" RESET "         Store the value from the stack into the memory cell at the address specified by the register value.\n"
           GREEN_COLOR "DRAW        " RESET "         Display a graphical representation of the RAM contents.\n"
           GREEN_COLOR "HLT         " RESET "         Terminate the program.\n"
           "\n"
           "Valid register names: " GREEN_COLOR "AX, BX , CX, DX, EX, FX" RESET "\n"
           "Examples of commands:\n"
           "                     PUSH 1\n"
           "                     PUSHREG AX\n"
           "                     CALL :1\n"
           "                     :1\n"
           "                     PUSHM BX\n"
           "                     RET\n"
           "                     HLT\n");

    return CMD_OK;
}
//------------------------------------------------------------SYSTEM

//-------------------------------------------------------------//
//                        EXE  FUNC                            //
//-------------------------------------------------------------//
