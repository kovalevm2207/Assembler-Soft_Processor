#include <stdio.h>
#include <cstring>
#include "color_print.h"

typedef struct cartoon_s
{
    int OX;
    int OX_SIZE;
    int OY;
    int OY_SIZE;
    int num;
} cartoon_s;


int MainArgAnalyze(int argc);
char* create_name(char* argv);


int main(int argc, char* argv[])
{
    if (MainArgAnalyze(argc) != 0) return 1;
    FILE* file_in = fopen(argv[1], "r");
    FILE* file_out = fopen(create_name(argv[1]), "w");

    cartoon_s cartoon = {};

    fprintf(file_out, "# программа, для вывода на экран файла %s\n", argv[1]);
    fprintf(file_out, ":1\n"
                      "PUSH 0\n"
                      "POPREG BX    # обнуляем счетчик\n"
                      "\n");
    char sym = fgetc(file_in);

    while(sym != EOF) {
        if(sym == '\n') {

            printf("\n");

            cartoon.OY++;
            if (cartoon.OX_SIZE == 0) cartoon.OX_SIZE = cartoon.OX;
            if (cartoon.OX_SIZE != cartoon.OX) {
                printf("Картинка имеет различную длину строк\n");
                return 1;
            }
            cartoon.OX = 0;
            sym = fgetc(file_in);
            if(sym == '\n') {
                if (cartoon.OY_SIZE == 0) cartoon.OY_SIZE = cartoon.OY;
                if (cartoon.OY != cartoon.OY_SIZE) {
                    printf("Ваша картинка имеет различное количество строк\n");
                    return 1;
                }
                cartoon.OY = 0;
                cartoon.num++;

                printf("\n");

                fprintf(file_out, "DRAW        # Выводим память\n"
                                  "PUSH 0\n"
                                  "POPREG BX    # обнуляем счетчик\n"
                                  "\n");
            }
            else if(sym == EOF) {
                if (cartoon.OY_SIZE == 0) cartoon.OY_SIZE = cartoon.OY;
                if (cartoon.OY != cartoon.OY_SIZE) {
                    printf("Ваша картинка имеет различное количество строк\n");
                    return 1;
                }
                cartoon.num++;

                printf("\n");

                fprintf(file_out, "DRAW        # Выводим память\n"
                                  "PUSH 0\n"
                                  "POPREG BX    # обнуляем счетчик\n"
                                  "\n");
                break;
            }
            else {
                cartoon.OX++;

                printf("%c ", sym);

                fprintf(file_out, " PUSH %d        # номер символа %c\n", sym, sym );
                fprintf(file_out, " POPM [BX]\n"
                                  " \n"
                                  " PUSHREG BX\n"
                                  " PUSH 2\n"
                                  " ADD            # увелчиваем счетчик\n"
                                  " POPREG BX\n"
                                  " \n");
            }
        }
        else {
            cartoon.OX++;

            printf("%c ", sym);

            fprintf(file_out, " PUSH %d        # %c\n", sym, sym );
            fprintf(file_out, " POPM [BX]\n"
                              " \n"
                              " PUSHREG BX\n"
                              " PUSH 2\n"
                              " ADD            # увелчиваем счетчик\n"
                              " POPREG BX\n"
                              " \n");
        }
        sym = fgetc(file_in);
    }

    fprintf(file_out, "PUSH 1\n"
                      "PUSH 1\n"
                      "JE :1\n"
                      "HLT");

    printf("OX_SIZE = %d\n"
           "OY_SIZE = %d\n"
           "cartoon num = %d\n", cartoon.OX_SIZE, cartoon.OY_SIZE, cartoon.num);

    fclose(file_in);
    fclose(file_out);

    return 0;
}

int MainArgAnalyze(int argc)
{
    if (argc != 2) {
        printf(CHANGE_ON RED TEXT_COLOR "You should write: ./assambler <file name from cur dir>\n" RESET);
        return 1;
    }
    else return 0;
}

char* create_name(char* argv)
{
    char* dot_pos = strchr(argv, '.');
    *(dot_pos + 1) = '\0';
    return strcat(argv, "asm");
}
