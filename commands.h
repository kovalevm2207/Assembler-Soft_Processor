#ifndef COMMANDS
#define COMMANDS

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <cstdio>
#include "my_stack.h"

const int VERSION = 12;
const char SIGNATURE[4] = "SVU";

const size_t CAPACITY = 20;
const int COMMANDS_NUM = 26;

typedef enum
{
    PUSH            = 0,      //=============================================//
    ADD             = 1,      //                                             //
    MOD             = 2,      //                                             //
    SUB             = 3,      //         IF YOU CHANGE THE COMMANDS.H        //
    DIV             = 4,      //                                             //
    OUT             = 5,      //                 YOU MUST                    //
    MUL             = 6,      //                                             //
    POW             = 7,      //            CHANGE THE VERSION               //
    SQRT            = 8,      //                                             //
    RESET_STK       = 9,      //=============================================//
    PUSHREG         = 10,
    POPREG          = 11,
    JMP             = 12,
    JB              = 13, // <
    JBE             = 14, // <=
    JA              = 15, // >
    JAE             = 16, // >=
    JE              = 17, // ==
    JNE             = 18, // !=
    CALL            = 19,
    RET             = 20,
    IN              = 21,
    PUSHM           = 22,
    POPM            = 23,
    DRAW            = 24,
    HLT             = 25,
    START           = 26,
    INVALID_COMMAND = 27,
    HELP            = 28
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


typedef enum
{
    NO = 0,
    PLUS = 1,
    PERCENT = 2,
    MINUS = 3,
    SLASH = 4,
    STAR = 5,
    B = 1,
    BE =2,
    A = 3,
    AE = 4,
    E = 5,
    NE = 6
} sign_t;


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
    void (*func_exe)(SPU* spu);
    char name[16];
    int num;
    arg_t arg;
    sign_t sign;
} command_s;



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
void hlt(SPU* spu);


const command_s commands[COMMANDS_NUM] =
    {
     {push, "PUSH", PUSH, NUM, NO},
     {add, "ADD", ADD, EMP, PLUS},
     {mod, "MOD", MOD, EMP, PERCENT},
     {sub, "SUB", SUB, EMP, MINUS},
     {div, "DIV", DIV, EMP, SLASH},
     {out, "OUT", OUT, EMP, NO},
     {mul, "MUL", MUL, EMP, STAR},
     {pow, "POW", POW, NUM, NO},
     {my_sqrt, "SQRT", SQRT, EMP, NO},
     {reset_stk, "RESET_STK", RESET_STK, EMP, NO},
     {pushreg, "PUSHREG", PUSHREG, REG, NO},
     {popreg, "POPREG", POPREG, REG, NO},
     {jmp, "JMP", JMP, LBL, NO},
     {jb, "JB", JB, LBL, B},
     {jbe, "JBE", JBE, LBL, BE},
     {ja, "JA", JA, LBL, A},
     {jae, "JAE", JAE, LBL, AE},
     {je, "JE", JE, LBL, E},
     {jne, "JNE", JNE, LBL, NE},
     {call, "CALL", CALL, LBL, NO},
     {ret, "RET", RET, EMP, NO},
     {in, "IN", IN, EMP, NO},
     {pushm, "PUSHM", PUSHM, REG, NO},
     {popm, "POPM", POPM, REG, NO},
     {draw, "DRAW", DRAW, EMP, NO},
     {hlt, "HLT", HLT, EMP, NO}
    };

#endif // COMMANDS
