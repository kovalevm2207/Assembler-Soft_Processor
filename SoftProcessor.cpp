#include <stdio.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

#include "commands.h"
#include "my_stack.h"
#include "ReadFile.h"

const size_t CAPACITY = 20;
const char* FILE_NAME = "NotLineSquareSolver.bin";//"example2.bin";

typedef enum
{
    PROCESSOR_OK         = 1 << 1,
    COMMAND_NOT_FOUND    = 1 << 2,
    PROGRAM_END_MISSING  = 1 << 3,
    BAD_PUSH             = 1 << 4,
    BAD_PASSWORD         = 1 << 5,
    BAD_VERSION          = 1 << 6
} ProcessorErr_t;

typedef struct SPU
{
    int* code;
    size_t PC;
    stack_s stk;
    int regs[5];
    size_t file_size;
} SPU;

ProcessorErr_t ProcessorCtor(SPU* spu, int* code);
ProcessorErr_t check_signature(int* code);
ProcessorErr_t ProcessorExe(SPU* spu);
ProcessorErr_t ProcessorDtor(SPU* spu);
ProcessorErr_t ProcessorDump(SPU* spu);

int main()
{
    SPU spu = {};

    FILE* file = fopen(FILE_NAME, "rb"); // fopen возвращает файловый дискриптор
    assert(file != NULL);

    if (fread(&(spu.file_size), sizeof(size_t), 1, file) != 1) {
        printf(CHANGE_ON RED TEXT_COLOR "ERROR! FILE SIZE READ FAILED\n" RESET);
        return 1;
    }

    int* code = (int*) calloc(spu.file_size, sizeof(char));
    assert(code != NULL);

    if (fread(code, sizeof(int), spu.file_size/sizeof(int), file) != spu.file_size/sizeof(int)) {
        printf(CHANGE_ON RED TEXT_COLOR "ERROR! CODE READ FAILED\n" RESET);
        return 1;
    }

    if (ProcessorCtor(&spu, code) != PROCESSOR_OK) {
        printf(CHANGE_ON RED TEXT_COLOR "ERROR! PROCESSORCTOR FAILED\n" RESET);
        ProcessorDtor(&spu);
        return 1;
    }

    fclose(file);

    ProcessorExe(&spu);

    ProcessorDtor(&spu);

    return 0;
}


ProcessorErr_t ProcessorCtor(SPU* spu, int* code)
{
    assert(spu != NULL);
    assert(code != NULL);

    int STATUS = PROCESSOR_OK;
    STATUS |= check_signature(code);

    stack_s stk = {};
    STATUS |= StackCtor(&stk, CAPACITY);

    spu->code = code;
    spu->PC = 2;
    spu->stk = stk;
    for (int i = 0; i < 4; i++) {
        spu->regs[i] = 0;
    }

    if (STATUS == PROCESSOR_OK) return PROCESSOR_OK;
    else                        return (ProcessorErr_t) STATUS;
}


ProcessorErr_t check_signature(int* code)
{
    assert(code != NULL);

    int sign_err = PROCESSOR_OK;

    if (code[1] != VERSION)              sign_err |= BAD_VERSION;
    for(int i = 0; i < 4; i++) {
        if (*((char*)code + i) != PASSWORD[i]) {
            sign_err |= BAD_PASSWORD;
        }
    }
    return (ProcessorErr_t) sign_err;
}


ProcessorErr_t ProcessorDtor(SPU* spu)
{
    assert(spu != NULL);

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
    assert(spu != NULL);

    printf(CHANGE_ON GREEN TEXT_COLOR "ProcessorDump()\n" RESET);
    StackDump(&spu->stk);
    printf("code:\n");
    printf("PC:" CHANGE_ON PURPLE TEXT_COLOR);
    for (size_t i = 2; i < spu->file_size/sizeof(int); i++) {
        printf(CHANGE_ON PURPLE TEXT_COLOR);
        for (size_t j = i; ((j+1) % 30) != 0 && j < spu->file_size/sizeof(int); j++) {
            printf(" %3zu", j);
        }
        printf(RESET "\n   ");
        for (size_t j = i; ((j+1) % 30) != 0 && j < spu->file_size/sizeof(int); j++) {
            printf(" %3d", spu->code[j]);
        }
        printf("\n   ");
        for (; ((i+1) % 30) != 0 && i < spu->file_size/sizeof(int); i++) {
            if (i == spu->PC) printf("-" CHANGE_ON BOLD WITH RED TEXT_COLOR "^^^" RESET);
            else              printf("----");
        }
        printf(RESET"\n   ");
    }
    printf("\n" CHANGE_ON PURPLE TEXT_COLOR "       PASSWORD    " CHANGE_ON YELLOW TEXT_COLOR  "        VERSION\n" RESET);
    printf("regs:   AX = [%d], BX = [%d], CX = [%d], DX = [%d], SX[%d]\n", spu->regs[0], spu->regs[1], spu->regs[2], spu->regs[3], spu->regs[4]);
    return PROCESSOR_OK;
}


