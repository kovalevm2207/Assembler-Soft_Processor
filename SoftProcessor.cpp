// todo: hash !?
#include <stdio.h>
#include <math.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

#include "commands.h"
#include "ReadFile.h"
#include "my_stack.h"

typedef enum
{
    PROCESSOR_OK         = 1 << 1,
    PROGRAM_END_MISSING  = 1 << 2,
    BAD_SIGNATURE        = 1 << 3,
    BAD_VERSION          = 1 << 4,
    READ_FILE_ERR        = 1 << 5,
    MAIN_ARG_NUM_ERR     = 1 << 6,
    COMMANDS_NUM_ERR     = 1 << 7,
    COMMAND_EXE_ERR      = 1 << 8

} ProcessorErr_t;

ProcessorErr_t MainArgAnalyze(int argc);
int* ReadBin(char* argv, SPU* spu);
ProcessorErr_t ProcessorCtor(SPU* spu, int* code);
ProcessorErr_t check_heder(int* code);
ProcessorErr_t check_commands_num(const command_s* arr_struct);
ProcessorErr_t ProcessorExe(SPU* spu);
ProcessorErr_t ProcessorDtor(SPU* spu);
ProcessorErr_t ProcessorDump(SPU* spu);

int main(int argc, char* argv[])
{
    system("clear");

    if (MainArgAnalyze(argc) != PROCESSOR_OK) return 1;

    SPU spu = {};
    int* code = ReadBin(argv[1],&spu);
    if(code == NULL) return 1;
    int STATUS = ProcessorCtor(&spu, code);
    if(STATUS != PROCESSOR_OK) {
        printf(CHANGE_ON RED TEXT_COLOR "ERROR! PROCESSOR_CTOR FAILED; STATUS = %d\n", STATUS);
        if (STATUS & BAD_VERSION) printf("BAD_VERSION(%d)\n", BAD_VERSION);
        if (STATUS & BAD_SIGNATURE) printf("BAD_SIGNATURE(%d)\n" RESET, BAD_SIGNATURE);
        ProcessorDtor(&spu);
        return 1;
    }

    ProcessorExe(&spu);

    ProcessorDtor(&spu);

    return 0;
}

ProcessorErr_t MainArgAnalyze(int argc)
{
    if (argc != 2) {
        printf(CHANGE_ON RED TEXT_COLOR "You should write: ./assambler <file name from cur dir>.bin\n" RESET);
        return MAIN_ARG_NUM_ERR;
    }
    else return PROCESSOR_OK;
}


int* ReadBin(char* argv, SPU* spu)
{
    FILE* file = fopen(argv, "rb");
    assert(file != NULL);

    if (fread(&(spu->file_size), sizeof(size_t), 1, file) != 1) {
        printf(CHANGE_ON RED TEXT_COLOR "ERROR! FILE SIZE READ FAILED\n" RESET);
        return NULL;
    }

    int* code = (int*) calloc(spu->file_size, sizeof(char));
    assert(code != NULL);

    if (fread(code, sizeof(int), spu->file_size/sizeof(int), file) != spu->file_size/sizeof(int)) {
        printf(CHANGE_ON RED TEXT_COLOR "ERROR! CODE READ FAILED\n" RESET);
        return NULL;
    }

    fclose(file);
    return code;
}


ProcessorErr_t ProcessorCtor(SPU* spu, int* code)
{
    assert(spu != NULL);

    int* RAM = (int*) calloc(RAM_SIZE, sizeof(int));
    assert(RAM != NULL);

    int STATUS = PROCESSOR_OK;
    STATUS |= check_heder(code);

    STATUS |= check_commands_num(commands);

    stack_s stk = {};
    STATUS |= StackCtor(&stk, CAPACITY);

    stack_s labels = {};
    STATUS |= StackCtor(&labels, MAX_LABELS_NUM);

    spu->code = code;
    spu->PC = START_PC;                        //define 2 KoV исправил (былл просто 2) Мика
    spu->stk = stk;
    spu->labels = labels;
    spu->RAM = RAM;
    /*for (int i = 0; i < 5; i++) {      //func and define KoV
        spu->regs[i] = 0;
    }*/
    memset(spu->RAM, 0, sizeof(spu->regs));       // переделал на memset  Мика

    if (STATUS == PROCESSOR_OK) return PROCESSOR_OK;
    else                        return (ProcessorErr_t) STATUS;
}

ProcessorErr_t check_heder(int* code)
{
    assert(code != NULL);

    int sign_err = PROCESSOR_OK;
    if (code[1] != heder::VERSION) sign_err |= BAD_VERSION;
    for(size_t i = 0; i < sizeof(heder::SIGNATURE)/sizeof(char); i++) {                  // ну ладно 4 значит не size_t KoV
        if (*((char*)code + i) != heder::SIGNATURE[i]) sign_err |= BAD_SIGNATURE;        // переделал на sizeof()/sizeof() Мика
    }
    return (ProcessorErr_t) sign_err;
}

ProcessorErr_t check_commands_num(const command_s* arr_struct)
{
    int STATUS = PROCESSOR_OK;
    for (int i = 0; i < COMMANDS_NUM; i++) {                                //size_t KoV не буду менять, писал уже почему Мика
        if (arr_struct[i].num != i) {
            printf(CHANGE_ON RED TEXT_COLOR "!!!ERROR!!! номера команд не соответствуют их позиции в массиве структур\n" RESET
                                            "Ошибка произошла в команде %s\n"
                   CHANGE_ON RED TEXT_COLOR "Сейчас:      номер команды %d        позиция в массиве структур %d\n" RESET
                 CHANGE_ON GREEN TEXT_COLOR "Должно быть: номер команды %d        позиция в массиве структур %d\n" RESET,
                                            arr_struct[i].name, arr_struct[i].num, i, arr_struct[i].num, arr_struct[i].num);
            STATUS = COMMANDS_NUM_ERR;
        }
    }
    return (ProcessorErr_t) STATUS;
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

        if (commands[current_command].func_exe(spu, &commands[current_command]) != CMD_OK)  { // так как в массиве структур номер команды совпадает
            // Processor_dump()        когда-то точно будет                                  // с номером ее ячейки в массиве
            printf(RED_COLOR "Ошибка исполнения команды\n" RESET);
            return COMMAND_EXE_ERR;
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


ProcessorErr_t ProcessorDtor(SPU* spu)
{
    assert(spu != NULL);

    StackDtor(&spu->stk);
    StackDtor(&spu->labels);
    spu->PC = 2;
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
