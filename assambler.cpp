#include <stdio.h>
#include <stdlib.h>    // для calloc, malloc, free
#include <string.h>    // для работы со строками
#include <fcntl.h>     // для O_RDONLY и других флагов
#include <unistd.h>    // для open, read, close
#include <sys/stat.h>
#include <assert.h>

#include "commands.h"
#include "color_print.h"

struct retranslator_s {
    int count;
    unsigned char* buffer;
    command_t command;
    FILE* file_name;
};

command_t find_command(retranslator_s* retranslator);
void do_command(retranslator_s* retranslator);
void do_help(void);
void do_push(retranslator_s* retranslator);
void do_pow(retranslator_s* retranslator);
size_t find_file_size(const char* file_name);

int main()
{
    size_t file_size0 = find_file_size("OneginText1251.txt");

    char* buffer = (char*) calloc(file_size0 + 1, sizeof(char));
    assert(buffer != NULL);

    int text_command = open("source.asm", O_RDONLY);
    assert(text_command != 0);

    size_t file_size = read (text_command, buffer, file_size0 + 1);
    assert(file_size != 0);
    buffer[file_size] = '\0';

    close(text_command);

    unsigned char command_codes[1000] = {};
    retranslator_s retranslator = {
        .count = 0,
        .buffer = buffer,
        .command = START,
        .file_name = file
    };

    while (retranslator.command != END_PROGRAM) {
        retranslator.command = find_command(&retranslator);
        do_command(&retranslator);
    };

    printf("Result codes: ");
    for (int i = 0; i < retranslator.count; i++) {
        printf("%d ", buffer[i]);
    }
    printf("\n");

    return 0;
}

size_t find_file_size(const char* file_name)
{
    assert (file_name != 0);

    struct stat file_info = {0};
    stat(file_name, &file_info);

    assert(file_info.st_size != 0);

    return file_info.st_size;
}


command_t find_command(retranslator_s* retranslator)
{
    char command[20] = {};
    fscanf(retranslator->file_name, "%s", command);
    if (strcmp(command,  "HELP") == 0) return HELP;
    if (strcmp(command,  "PUSH") == 0) return PUSH;
    if (strcmp(command,   "ADD") == 0) return ADD;
    if (strcmp(command,   "SUB") == 0) return SUB;
    if (strcmp(command,   "DIV") == 0) return DIV;
    if (strcmp(command,   "OUT") == 0) return OUT;
    if (strcmp(command,   "MUL") == 0) return MUL;
    if (strcmp(command,   "POW") == 0) return POW;
    if (strcmp(command,  "SQRT") == 0) return SQRT;
    if (strcmp(command,   "HLT") == 0) return END_PROGRAM;
    if (strcmp(command, "RESET") == 0) return RESET_STK;
    else return INVALID_COMMAND;
}


void do_command(retranslator_s* retranslator)
{
    assert(retranslator != NULL);

    unsigned char* buffer = retranslator->buffer;
    int count = retranslator->count;
    command_t command = retranslator->command;

    if (command == END_PROGRAM) printf("You end the calc program\n");
    if (command == HELP)      do_help();
    if (command == PUSH)      do_push(retranslator);
    if (command == ADD)       buffer[count++] = 2;
    if (command == SUB)       buffer[count++] = 3;
    if (command == DIV)       buffer[count++] = 4;
    if (command == OUT)       buffer[count++] = 5;
    if (command == MUL)       buffer[count++] = 6;
    if (command == POW)       do_pow(retranslator);
    if (command == SQRT)      buffer[count++] = 8;
    if (command == RESET_STK) buffer[count++] = 9;
    if (command == INVALID_COMMAND) {
        printf(CHANGE_ON RED TEXT_COLOR "INVALID_COMMAND, "
               CHANGE_ON RED TEXT_COLOR "please read help:\n" RESET);
        do_help();
    }

    retranslator->count = count;
}


void do_push(retranslator_s* retranslator)
{
    assert(retranslator != NULL);

    retranslator->buffer[retranslator->count++] = 1;
    int data = 0;
    fscanf(retranslator->file_name, "%d", &data);
    retranslator->buffer[retranslator->count++] = data;
}


void do_pow(retranslator_s* retranslator)
{
    assert(retranslator != NULL);

    retranslator->buffer[retranslator->count++] = 7;
    int n = 0;
    fscanf(retranslator->file_name, "%d", &n);
    retranslator->buffer[retranslator->count++] = n;
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
