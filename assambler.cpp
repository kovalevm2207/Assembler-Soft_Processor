// todo: заменить strcmpr(), на hash + switch
// todo: сделать так,чтобы программа пропускала пустые строки
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include "commands.h"
#include "color_print.h"
#include "ReadFile.h"

const int MAX_COMMAND_LENGTH = 20;
const int MAX_REG_LENGTH = 8;
const char* FILE_NAME = "NotLineSquareSolver.asm";//"example2.asm";
const char* CREATE_FILE = "NotLineSquareSolver.bin";//"example2.bin";      // todo: read about strcat

typedef enum
{
    ASSEMBLER_OK = 1 << 1,
    NOTFOUND_REG = 1 << 2,
    NOTFOUND_ARG = 1 << 3
} AssemblerErr_t;

struct translator_s
{
    size_t linenum;
    size_t count_line;
    size_t code_num;
    line* lines;
    command_t command;
    int* codes;
};


command_t find_and_translate_all_commands(translator_s* translator);
void create_signature(int* code);
void translate_command(translator_s* translator);
void do_help(void);
AssemblerErr_t find_reg(translator_s* translator);
AssemblerErr_t find_arg(translator_s* translator);
void print_result_code(translator_s* translator);
int write_result_in_text_file(translator_s* translator);
//todo: struct hadder{signature, version, PC};
int main()
{
    size_t file_size0 = find_file_size(FILE_NAME);

    char* command_text = (char*) calloc(file_size0 + 1, sizeof(char));
    assert(command_text != NULL);

    int file = open(FILE_NAME, O_RDONLY);
    assert(file != 0);

    size_t file_size = read (file, command_text, file_size0 + 1);
    assert(file_size != 0);
    command_text[file_size] = '\0';

    close(file);

    size_t linenum = count_lines(command_text);
    assert(linenum > 0);

    line *command_lines = (line*) calloc(linenum, sizeof(line));
    assert(command_lines != 0);
    get_lines(command_lines, command_text);

    int* command_codes = (int*) calloc(linenum * 2 + 1 /*SIGNATURE*/ + 1 /*version*/, sizeof(int));
    assert(command_codes != NULL);

    create_signature(command_codes);

    translator_s translator = {
        .linenum = linenum,
        .count_line = 0,
        .code_num = 2,
        .lines = command_lines,
        .command = START,
        .codes = command_codes
    };

    if (find_and_translate_all_commands(&translator) == INVALID_COMMAND) {
        printf(CHANGE_ON RED TEXT_COLOR "INVALID_COMMAND, please read help:\n" RESET);
        do_help();
        return 1;
    }

    write_result_in_text_file(&translator);

    free(command_text);
    free(command_lines);
    free(command_codes);

    return 0;
}


void create_signature(int* code)
{
    memcpy(code, SIGNATURE, 4);
    code[1] = VERSION;
}


