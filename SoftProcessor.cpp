#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <cstring>

#include "commands.h"
#include "my_stack.h"

const size_t CAPACITY = 15;

typedef enum
{
    SUCCESSFUL_EXECUTION = 1 << 1,
    COMMAND_NOT_FOUND    = 1 << 2,
    PROGRAM_END_MISSING  = 1 << 3
} DoCodeErr_t;

typedef enum
{
    CORRECT_SIGNATURE = 1 << 1,
    BAD_PASSWORD      = 1 << 2,
    BAD_VERSION       = 1 << 3
} SignatureErr_t;

typedef struct SPU
{
    char* code;
    size_t PC;
    stack_s stk;
    int* regs;
}SPU;

size_t find_file_size(const char* file_name);
void print(char* code);
SignatureErr_t check_signature(char* code);
DoCodeErr_t do_code(SPU* spu);

int main()
{
    size_t file_size0 = find_file_size("code_bin.asm");

    char* code = (char*) calloc(file_size0 + 1, sizeof(char));
    assert(code != NULL);

    int file = open("code_bin.asm", O_RDONLY);
    assert(file != 0);

    size_t file_size = read (file, code, file_size0 + 1);
    assert(file_size != 0);
    code[file_size] = '\0';

    close(file);

    stack_s stk = {};
    StackCtor(&stk, CAPACITY);
    int regs[4] = {};

    SPU spu = {
        .code = code,
        .PC = 7,
        .stk = stk,
        .regs = regs
    };

    print(code);

    int sign_err = check_signature(code);
    if (sign_err != CORRECT_SIGNATURE) {
        printf("SIGNATURE ERROR\n");
        return 1;
    }

    if(do_code(&spu) != SUCCESSFUL_EXECUTION) {
        return 1;
    }

    StackDtor(&spu.stk);

    free(code);

    return 0;
}


size_t find_file_size(const char* file_name)
{
    assert (file_name != NULL);

    struct stat file_info = {};
    stat(file_name, &file_info);

    assert(file_info.st_size != 0);

    return file_info.st_size;
}


void print(char* code)
{
    assert(code != NULL);

    int i = 0;
    do {
        printf("%2.d ", i);
    } while(code[i++] != '\0');

    printf("\n");

    i = 0;

    do {
        printf("%2.d ", code[i]);
    } while(code[i++] != '\0');

    printf("\n");
}


SignatureErr_t check_signature(char* code)
{
    assert(code != NULL);

    int sign_err = CORRECT_SIGNATURE;

    if (code[6] != VERSION)              sign_err |= BAD_VERSION;
    if (strncmp(code, PASSWORD, 6) != 0) sign_err |= BAD_PASSWORD;

    return (SignatureErr_t) sign_err;
}


DoCodeErr_t do_code(SPU* spu)
{
    assert(spu != NULL);

    char* code = spu->code;
    size_t* PC = &spu->PC;
    stack_s* stk = &spu->stk;
    int* regs = spu->regs;

    while (code[*PC] != '\0') {
        switch(code[*PC]) {
            case PUSH: {
                printf("PUSH(PC = %zu) -> ", (*PC)++);

                stack_t data = code[*PC];

                printf("data(PC = %zu) = %d\n", *PC, data);
                getchar();

                StackPush(stk, data);

                StackDump(stk);
                getchar();

                break;
            }
            case ADD: {
                stack_t a = 0, b = 0;
                StackPop(stk, &a);
                StackPop(stk, &b);

                printf("ADD(PC = %zu) -> %d + %d\n ", *PC, a, b);
                getchar();

                stack_t c = a + b;
                StackPush(stk, c);

                StackDump(stk);
                getchar();

                break;
            }
            case SUB: {
                stack_t a = 0, b = 0;
                StackPop(stk, &a);
                StackPop(stk, &b);

                printf("SUB(PC = %zu) -> %d - %d\n", *PC, b, a);
                getchar();

                stack_t c = b - a;
                StackPush(stk, c);

                StackDump(stk);
                getchar();

                break;
            }
            case DIV: {
                stack_t a = 0, b = 0;
                StackPop(stk, &a);
                StackPop(stk, &b);

                printf("DIV(PC = %zu) -> %d / %d", *PC, b, a);
                getchar();

                stack_t c = b / a;
                StackPush(stk, c);

                StackDump(stk);
                getchar();

                break;
            }
            case OUT: {
                stack_t data = 0;
                StackPop(stk, &data);

                printf("OUT(PC = %zu) -> %d\n", *PC, data);;
                getchar();

                StackDump(stk);
                getchar();


                break;
            }
            case MUL: {
                stack_t a = 0, b = 0;
                StackPop(stk, &a);
                StackPop(stk, &b);

                printf("MUL(PC = %zu) -> %d * %d\n", *PC, a, b);
                getchar();

                stack_t c = a * b;
                StackPush(stk, c);

                StackDump(stk);
                getchar();

                break;
            }
            case POW: {
                printf("POW(PC = %zu) -> ", (*PC)++);

                stack_t a = 0, n = code[*PC];
                StackPop(stk, &a);

                printf("%d ^ n(PC = %zu) = %d\n", a, *PC, n);
                getchar();

                stack_t c = pow(a,n);
                StackPush(stk, c);

                StackDump(stk);
                getchar();

                break;
            }
            case SQRT: {
                stack_t a = 0;
                StackPop(stk, &a);

                printf("SQRT(PC = %zu) -> sqrt(%d)\n", *PC, a);
                getchar();

                stack_t b = sqrt(a);
                StackPush(stk, b);

                StackDump(stk);
                getchar();

                break;
            }
            case RESET_STK: {
                printf("RESET_STK(PC = %zu)\n", *PC);
                getchar();

                StackDtor(stk);
                StackCtor(stk, CAPACITY);

                StackDump(stk);
                getchar();

                break;
            }
            case HLT:

                printf("HLT(PC = %zu)\n", *PC);
                getchar();

                return SUCCESSFUL_EXECUTION;
            default:
                return COMMAND_NOT_FOUND;
        }
        (*PC)++;
    }
    return PROGRAM_END_MISSING;
}