ProcessorErr_t ProcessorExe(SPU* spu)
{
    assert(spu != NULL);

    int* code = spu->code;
    size_t* PC = &spu->PC;
    stack_s* stk = &spu->stk;
    int* regs = spu->regs;

    while (code[*PC] != EOF) {
        switch(code[*PC]) {
            case PUSH: {
                stack_t data = code[++(*PC)];

                StackPush(stk, data);

                #ifdef DUMP
                    ProcessorDump(spu);
                    getchar();
                #endif
                break;
            }
            case ADD: {
                stack_t a = 0, b = 0;
                StackPop(stk, &a);
                StackPop(stk, &b);

                stack_t c = a + b;
                StackPush(stk, c);

                #ifdef DUMP
                    ProcessorDump(spu);
                    getchar();
                #endif
                break;
            }
            case SUB: {
                stack_t a = 0, b = 0;
                StackPop(stk, &a);
                StackPop(stk, &b);

                stack_t c = b - a;
                StackPush(stk, c);

                #ifdef DUMP
                    ProcessorDump(spu);
                    getchar();
                #endif
                break;
            }
            case DIV: {
                stack_t a = 0, b = 0;
                StackPop(stk, &a);
                StackPop(stk, &b);

                stack_t c = b / a;
                StackPush(stk, c);

                #ifdef DUMP
                    ProcessorDump(spu);
                    getchar();
                #endif
                break;
            }
            case OUT: {
                stack_t data = 0;
                StackPop(stk, &data);

                printf(CHANGE_ON BLUE TEXT_COLOR "answer = %10d\n" RESET, data);

                #ifdef DUMP
                    ProcessorDump(spu);
                    getchar();
                #endif
                break;
            }
            case MUL: {
                stack_t a = 0, b = 0;
                StackPop(stk, &a);
                StackPop(stk, &b);

                stack_t c = a * b;
                StackPush(stk, c);

                #ifdef DUMP
                    ProcessorDump(spu);
                    getchar();
                #endif
                break;
            }
            case POW: {
                stack_t a = 0, n = code[++(*PC)];
                StackPop(stk, &a);

                stack_t c = pow(a,n);
                StackPush(stk, c);

                #ifdef DUMP
                    ProcessorDump(spu);
                    getchar();
                #endif
                break;
            }
            case SQRT: {
                stack_t a = 0;
                StackPop(stk, &a);

                stack_t b = sqrt(a);
                StackPush(stk, b);

                #ifdef DUMP
                    ProcessorDump(spu);
                    getchar();
                #endif
                break;
            }
            case RESET_STK: {
                StackDtor(stk);
                StackCtor(stk, CAPACITY);

                #ifdef DUMP
                    ProcessorDump(spu);
                    getchar();
                #endif
                break;
            }
            case PUSHREG: {
                reg_t reg = (reg_t) code[++(*PC)];

                StackPush(stk, regs[reg]);

                #ifdef DUMP
                    ProcessorDump(spu);
                    getchar();
                #endif
                break;
            }
            case POPREG: {
                reg_t reg = (reg_t) code[++(*PC)];

                StackPop(stk, &regs[reg]);

                #ifdef DUMP
                    ProcessorDump(spu);
                    getchar();
                #endif
                break;
            }
            case JMP: {
                int new_pc = code[++(*PC) - 1];
                *PC = new_pc;

                #ifdef DUMP
                    ProcessorDump(spu);
                    getchar();
                #endif
                break;
            }
            case JB: {
                int a = 0, b = 0;
                StackPop(stk, &b);
                StackPop(stk, &a);

                if (a < b) {
                    int new_pc = code[++(*PC) - 1];
                    *PC = new_pc;
                } else (*PC)++;

                #ifdef DUMP
                    ProcessorDump(spu);
                    getchar();
                #endif
                break;
            }
            case JBE: {
                int a = 0, b = 0;
                StackPop(stk, &b);
                StackPop(stk, &a);

                if (a <= b) {
                    int new_pc = code[++(*PC) - 1];
                    *PC = new_pc;
                } else (*PC)++;

                #ifdef DUMP
                    ProcessorDump(spu);
                    getchar();
                #endif
                break;
            }
            case JA: {
                int a = 0, b = 0;
                StackPop(stk, &b);
                StackPop(stk, &a);

                if (a > b) {
                    int new_pc = code[++(*PC) - 1];
                    *PC = new_pc;
                } else (*PC)++;

                #ifdef DUMP
                    ProcessorDump(spu);
                    getchar();
                #endif
                break;
            }
            case JAE: {
                int a = 0, b = 0;
                StackPop(stk, &b);
                StackPop(stk, &a);

                if (a >= b) {
                    int new_pc = code[++(*PC) - 1];
                    *PC = new_pc;
                } else (*PC)++;

                #ifdef DUMP
                    ProcessorDump(spu);
                    getchar();
                #endif
                break;
            }
            case JE: {
                int a = 0, b = 0;
                StackPop(stk, &b);
                StackPop(stk, &a);

                if (a == b) {
                    int new_pc = code[++(*PC) - 1];
                    *PC = new_pc;
                } else (*PC)++;

                #ifdef DUMP
                    ProcessorDump(spu);
                    getchar();
                #endif
                break;
            }
            case JNE: {
                int a = 0, b = 0;
                StackPop(stk, &b);
                StackPop(stk, &a);

                if (a != b) {
                    int new_pc = code[++(*PC) - 1];
                    *PC = new_pc;
                } else (*PC)++;

                #ifdef DUMP
                    ProcessorDump(spu);
                    getchar();
                #endif
                break;
            }
            case IN: {
                int a = 0;
                printf("Write integer: ");
                scanf("%d", &a);

                StackPush(stk, a);

                #ifdef DUMP
                    ProcessorDump(spu);
                    getchar();
                #endif
                break;
            }
            case HLT:
                printf("You end the calc program\n");
                return PROCESSOR_OK;
            default:
                return COMMAND_NOT_FOUND;
        }
        (*PC)++;
    }
    return PROGRAM_END_MISSING;
}