command_t find_and_translate_all_commands(translator_s* translator)
{
    assert(translator != NULL);

    char command[MAX_COMMAND_LENGTH] = {};

    for (; translator->count_line < translator->linenum; translator->count_line++) {
        sscanf(translator->lines[translator->count_line].ptr, "%s", command);

        if (strcmp(command,"PUSH") == 0) {
            #ifdef DUMP
                printf("%3zu %s  ", translator->code_num, command);
            #endif

            translator->command = PUSH;
        }
        else if (strcmp(command,"POW") == 0) {
            #ifdef DUMP
                printf("%3zu %s  ", translator->code_num, command);
            #endif

            translator->command = POW;
        }
        else if (strcmp(command,"PUSHREG") == 0) {
            #ifdef DUMP
                printf("%3zu %s  ", translator->code_num, command);
            #endif

            translator->command = PUSHREG;
        }
        else if (strcmp(command,"POPREG") == 0) {
            #ifdef DUMP
                printf("%3zu %s  ", translator->code_num, command);
            #endif

            translator->command = POPREG;
        }
        else if (strcmp(command,"JMP") == 0) {
            #ifdef DUMP
                printf("%3zu %s  ", translator->code_num, command);
            #endif

            translator->command = JMP;
        }
        else if (strcmp(command,"JB") == 0) {
            #ifdef DUMP
                printf("%3zu %s  ", translator->code_num, command);
            #endif

            translator->command =  JB;
        }
        else if (strcmp(command,"JBE") == 0) {
            #ifdef DUMP
                printf("%3zu %s  ", translator->code_num, command);
            #endif

            translator->command = JBE;
        }
        else if (strcmp(command,"JA") == 0) {
            #ifdef DUMP
                printf("%3zu %s  ", translator->code_num, command);
            #endif

            translator->command =  JA;
        }
        else if (strcmp(command,"JAE") == 0) {
            #ifdef DUMP
                printf("%3zu %s  ", translator->code_num, command);
            #endif

            translator->command = JAE;
        }
        else if (strcmp(command,"JE") == 0) {
            #ifdef DUMP
                printf("%3zu %s  ", translator->code_num, command);
            #endif

            translator->command =  JE;
        }
        else if (strcmp(command,"JNE") == 0) {
            #ifdef DUMP
                printf("%3zu %s  ", translator->code_num, command);
            #endif

            translator->command = JNE;
        }
        else if (strcmp(command,"IN") == 0) {
            #ifdef DUMP
                printf("%3zu %s  ", translator->code_num, command); getchar();
            #endif

            translator->command =  IN;
        }
        else if (strcmp(command,"ADD") == 0) {
            #ifdef DUMP
                printf("%3zu %s  ", translator->code_num, command); getchar();
            #endif

            translator->command = ADD;
        }
        else if (strcmp(command,"SUB") == 0) {
            #ifdef DUMP
                printf("%3zu %s  ", translator->code_num, command); getchar();
            #endif

            translator->command = SUB;
        }
        else if (strcmp(command,"DIV") == 0) {
            #ifdef DUMP
                printf("%3zu %s  ", translator->code_num, command); getchar();
            #endif

            translator->command = DIV;
        }
        else if (strcmp(command,"OUT") == 0) {
            #ifdef DUMP
                printf("%3zu %s  ", translator->code_num, command); getchar();
            #endif

            translator->command = OUT;
        }
        else if (strcmp(command,"MUL") == 0) {
            #ifdef DUMP
                printf("%3zu %s  ", translator->code_num, command); getchar();
            #endif

            translator->command = MUL;
        }
        else if (strcmp(command,"SQRT") == 0) {
            #ifdef DUMP
                printf("%3zu %s  ", translator->code_num, command); getchar();
            #endif

            translator->command = SQRT;
        }
        else if (strcmp(command,"HLT") == 0) {
            #ifdef DUMP
                printf("%3zu %s  ", translator->code_num, command); getchar();
            #endif

            translator->command = HLT;
        }
        else if (strcmp(command,"RESET") == 0) {
            #ifdef DUMP
                printf("%3zu %s  ", translator->code_num, command); getchar();
            #endif

            translator->command = RESET_STK;
        }
        else return INVALID_COMMAND;

        translate_command(translator);

        memset(command, 0, sizeof(command));
    }

    return translator->command;
}


void translate_command(translator_s* translator)
{
    assert(translator != NULL);

    int* codes = translator->codes;
    command_t command = translator->command;
    size_t* count = &(translator->code_num);

    switch (command) {
        case HLT:
                codes[(*count)++] = HLT;
                break;
        case HELP:
                do_help();
                break;
        case PUSH:
                codes[(*count)++] = PUSH;
                find_arg(translator);
                break;
        case ADD:
                codes[(*count)++] = ADD;
                break;
        case SUB:
                codes[(*count)++] = SUB;
                break;
        case DIV:
                codes[(*count)++] = DIV;
                break;
        case OUT:
                codes[(*count)++] = OUT;
                break;
        case MUL:
                codes[(*count)++] = MUL;
                break;
        case POW:
                codes[(*count)++] = POW;
                find_arg(translator);
                break;
        case SQRT:
                codes[(*count)++] = SQRT;
                break;
        case RESET_STK:
                codes[(*count)++] = RESET_STK;
                break;
        case PUSHREG:
                codes[(*count)++] = PUSHREG;
                find_reg(translator);
                break;
        case POPREG:
                codes[(*count)++] = POPREG;
                find_reg(translator);
                break;
        case JMP:
                codes[(*count)++] = JMP;
                find_arg(translator);
                break;
        case  JB:
                codes[(*count)++] = JB;
                find_arg(translator);
                break;
        case JBE:
                codes[(*count)++] = JBE;
                find_arg(translator);
                break;
        case  JA:
                codes[(*count)++] = JA;
                find_arg(translator);
                break;
        case JAE:
                codes[(*count)++] = JAE;
                find_arg(translator);
                break;
        case  JE:
                codes[(*count)++] = JE;
                find_arg(translator);
                break;
        case JNE:
                codes[(*count)++] = JNE;
                find_arg(translator);
                break;
        case IN:
                codes[(*count)++] = IN;
                break;
        case START:
        case INVALID_COMMAND:
                break;
        default:
            break;
    }
}


