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
#include "my_stack.h"

const int MAX_COMMAND_LENGTH = 20;
const int MAX_REG_LENGTH = 8;
const int MAX_LABELS_NUM = 10;
const char* FILE_NAME = "Work_With_RAM.asm";  /*"factorial.asm";*/  /*"NotLineSquareSolver.asm";*/   /*"example2.asm";*/
const char* CREATE_FILE = "Work_With_RAM.bin";  /*"factorial.bin";*/  /*"NotLineSquareSolver.bin";*/   /*"example2.bin";*/      //todo: read about strcat

typedef enum
{
    ASSEMBLER_OK   = 1 << 1,
    NOTFOUND_REG   = 1 << 2,
    NOTFOUND_ARG   = 1 << 3,
    INCORRECT_REG  = 1 << 4,
    FIND_REG_ERR   = 1 << 5,
    FIND_ARG_ERR   = 1 << 6,
    FIND_MEM_I_ERR = 1 << 7
} AssemblerErr_t;

struct translator_s
{
    size_t linenum;
    size_t count_line;
    int code_num;
    line* lines;
    int* codes;
    int* labels;
};


AssemblerErr_t find_and_translate_all_commands(translator_s* translator);
void create_signature(int* code);
void translate_command(translator_s* translator);
void do_help(void);
AssemblerErr_t find_reg(translator_s* translator);
AssemblerErr_t find_arg(translator_s* translator);
AssemblerErr_t find_mem_i(translator_s* translator);
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

    int labels[MAX_LABELS_NUM] = {};

    translator_s translator = {
        .linenum = linenum,
        .count_line = 0,
        .code_num = 2,
        .lines = command_lines,
        .codes = command_codes,
        .labels = labels,
    };


    find_and_translate_all_commands(&translator);
    translator.count_line = 0;
    translator.code_num = 2;
    find_and_translate_all_commands(&translator);

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


