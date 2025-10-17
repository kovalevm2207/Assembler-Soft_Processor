#ifndef COMMANDS
#define COMMANDS

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <cstdio>
#include "my_stack.h"

const int VERSION = 12;
const char SIGNATURE[4] = "SVU";

typedef enum
{
    HELP            = 0,
    PUSH            = 1,      //=============================================//
    ADD             = 2,      //                                             //
    MOD             = 3,      //                                             //
    SUB             = 4,      //         IF YOU CHANGE THE COMMANDS.H        //
    DIV             = 5,      //                                             //
    OUT             = 6,      //                 YOU MUST                    //
    MUL             = 7,      //                                             //
    POW             = 8,      //            CHANGE THE VERSION               //
    SQRT            = 9,      //                                             //
    RESET_STK       = 10,     //=============================================//
    PUSHREG         = 11,
    POPREG          = 12,
    JMP             = 13,
    JB              = 14, // <
    JBE             = 15, // <=
    JA              = 16, // >
    JAE             = 17, // >=
    JE              = 18, // ==
    JNE             = 19, // !=
    CALL            = 20,
    RET             = 21,
    IN              = 22,
    PUSHM           = 23,
    POPM            = 24,
    DRAW            = 25,
    START           = 26,
    INVALID_COMMAND = 27,
    HLT             = 28
} command_t;

typedef enum
{
    AX = 0,
    BX = 1,
    CX = 2,
    DX = 3,
    SX = 4,
    RX = 5
} reg_t;

typedef struct SPU
{
    int* code;
    size_t PC;
    stack_s stk;
    int regs[5];
    size_t file_size;
    stack_s labels;
    int* RAM;
} SPU;


typedef enum {
    EMP,
    NUM,
    REG,
    LBL
} arg_t;


typedef struct {
    char name[16];
    int num;
    arg_t arg;
} command_s;


const size_t CAPACITY = 20;

void push(SPU* spu);
void add(SPU* spu);
void sub(SPU* spu);
void mod(SPU* spu);
void div(SPU* spu);
void out(SPU* spu);
void mul(SPU* spu);
void pow(SPU* spu);
void my_sqrt(SPU* spu);
void reset_stk(SPU* spu);
void pushreg(SPU* spu);
void popreg(SPU* spu);
void call(SPU* spu);
void ret(SPU* spu);
void jmp(SPU* spu);
void jb(SPU* spu);
void jbe(SPU* spu);
void ja(SPU* spu);
void jae(SPU* spu);
void je(SPU* spu);
void jne(SPU* spu);
void in(SPU* spu);
void popm(SPU* spu);
void pushm(SPU* spu);
void draw(SPU* spu);
void hlt(void);


#endif // COMMANDS
