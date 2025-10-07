#include <stdio.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

#include "commands.h"
#include "my_stack.h"
#include "ReadFile.h"

const size_t CAPACITY = 15;

typedef enum
{
    PROCESSOR_OK         = 1 << 1,
    COMMAND_NOT_FOUND    = 1 << 3,
    PROGRAM_END_MISSING  = 1 << 4,
    BAD_PUSH             = 1 << 5,
    BAD_PASSWORD         = 1 << 7,
    BAD_VERSION          = 1 << 8
} ProcessorErr_t;

typedef struct SPU
{
    char* code;
    size_t PC;
    stack_s stk;
    int* regs;
    size_t file_size;
}SPU;

ProcessorErr_t ProcessorCtor(SPU* spu, char* code, int* regs, size_t file_size);
ProcessorErr_t check_signature(char* code);
ProcessorErr_t ProcessorExe(SPU* spu);
ProcessorErr_t ProcessorDtor(SPU* spu);
ProcessorErr_t ProcessorDump(SPU* spu);

int main()
{
    size_t file_size0 = find_file_size("code2.bin");

    char* code = (char*) calloc(file_size0 + 1, sizeof(char));
    assert(code != NULL);

    int file = open("code2.bin", O_RDONLY);
    assert(file != 0);

    size_t file_size = read (file, code, file_size0 + 1);
    assert(file_size != 0);
    code[file_size] = '\0';

    close(file);

    SPU spu = {};
    int regs[4] = {/*AX, BX, CX, DX*/};
    ProcessorCtor(&spu, code, regs, file_size);

    ProcessorExe(&spu);

    ProcessorDtor(&spu);

    return 0;
}


ProcessorErr_t ProcessorCtor(SPU* spu, char* code, int* regs, size_t file_size)
{
    int STATUS = PROCESSOR_OK;
    STATUS |= check_signature(code);

    stack_s stk = {};
    STATUS |= StackCtor(&stk, CAPACITY);
    printf("STATUS == %d\n", STATUS);

    spu->code = code;
    spu->PC = 7;
    spu->stk = stk;
    spu->regs = regs;
    spu->file_size = file_size;

    if (STATUS & PROCESSOR_OK) return PROCESSOR_OK;
    else                       return (ProcessorErr_t) STATUS;
}


ProcessorErr_t check_signature(char* code)
{
    assert(code != NULL);

    int sign_err = PROCESSOR_OK;

    if (code[6] != VERSION)              sign_err |= BAD_VERSION;
    if (strncmp(code, PASSWORD, 6) != 0) sign_err |= BAD_PASSWORD;

    return (ProcessorErr_t) sign_err;
}


ProcessorErr_t ProcessorDtor(SPU* spu)
{
    StackDtor(&spu->stk);
    spu->PC = 0;
    for (int i = 0; i < 4; i++) {
        spu->regs[i] = 0;
    }
    free(spu->code);

    return PROCESSOR_OK;
}


ProcessorErr_t ProcessorDump(SPU* spu)
{
    printf(CHANGE_ON GREEN TEXT_COLOR "ProcessorDump()\n" RESET);
    StackDump(&spu->stk);
    printf("code:\n");
    printf("PC:" CHANGE_ON PURPLE TEXT_COLOR);
    for (size_t i = 0; i < spu->file_size; i++) {
        if (i == 6)  printf(CHANGE_ON YELLOW TEXT_COLOR);
        if (i > 6) printf(RESET);
        printf("  %3zu", i);
    }
    printf("\n   ");
    for (size_t i = 0; i < spu->file_size; i++) {
        if (i < 6) printf("  %3c", spu->code[i]);
        else       printf("  %3d", spu->code[i]);
    }
    printf("\n   ");
    for (size_t i = 0; i < spu->file_size; i++) {
        if (i == spu->PC) printf("--" CHANGE_ON BOLD WITH RED TEXT_COLOR "^^^" RESET);
        else              printf("-----");
    }
    printf("\n" CHANGE_ON PURPLE TEXT_COLOR "       PASSWORD    " CHANGE_ON YELLOW TEXT_COLOR  "        VERSION\n" RESET);
    printf("regs:   AX = [%d], BX = [%d], CX = [%d], DX = [%d]\n", spu->regs[0], spu->regs[1], spu->regs[2], spu->regs[3]);
    return PROCESSOR_OK;
}


ProcessorErr_t ProcessorExe(SPU* spu)
{
    assert(spu != NULL);

    char* code = spu->code;
    size_t* PC = &spu->PC;
    stack_s* stk = &spu->stk;
    int* regs = spu->regs;

    while (code[*PC] != '\0') {
        switch(code[*PC]) {
            case PUSH: {
                stack_t data = code[++(*PC)];

                StackPush(stk, data);
                ProcessorDump(spu);
                getchar();
                break;
            }
            case ADD: {
                stack_t a = 0, b = 0;
                StackPop(stk, &a);
                StackPop(stk, &b);

                stack_t c = a + b;
                StackPush(stk, c);

                ProcessorDump(spu);
                getchar();
                break;
            }
            case SUB: {
                stack_t a = 0, b = 0;
                StackPop(stk, &a);
                StackPop(stk, &b);

                stack_t c = b - a;
                StackPush(stk, c);

                ProcessorDump(spu);
                getchar();
                break;
            }
            case DIV: {
                stack_t a = 0, b = 0;
                StackPop(stk, &a);
                StackPop(stk, &b);

                stack_t c = b / a;
                StackPush(stk, c);

                ProcessorDump(spu);
                getchar();
                break;
            }
            case OUT: {
                stack_t data = 0;
                StackPop(stk, &data);

                printf("answer = %10d\n", data);

                ProcessorDump(spu);
                getchar();
                break;
            }
            case MUL: {
                stack_t a = 0, b = 0;
                StackPop(stk, &a);
                StackPop(stk, &b);

                stack_t c = a * b;
                StackPush(stk, c);

                ProcessorDump(spu);
                getchar();
                break;
            }
            case POW: {
                stack_t a = 0, n = code[++(*PC)];
                StackPop(stk, &a);

                stack_t c = pow(a,n);
                StackPush(stk, c);

                ProcessorDump(spu);
                getchar();
                break;
            }
            case SQRT: {
                stack_t a = 0;
                StackPop(stk, &a);

                stack_t b = sqrt(a);
                StackPush(stk, b);

                ProcessorDump(spu);
                getchar();
                break;
            }
            case RESET_STK: {
                StackDtor(stk);
                StackCtor(stk, CAPACITY);

                ProcessorDump(spu);
                getchar();
                break;
            }
            case PUSHREG: {
                reg_t reg = (reg_t) code[++(*PC)];

                StackPush(stk, regs[reg]);

                ProcessorDump(spu);
                getchar();
                break;
            }
            case POPREG: {
                reg_t reg = (reg_t) code[++(*PC)];

                StackPop(stk, &regs[reg]);

                ProcessorDump(spu);
                getchar();
                break;
            }
            case JMP: {
                int new_pc = code[++(*PC)];
                *PC = new_pc;

                ProcessorDump(spu);
                getchar();
                break;
            }
            case HLT:
                return PROCESSOR_OK;
            default:
                return COMMAND_NOT_FOUND;
        }
        (*PC)++;
    }
    return PROGRAM_END_MISSING;
}