AssemblerErr_t find_and_translate_all_commands(translator_s* translator)
{
    assert(translator != NULL);

    char command[MAX_COMMAND_LENGTH] = {};

    int* labels = translator->labels;
    int* count = &(translator->code_num);
    int* codes = translator->codes;

    for (; translator->count_line < translator->linenum; translator->count_line++) {

        sscanf(translator->lines[translator->count_line].ptr, "%s", command);
        if (strcmp(command,"PUSH") == 0) {
            #ifdef DUMP
                printf("%3d %s  ", *count, command);
            #endif

            codes[(*count)++] = PUSH;
            if (find_arg(translator) != ASSEMBLER_OK) return FIND_ARG_ERR;
        }
        else if (strcmp(command,"POW") == 0) {
            #ifdef DUMP
                printf("%3d %s  ", *count, command);
            #endif

            codes[(*count)++] = POW;
            if (find_arg(translator) != ASSEMBLER_OK) return FIND_ARG_ERR;
        }
        else if (strcmp(command,"PUSHREG") == 0) {
            #ifdef DUMP
                printf("%3d %s  ", *count, command);
            #endif

            codes[(*count)++] = PUSHREG;
            if (find_reg(translator) != ASSEMBLER_OK) return FIND_REG_ERR;
        }
        else if (strcmp(command,"POPREG") == 0) {
            #ifdef DUMP
                printf("%3d %s  ", *count, command);
            #endif

            codes[(*count)++] = POPREG;
            if (find_reg(translator) != ASSEMBLER_OK) return FIND_REG_ERR;
        }
        else if (strcmp(command,"CALL") == 0) {
            #ifdef DUMP
                printf("%3d %s  ", *count, command); getchar();
            #endif

            codes[(*count)++] = CALL;

            int label = 0;
            sscanf(translator->lines[translator->count_line].ptr, "%*s :%d", &label);
            codes[(*count)++] = labels[label];
        }
        else if (strcmp(command,"RET") == 0) {
            #ifdef DUMP
                printf("%3d %s  ", *count, command);
            #endif

            codes[(*count)++] = RET;
        }
        else if (strcmp(command,"JMP") == 0) {
            codes[(*count)++] = JMP;

            int label = 0;
            sscanf(translator->lines[translator->count_line].ptr, "%*s :%d", &label);
            codes[(*count)++] = labels[label];

            #ifdef DUMP
                printf("%3d %s  ", *count, command); getchar();
                printf(":%d or JMP %d", label, labels[label]); getchar();
            #endif
        }
        else if (strcmp(command,"JB") == 0) {
            codes[(*count)++] = JB;

            int label = 0;
            sscanf(translator->lines[translator->count_line].ptr, "%*s :%d", &label);
            #ifdef DUMP
                printf("%3d %s  ", *count, command);
                printf(":%d or JB %d", label, labels[label]); getchar();
            #endif
            codes[(*count)++] = labels[label];
        }
        else if (strcmp(command,"JBE") == 0) {
            codes[(*count)++] = JBE;

            int label = 0;
            sscanf(translator->lines[translator->count_line].ptr, "%*s :%d", &label);
            #ifdef DUMP
                printf("%3d %s  ", *count, command);
                printf(":%d or JBE %d", label, labels[label]); getchar();
            #endif
            codes[(*count)++] = labels[label];
        }
        else if (strcmp(command,"JA") == 0) {
            codes[(*count)++] = JA;

            int label = 0;
            sscanf(translator->lines[translator->count_line].ptr, "%*s :%d", &label);
            #ifdef DUMP
                printf("%3d %s  ", *count, command);
                printf(":%d or JA %d", label, labels[label]); getchar();
            #endif
            codes[(*count)++] = labels[label];
        }
        else if (strcmp(command,"JAE") == 0) {
            codes[(*count)++] = JAE;

            int label = 0;
            sscanf(translator->lines[translator->count_line].ptr, "%*s :%d", &label);
            #ifdef DUMP
                printf("%3d %s  ", *count, command);
                printf(":%d or JAE %d", label, labels[label]); getchar();
            #endif
            codes[(*count)++] = labels[label];
        }
        else if (strcmp(command,"JE") == 0) {
            codes[(*count)++] = JE;

            int label = 0;
            sscanf(translator->lines[translator->count_line].ptr, "%*s :%d", &label);
            #ifdef DUMP
                printf("%3d %s  ", *count, command);
                printf(":%d or JE %d", label, labels[label]); getchar();
            #endif
            codes[(*count)++] = labels[label];
        }
        else if (strcmp(command,"JNE") == 0) {
            codes[(*count)++] = JNE;

            int label = 0;
            sscanf(translator->lines[translator->count_line].ptr, "%*s :%d", &label);
            #ifdef DUMP
                printf("%3d %s  ", *count, command);
                printf(":%d or JNE %d", label, labels[label]); getchar();
            #endif
            codes[(*count)++] = labels[label];
        }
        else if (strcmp(command,"IN") == 0) {
            #ifdef DUMP
                printf("%3d %s  ", *count, command); getchar();
            #endif

            codes[(*count)++] = IN;
        }
        else if (strcmp(command,"ADD") == 0) {
            #ifdef DUMP
                printf("%3d %s  ", *count, command); getchar();
            #endif

            codes[(*count)++] = ADD;
        }
        else if (strcmp(command,"SUB") == 0) {
            #ifdef DUMP
                printf("%3d %s  ", *count, command); getchar();
            #endif

            codes[(*count)++] = SUB;
        }
        else if (strcmp(command,"MOD") == 0) {
            #ifdef DUMP
                printf("%3d %s  ", *count, command); getchar();
            #endif

            codes[(*count)++] = MOD;
        }
        else if (strcmp(command,"DIV") == 0) {
            #ifdef DUMP
                printf("%3d %s  ", *count, command); getchar();
            #endif

            codes[(*count)++] = DIV;
        }
        else if (strcmp(command,"OUT") == 0) {
            #ifdef DUMP
                printf("%3d %s  ", *count, command); getchar();
            #endif

            codes[(*count)++] = OUT;
        }
        else if (strcmp(command,"MUL") == 0) {
            #ifdef DUMP
                printf("%3d %s  ", *count, command); getchar();
            #endif

            codes[(*count)++] = MUL;
        }
        else if (strcmp(command,"SQRT") == 0) {
            #ifdef DUMP
                printf("%3d %s  ", *count, command); getchar();
            #endif

            codes[(*count)++] = SQRT;
        }
        else if (strcmp(command,"PUSHM") == 0) {
            #ifdef DUMP
                printf("%3d %s  ", *count, command);
            #endif

            codes[(*count)++] = PUSHM;
            if(find_reg(translator) != ASSEMBLER_OK) return FIND_REG_ERR;
        }
        else if (strcmp(command,"POPM") == 0) {
            #ifdef DUMP
                printf("%3d %s  ", *count, command);
            #endif

            codes[(*count)++] = POPM;
            if(find_reg(translator) != ASSEMBLER_OK) return FIND_REG_ERR;
        }
        else if (strcmp(command,"DRAW") == 0) {
            #ifdef DUMP
                printf("%3d %s  ", *count, command); getchar();
            #endif

            codes[(*count)++] = DRAW;
        }
        else if (strcmp(command,"HLT") == 0) {
            #ifdef DUMP
                printf("%3d %s  ", *count, command); getchar();
            #endif

            codes[(*count)++] = HLT;
        }
        else if (strcmp(command,"RESET") == 0) {
            #ifdef DUMP
                printf("%3d %s  ", *count, command); getchar();
            #endif

            codes[(*count)++] = RESET_STK;
        }
        else {
            int label = 0;
            if (sscanf(translator->lines[translator->count_line].ptr, " :%d", &label) == 1) {
                labels[label] = *count;
                #ifdef DUMP
                    printf(":%d or count = %d\n", label, *count);
                #endif
            }
            else return (AssemblerErr_t) INVALID_COMMAND;
        }
        memset(command, 0, sizeof(command));
    }

    return ASSEMBLER_OK;
}


