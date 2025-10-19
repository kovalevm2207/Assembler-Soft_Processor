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
const char* FILE_NAME = /*"OUT_grustny.asm";*/ /*"OUT_BAD_APPLE.asm";*/  "Work_With_RAM.asm";  /*"factorial.asm";*/  /*"NotLineSquareSolver.asm";*/   /*"example2.asm";*/
const char* CREATE_FILE = /*"OUT_grustny.bin";*/ /*"OUT_BAD_APPLE.bin";*/  "Work_With_RAM.bin";  /*"factorial.bin";*/  /*"NotLineSquareSolver.bin";*/   /*"example2.bin";*/      //todo: read about strcat

typedef enum
{
    ASSEMBLER_OK        = 1 << 1,
    NULL_TRANSLATOR_PTR = 1 << 2
} AssemblerErr_t;


AssemblerErr_t AssamblerCtor(translator_s* translator);
AssemblerErr_t AssamblerExe(translator_s* translator);
AssemblerErr_t AssamblerDtor(translator_s* translator);
void create_signature(int* code);

int label_asm(translator_s* translator);

void do_help(void);
void print_result_code(translator_s* translator);
int write_result_in_text_file(translator_s* translator);
//todo: struct hadder{signature, version, PC};
int main()
{
    translator_s translator = {};
    AssamblerCtor(&translator);

    AssamblerExe(&translator);
    translator.count_line = 0;
    translator.code_num = 2;
    AssamblerExe(&translator);

    create_signature(translator.codes);
    write_result_in_text_file(&translator);

    AssamblerDtor(&translator);

    return 0;
}


AssemblerErr_t AssamblerCtor(translator_s* translator)
{
    if (translator == NULL) return NULL_TRANSLATOR_PTR;

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

    free(command_text); command_text = NULL;

    int* command_codes = (int*) calloc(linenum * 2 + 1 /*SIGNATURE*/ + 1 /*version*/, sizeof(int));
    assert(command_codes != NULL);

    translator->linenum = linenum;
    translator->count_line = 0;
    translator->code_num = 2;
    translator->lines = command_lines;
    translator->codes = command_codes;
    for (int i = 0; i < MAX_LABELS_NUM; i++) {
        translator->labels[i] = 0;
    }

    return ASSEMBLER_OK;
}


AssemblerErr_t AssamblerDtor(translator_s* translator)
{
    if (translator == NULL) return NULL_TRANSLATOR_PTR;

    free(translator->lines); translator->lines = NULL;
    free(translator->codes); translator->codes = NULL;

    translator->linenum = 0;
    translator->count_line = 0;
    translator->code_num = 2;
    translator->lines = NULL;
    translator->codes = NULL;
    for (int i = 0; i < MAX_LABELS_NUM; i++) {
        translator->labels[i] = 0;
    }

    return ASSEMBLER_OK;
}


void create_signature(int* code)
{
    memcpy(code, heder::SIGNATURE, 4);
    code[1] = heder::VERSION;
}


AssemblerErr_t AssamblerExe(translator_s* translator)
{
    assert(translator != NULL);

    char command[MAX_COMMAND_LENGTH] = {};

    int* count = &(translator->code_num);
    int* codes = translator->codes;

    for (; translator->count_line < translator->linenum; translator->count_line++) {

        sscanf(translator->lines[translator->count_line].ptr, "%s", command);
        for (int i = 0; i <= COMMANDS_NUM; i++) {
            if (i == COMMANDS_NUM) {
                label_asm(translator);
                break;
            }
            else if (strcmp(command, commands[i].name) == 0) {
                codes[(*count)++] = commands[i].num;
                #ifdef DUMP
                    printf("%3d %s  ", *count, command); getchar();
                #endif
                commands[i].func_asm(translator);
                memset(command, 0, sizeof(command));
                break;
            }
        }
    }
    return ASSEMBLER_OK;
}

int label_asm(translator_s* translator)
{
    int label = 0;
    if (sscanf(translator->lines[translator->count_line].ptr, " :%d", &label) == 1) {
        translator->labels[label] = translator->code_num;
        #ifdef DUMP
            printf(":%d or count = %d\n", label, translator->code_num);
        #endif
        return 0;
    }
    else return 1;
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
