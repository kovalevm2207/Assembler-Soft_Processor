#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#include "commands.h"
#include "color_print.h"
#include "ReadFile.h"
#include "my_stack.h"

typedef enum
{
    ASSEMBLER_OK         = 1 << 1,
    NULL_TRANSLATOR_PTR  = 1 << 2,
    MAIN_ARG_NUM_ERR     = 1 << 3,
    NULL_TEXT_PTR        = 1 << 4,
    NULL_CODE_PTR        = 1 << 5,
    COUNT_LINES_ERR      = 1 << 6,
    INVALID_COMMAND_ERR  = 1 << 7,
    DIFFERENT_HASHES_ERR = 1 << 8,
    MEM_ALLOC_ERR        = 1 << 9,
    HAHS_CHECK_ERR       = 1 << 10
} AssemblerErr_t;


AssemblerErr_t MainArgAnalyze(int argc);
char* ReadProgram(char* argv);

AssemblerErr_t AssamblerCtor(translator_s* translator, char* command_text);
AssemblerErr_t HashsCtor(long int* hashs);
AssemblerErr_t check_hashs(long int* hashs);

AssemblerErr_t AssamblerExe(translator_s* translator);
long int hashing(char* command);

int write_result_in_text_file(translator_s* translator, char* argv);
AssemblerErr_t create_signature(int* code);
char* create_name(char* argv);
void print_result_code(translator_s* translator);

AssemblerErr_t AssamblerDtor(translator_s* translator);

int main(int argc, char* argv[])
{
    if (MainArgAnalyze(argc) == MAIN_ARG_NUM_ERR) return 1;
    char* command_text = ReadProgram(argv[1]);
    if(command_text == NULL) return 1;

    translator_s translator = {};
    if (AssamblerCtor(&translator, command_text) != ASSEMBLER_OK) {AssamblerDtor(&translator); return 1;}

    if(AssamblerExe(&translator) != ASSEMBLER_OK) {AssamblerDtor(&translator); return 1;}
    translator.count_line = 0;
    translator.code_num = 2;
    AssamblerExe(&translator);

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

    int STATUS = ASSEMBLER_OK;
    long int* hashs = (long int*) calloc(COMMANDS_NUM, sizeof(long int));
    if (hashs == NULL) {STATUS |= MEM_ALLOC_ERR; return (AssemblerErr_t) STATUS;}

    STATUS = HashsCtor(hashs);
    if (STATUS != ASSEMBLER_OK) {free(hashs); return (AssemblerErr_t) STATUS;}

    size_t linenum = count_lines(command_text);
    if (linenum <= 0) {free(hashs); STATUS |= COUNT_LINES_ERR; return (AssemblerErr_t) STATUS;}

    line *command_lines = (line*) calloc(linenum, sizeof(line));
    if(command_lines == NULL) {free(hashs); STATUS |= MEM_ALLOC_ERR; return (AssemblerErr_t) STATUS;}        //define PROVE_POINTER mb no KoV (пока не делаем)

    get_lines(command_lines, command_text);

    free(command_text); command_text = NULL;

    int* command_codes = (int*) calloc(linenum * 2 + 1 /*SIGNATURE*/ + 1 /*version*/, sizeof(int));
    if(command_codes == NULL) {free(hashs); free(command_lines); STATUS |= MEM_ALLOC_ERR; return (AssemblerErr_t) STATUS;}

    translator->linenum = linenum;
    translator->count_line = 0;
    translator->code_num = 2;
    translator->lines = command_lines;
    translator->codes = command_codes;
    for (int i = 0; i < MAX_LABELS_NUM; i++) {
        translator->labels[i] = 0;
    }
    translator->hashs = hashs;

    return (AssemblerErr_t) STATUS;
}


AssemblerErr_t HashsCtor(long int* hashs)
{
    for (int i = 0; i < COMMANDS_NUM; i++)
    {
        long int hash = 0;
        int letter = 0;
        while(commands[i].name[letter] != '\0') {
            hash = hash * 31 + commands[i].name[letter++];
        }
        hashs[i] = hash;
        #ifdef DUMP
            printf("command [%10s] (with number [%2d])  ---has-hash---> %ld\n", commands[i].name, commands[i].num, hash);
        #endif
    }

    return check_hashs(hashs);
}

