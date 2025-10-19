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

typedef enum
{
    ASSEMBLER_OK        = 1 << 1,
    NULL_TRANSLATOR_PTR = 1 << 2,
    MAIN_ARG_NUM_ERR    = 1 << 3,
    NULL_TEXT_PTR       = 1 << 4,
    NULL_CODE_PTR       = 1 << 5
} AssemblerErr_t;


AssemblerErr_t MainArgAnalyze(int argc);
char* ReadProgram(char* argv);
AssemblerErr_t AssamblerCtor(translator_s* translator, char* command_text);
AssemblerErr_t AssamblerExe(translator_s* translator);
AssemblerErr_t AssamblerDtor(translator_s* translator);
AssemblerErr_t create_signature(int* code);
int label_asm(translator_s* translator);
void print_result_code(translator_s* translator);
int write_result_in_text_file(translator_s* translator, char* argv);
char* create_name(char* argv);

int main(int argc, char* argv[])
{
    if (MainArgAnalyze(argc) == MAIN_ARG_NUM_ERR) return 1;
    char* command_text = ReadProgram(argv[1]);
    if(command_text == NULL) return 1;

    translator_s translator = {};
    if (AssamblerCtor(&translator, command_text) != ASSEMBLER_OK) return 1;

    AssamblerExe(&translator);
    translator.count_line = 0;
    translator.code_num = 2;
    AssamblerExe(&translator);

    if (create_signature(translator.codes) != ASSEMBLER_OK) return 1;
    write_result_in_text_file(&translator, argv[1]);

    AssamblerDtor(&translator);

    return 0;
}


AssemblerErr_t MainArgAnalyze(int argc)
{
    if (argc != 2) {
        printf(CHANGE_ON RED TEXT_COLOR "You should write: ./assambler <file name from cur dir>\n" RESET);
        return MAIN_ARG_NUM_ERR;
    }
    else return ASSEMBLER_OK;
}


char* ReadProgram(char* argv)
{
    assert(argv != NULL);

    size_t file_size0 = find_file_size(argv);

    char* command_text = (char*) calloc(file_size0 + 1, sizeof(char));
    assert(command_text != NULL);

    int file = open(argv, O_RDONLY);
    assert(file != -1);

    size_t file_size = read (file, command_text, file_size0 + 1);
    assert(file_size != 0);
    command_text[file_size] = '\0';

    close(file);

    return command_text;
}


AssemblerErr_t AssamblerCtor(translator_s* translator, char* command_text)
{
    if (translator == NULL) return NULL_TRANSLATOR_PTR;
    if (command_text == NULL) return NULL_TEXT_PTR;

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


AssemblerErr_t create_signature(int* code)
{
    if (code == NULL) return NULL_CODE_PTR;
    memcpy(code, heder::SIGNATURE, 4);
    code[1] = heder::VERSION;
    return ASSEMBLER_OK;
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


int write_result_in_text_file(translator_s* translator, char* argv)
{
    translator->codes[translator->code_num++] = '\0';
    #ifdef DUMP
        print_result_code(translator);
    #endif

    char* CREATE_FILE = create_name(argv);
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


char* create_name(char* argv)
{
    char* dot_pos = strchr(argv, '.');
    *(dot_pos + 1) = '\0';
    return strcat(argv, "bin");
}
