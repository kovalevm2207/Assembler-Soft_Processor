#include <stdio.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

#include "commands.h"
#include "ReadFile.h"
#include "my_stack.h"

const size_t MAX_LABELS_NUM = 20;
const char* FILE_NAME = /*"OUT_grustny.bin";*/ /*"OUT_BAD_APPLE.bin";*/ "Work_With_RAM.bin";  /*"factorial.bin";*/  /*"NotLineSquareSolver.bin";*/  /*"example2.bin";*/

typedef enum
{
    PROCESSOR_OK         = 1 << 1,
    COMMAND_NOT_FOUND    = 1 << 2,
    PROGRAM_END_MISSING  = 1 << 3,
    BAD_PUSH             = 1 << 4,
    BAD_SIGNATURE        = 1 << 5,
    BAD_VERSION          = 1 << 6
} ProcessorErr_t;

ProcessorErr_t ProcessorCtor(SPU* spu, int* code, int* RAM);
ProcessorErr_t check_heder(int* code);
ProcessorErr_t ProcessorExe(SPU* spu);
ProcessorErr_t ProcessorDtor(SPU* spu);
ProcessorErr_t ProcessorDump(SPU* spu);

int main()
{
    SPU spu = {};

    FILE* file = fopen(FILE_NAME, "rb");
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

    int* RAM = (int*) calloc(9213, sizeof(int));
    assert(RAM != NULL);

    int STATUS = ProcessorCtor(&spu, code, RAM);
    if(STATUS != PROCESSOR_OK) {
        printf(CHANGE_ON RED TEXT_COLOR "ERROR! PROCESSOR_CTOR FAILED; STATUS = %d\n", STATUS);
        if (STATUS & BAD_VERSION) printf("BAD_VERSION(%d)\n", BAD_VERSION);
        if (STATUS & BAD_SIGNATURE) printf("BAD_SIGNATURE(%d)\n" RESET, BAD_SIGNATURE);
        ProcessorDtor(&spu);
        return 1;
    }

    fclose(file);

    ProcessorExe(&spu);

    ProcessorDtor(&spu);

    return 0;
}


ProcessorErr_t ProcessorCtor(SPU* spu, int* code, int* RAM)
{
    assert(spu != NULL);
    assert(code != NULL);
    assert(RAM != NULL);

    int STATUS = PROCESSOR_OK;
    STATUS |= check_heder(code);

    stack_s stk = {};
    STATUS |= StackCtor(&stk, CAPACITY);

    stack_s labels = {};
    STATUS |= StackCtor(&labels, MAX_LABELS_NUM);

    spu->code = code;
    spu->PC = 2;
    spu->stk = stk;
    spu->labels = labels;
    spu->RAM = RAM;
    for (int i = 0; i < 4; i++) {
        spu->regs[i] = 0;
    }

    if (STATUS == PROCESSOR_OK) return PROCESSOR_OK;
    else                        return (ProcessorErr_t) STATUS;
}


ProcessorErr_t check_heder(int* code)
{
    assert(code != NULL);

    int sign_err = PROCESSOR_OK;
    if (code[1] != VERSION)              sign_err |= BAD_VERSION;
    for(int i = 0; i < 4; i++) {
        if (*((char*)code + i) != SIGNATURE[i]) {
            sign_err |= BAD_SIGNATURE;
        }
    }
    return (ProcessorErr_t) sign_err;
}


ProcessorErr_t ProcessorDtor(SPU* spu)
{
    assert(spu != NULL);

    StackDtor(&spu->stk);
    StackDtor(&spu->labels);
    spu->PC = 0;
    for (int i = 0; i < 4; i++) {
        spu->regs[i] = 0;
    }
    free(spu->code);
    free(spu->RAM);

    return PROCESSOR_OK;
}


ProcessorErr_t ProcessorDump(SPU* spu)
{
    assert(spu != NULL);

    printf(CHANGE_ON GREEN TEXT_COLOR "ProcessorDump()\n" RESET);
    StackDump(&spu->stk);
    printf("code:\n");
    printf("PC:" CHANGE_ON PURPLE TEXT_COLOR);
    for (size_t i = 2; i < spu->file_size/sizeof(int);) {
        printf(CHANGE_ON PURPLE TEXT_COLOR);
        for (size_t j = i; j < i + 30 && j < spu->file_size/sizeof(int); j++) {
            printf(" %3zu", j);
        }
        printf(RESET "\n   ");
        for (size_t j = i; j < i + 30 && j < spu->file_size/sizeof(int); j++) {
            printf(" %3d", spu->code[j]);
        }
        printf("\n   ");
        for (size_t j = i; j < i + 30 && j < spu->file_size/sizeof(int); j++) {
            if (j == spu->PC) printf("-" CHANGE_ON BOLD WITH RED TEXT_COLOR "^^^" RESET);
            else              printf("----");
        }
        printf(RESET"\n   ");

        size_t next_i = i + 30;
        if (next_i >= spu->file_size/sizeof(int)) break;
        i = next_i;
    }
    printf("\n" CHANGE_ON PURPLE TEXT_COLOR "       SIGNATURE    " CHANGE_ON YELLOW TEXT_COLOR  "        VERSION\n" RESET);
    printf("regs:   AX = [%d], BX = [%d], CX = [%d], DX = [%d], SX[%d]\n", spu->regs[0], spu->regs[1], spu->regs[2], spu->regs[3], spu->regs[4]);
    return PROCESSOR_OK;
}


ProcessorErr_t ProcessorExe(SPU* spu)
{
    assert(spu != NULL);

    int* code = spu->code;
    size_t* PC = &spu->PC;
    int current_command = -1;

    while (code[*PC] != EOF) {
        current_command = code[*PC];
        assert(current_command > -1);
        assert(current_command < COMMANDS_NUM);

        for (int i = 0; i < COMMANDS_NUM; i++) {
            if (current_command == commands[i].num) {
                commands[i].func_exe(spu, &commands[i]);
            }
        }

        if (current_command == HLT) return PROCESSOR_OK;

        (*PC)++;

        #ifdef DUMP
            ProcessorDump(spu);
            getchar();
        #endif
    }
    return PROGRAM_END_MISSING;
}