AssemblerErr_t find_reg(translator_s* translator)
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
            do_help();
            return NOTFOUND_REG;
        }
    } else {
        #ifdef DUMP
            printf("[");
            MODE = MEM_I
        #endif
        ;
    }

    if(strcmp(reg, "AX") == 0) {
        #ifdef DUMP
            printf("%s", reg);
        #endif
        translator->codes[translator->code_num++] = AX;
    }
    else if(strcmp(reg, "BX") == 0) {
        #ifdef DUMP
            printf("%s", reg);
        #endif
        translator->codes[translator->code_num++] = BX;
    }
    else if(strcmp(reg, "CX") == 0) {
        #ifdef DUMP
            printf("%s", reg);
        #endif
        translator->codes[translator->code_num++] = CX;
    }
    else if(strcmp(reg, "DX") == 0) {
        #ifdef DUMP
            printf("%s", reg);
        #endif
        translator->codes[translator->code_num++] = DX;
    }
    else if(strcmp(reg, "SX") == 0) {
        #ifdef DUMP
            printf("%s", reg);
        #endif
        translator->codes[translator->code_num++] = SX;
    }
    else if(strcmp(reg, "RX") == 0) {
        #ifdef DUMP
            printf("%s", reg);
        #endif
        translator->codes[translator->code_num++] = RX;
    }
    else {
        printf(CHANGE_ON RED TEXT_COLOR "Invalid reg\n" RESET);
        do_help();
        return INCORRECT_REG;
    }
    #ifdef DUMP
        if (MODE == MEM_I) printf("]");
        getchar();
    #endif

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

    size_t code_size = (size_t) translator->code_num * sizeof(int);

    fwrite(&code_size, sizeof(size_t), 1, file);
    fwrite(translator->codes, sizeof(int), translator->code_num, file);

    fclose(file);
    return 0;
}


void print_result_code(translator_s* translator)
{
    printf("Result codes: ");
        for (int i = 0; i < translator->code_num; i++) {
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