AssemblerErr_t find_reg(translator_s* translator)
{
    assert(translator != NULL);

    char reg[MAX_REG_LENGTH] = {};
    if (sscanf(translator->lines[translator->count_line].ptr,"%*s %s", reg) != 1) {
        printf(CHANGE_ON RED TEXT_COLOR "Missing reg for a function that must have an reg\n" RESET);
        do_help();
        return NOTFOUND_REG;
    }

    if(strcmp(reg, "AX") == 0) {
        #ifdef DUMP
            printf("reg %s", reg);
            getchar();
        #endif
        translator->codes[translator->code_num++] = AX;
    }
    else if(strcmp(reg, "BX") == 0) {
        #ifdef DUMP
            printf("reg %s", reg);
            getchar();
        #endif
        translator->codes[translator->code_num++] = BX;
    }
    else if(strcmp(reg, "CX") == 0) {
        #ifdef DUMP
            printf("reg %s", reg);
            getchar();
        #endif
        translator->codes[translator->code_num++] = CX;
    }
    else if(strcmp(reg, "DX") == 0) {
        #ifdef DUMP
            printf("reg %s", reg);
            getchar();
        #endif
        translator->codes[translator->code_num++] = DX;
    }
    else if(strcmp(reg, "SX") == 0) {
        #ifdef DUMP
            printf("reg %s", reg);
            getchar();
        #endif
        translator->codes[translator->code_num++] = SX;
    }
    else {
        printf(CHANGE_ON RED TEXT_COLOR "Invalid reg\n" RESET);
        do_help();
        return NOTFOUND_REG;
    }
    return ASSEMBLER_OK;
}


AssemblerErr_t find_arg(translator_s* translator)
{
    assert(translator != NULL);

    int data = 0;

    if (sscanf(translator->lines[translator->count_line].ptr,"%*s %d", &data) != 1) {
        printf(CHANGE_ON RED TEXT_COLOR "Missing argument for a function that must have an argument\n" RESET);
        do_help();
        return NOTFOUND_ARG;
    }
    #ifdef DUMP
        printf("arg = %d", data);
        getchar();
    #endif
    translator->codes[translator->code_num++] = data;
    return ASSEMBLER_OK;
}


int write_result_in_text_file(translator_s* translator)
{
    translator->codes[translator->code_num++] = '\0';
    #ifdef DUMP
        print_result_code(translator);
    #endif

    FILE* file = fopen(CREATE_FILE, "wb");

    size_t code_size = translator->code_num * sizeof(int);

    fwrite(&code_size, sizeof(size_t), 1, file);
    fwrite(translator->codes, sizeof(int), translator->code_num, file);

    fclose(file);
    return 0;
}


void print_result_code(translator_s* translator)
{
    printf("Result codes: ");
        for (size_t i = 0; i < translator->code_num; i++) {
            printf("%d ", translator->codes[i]);
        }
        printf("\n");
}


void do_help(void)
{
    printf("\nusage: main_calc " CHANGE_ON BLUE TEXT_COLOR "[HELP] [HLT] [PUSH] [OUT]\n"
           "                 [SUB] [DIV] [MUL] [POW] [SQRT]\n"
           RESET "\n"
	       "These are common main_calc commands used in various situations:\n"
           "You can use " CHANGE_ON PURPLE TEXT_COLOR "only integers" RESET " value\n"
	       "\n"
           CHANGE_ON GREEN TEXT_COLOR "HELP     " RESET "         cli flags help message\n"
	       CHANGE_ON GREEN TEXT_COLOR "HLT      " RESET "         HaLT (end the program)\n"
           CHANGE_ON GREEN TEXT_COLOR "PUSH  num" RESET "         save the value (num) to calculator memory\n"
           CHANGE_ON GREEN TEXT_COLOR "OUT      " RESET "         retrieve the last saved value from calculator memory\n"
           CHANGE_ON GREEN TEXT_COLOR "ADD      " RESET "         add the two last numbers stored in memory\n"
           CHANGE_ON GREEN TEXT_COLOR "SUB      " RESET "         subtract the last number in memory from the second last number in memory\n"
           CHANGE_ON GREEN TEXT_COLOR "DIV      " RESET "         divide the last number stored in memory by the second last number stored in memory\n"
           CHANGE_ON GREEN TEXT_COLOR "MUL      " RESET "         multiply the two last numbers stored in memory\n"
           CHANGE_ON GREEN TEXT_COLOR "POW   num" RESET "         raise the last number stored in memory to a power (num)\n"
           CHANGE_ON GREEN TEXT_COLOR "SQRT     " RESET "         calculate the square root of the last number in memory\n"
           "\n");
}