AssemblerErr_t check_hashs(long int* hashs)
{
    int STATUS = ASSEMBLER_OK;

    for (int i = 0; i < COMMANDS_NUM; i++)
    {
        for(int j = i + 1; j < COMMANDS_NUM; j++)
        {
            if (hashs[i] == hashs[j])
            {
                printf(CHANGE_ON RED TEXT_COLOR "You have equal hashes for commands:\n"
                                                "%10s ---> hash = %ld\n"
                                                "%10s ---> hash = %ld\n" RESET,
                                                commands[i].name, hashs[i],
                                                commands[j].name, hashs[j]);
                STATUS = HAHS_CHECK_ERR;
            }
        }
    }

    return (AssemblerErr_t) STATUS;
}


AssemblerErr_t AssamblerExe(translator_s* translator)
{
    assert(translator != NULL);

    char command[MAX_COMMAND_LENGTH] = {};

    size_t* count = &(translator->code_num);
    int* codes = translator->codes;

    for (; translator->count_line < translator->linenum; translator->count_line++) {
        sscanf(translator->lines[translator->count_line].ptr, "%s", command);                   //max size command KoV
                                                            //  ^^------------------------------------- тяжело подобрать значение
        long int com_hash = hashing(command);                                                         // падает sanitizer при 20

        for (int i = 0; i <= COMMANDS_NUM; i++) {                                               //size_t KoV
            if (i == COMMANDS_NUM) {                                                               // (COMMANDS_NUM has type int, not size_t) мика
                if (lbl_asm(translator) != CMD_OK)
                {
                    printf(CHANGE_ON RED TEXT_COLOR "!!!ERROR!!! INVALID COMMAND ERR\n" RESET);
                    return INVALID_COMMAND_ERR;
                }
                break;
            }
            else if (translator->hashs[i] == com_hash) {
                codes[(*count)++] = commands[i].num;
                #ifdef DUMP
                    printf("%3zu %s  ", *count, command); getchar();                 //DEBUG_ON (...) __VA__ARGS__ KoV (Это наверное придется менять тогда,)
                #endif                                                                                     // появится AssemblerDump)
                if(commands[i].func_asm(translator) != CMD_OK) return INVALID_COMMAND_ERR;
                memset(command, 0, sizeof(command));
                break;
            }
        }
    }
    return ASSEMBLER_OK;
}


long int hashing(char* command)
{
    long int hash = 0;
    long int i = 0;
    while(command[i] != '\0') {
        hash = hash * 31 + command[i++];
    }
    return hash;
}


int write_result_in_text_file(translator_s* translator, char* argv)
{
    create_signature(translator->codes);

    translator->codes[translator->code_num++] = '\0';
    #ifdef DUMP
        print_result_code(translator);
    #endif

    char* CREATE_FILE = create_name(argv);
    FILE* file = fopen(CREATE_FILE, "wb");

    size_t code_size = translator->code_num * sizeof(int);

    fwrite(&code_size, sizeof(size_t), 1, file);
    fwrite(translator->codes, sizeof(int), translator->code_num, file);

    fclose(file);
    return 0;
}


AssemblerErr_t create_signature(int* code)
{
    if (code == NULL) return NULL_CODE_PTR;
    memcpy(code, heder::SIGNATURE, 4);
    code[1] = heder::VERSION;
    return ASSEMBLER_OK;
}


char* create_name(char* argv)
{
    // меняем директорию
    char* slash_pos = strchr(argv, '/');
    strcpy(argv, "bin");
    *slash_pos = '/';
    // меняем расширение файла
    *(strchr(argv, '.') + 1) = '\0';
    return strcat(argv, "bin");
}


void print_result_code(translator_s* translator)
{
    printf("Result codes: ");
        for (size_t i = 0; i < translator->code_num; i++) {   // size_t
            printf("%d ", translator->codes[i]);
        }
        printf("\n");
}


AssemblerErr_t AssamblerDtor(translator_s* translator)
{
    if (translator == NULL) return NULL_TRANSLATOR_PTR;

    free(translator->lines); translator->lines = NULL;
    free(translator->codes); translator->codes = NULL;
    free(translator->hashs); translator->hashs = NULL;

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
