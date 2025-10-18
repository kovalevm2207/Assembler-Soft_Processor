//=============================================//
//                                             //
//                                             //
//         IF YOU CHANGE THE COMMANDS.H        //
//                                             //
//                 YOU MUST                    //
//                                             //
//            CHANGE THE VERSION               //
//                                             //
//=============================================//

#ifndef COMMANDS
#define COMMANDS

#include <stdio.h>
#include <math.h>
#include <cstdio>
#include <unistd.h>
#include <stdlib.h>
#include "my_stack.h"

const int VERSION = 12;
const char SIGNATURE[4] = "SVU";

const size_t CAPACITY = 20;
const int COMMANDS_NUM = 25;

typedef enum
{
    PUSH            = 0,
    ADD             = 1,
    MOD             = 2,
    SUB             = 3,
    DIV             = 4,
    OUT             = 5,
    MUL             = 6,
    POW             = 7,
    SQRT            = 8,
    RESET_STK       = 9,
    PUSHREG         = 10,
    POPREG          = 11,
    JB              = 12, // <
    JBE             = 13, // <=
    JA              = 14, // >
    JAE             = 15, // >=
    JE              = 16, // ==
    JNE             = 17, // !=
    CALL            = 18,
    RET             = 19,
    IN              = 20,
    PUSHM           = 21,
    POPM            = 22,
    DRAW            = 23,
    HLT             = 24,
    START           = 25,
    INVALID_COMMAND = 26,
    HELP            = 27
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

typedef struct command_s{
    void (*func_exe)(SPU* spu, const command_s* command);
    char name[16];
    int num;
    arg_t arg;
    int (*action_exe)(int a, int b);
} command_s;

//-----------------------------------------------------------PUSH
int nothing(int a, int b);
void push(SPU* spu, const command_s* commands);
void pushreg(SPU* spu, const command_s* command);
void pushm(SPU* spu, const command_s* command);
//-----------------------------------------------------------PUSH
//-----------------------------------------------------------POP
void popreg(SPU* spu, const command_s* command);
void popm(SPU* spu, const command_s* command);
//-----------------------------------------------------------POP
//-----------------------------------------------------------MATH
int add(int a, int b);
int sub(int a, int b);
int mod(int a, int b);
int my_div(int a, int b);
int mul(int a, int b);
void math(SPU* spu, const command_s* command);
void pow(SPU* spu, const command_s* command);
void my_sqrt(SPU* spu, const command_s* command);
//-----------------------------------------------------------MATH
//-----------------------------------------------------------JMPs
int jb(int a, int b);
int jbe(int a, int b);
int ja(int a, int b);
int jae(int a, int b);
int je(int a, int b);
int jne(int a, int b);
void jmp(SPU* spu, const command_s* command);
void call(SPU* spu, const command_s* command);
void ret(SPU* spu, const command_s* command);
//------------------------------------------------------------JMPs
//------------------------------------------------------------SYSTEM
void out(SPU* spu, const command_s* command);
void reset_stk(SPU* spu, const command_s* command);
void in(SPU* spu, const command_s* command);
void draw(SPU* spu, const command_s* command);
void hlt(SPU* spu, const command_s* command);
//------------------------------------------------------------SYSTEM

const command_s commands[COMMANDS_NUM] =
{
 {push, "PUSH", PUSH, NUM, nothing},
 {math, "ADD", ADD, EMP, add},
 {math, "MOD", MOD, EMP, mod},
 {math, "SUB", SUB, EMP, sub},
 {math, "DIV", DIV, EMP, my_div},
 {out, "OUT", OUT, EMP, nothing},
 {math, "MUL", MUL, EMP, mul},
 {pow, "POW", POW, NUM, nothing},
 {my_sqrt, "SQRT", SQRT, EMP, nothing},
 {reset_stk, "RESET_STK", RESET_STK, EMP, nothing},
 {pushreg, "PUSHREG", PUSHREG, REG, nothing},
 {popreg, "POPREG", POPREG, REG, nothing},
 {jmp, "JB", JB, LBL, jb},
 {jmp, "JBE", JBE, LBL, jb},
 {jmp, "JA", JA, LBL, ja},
 {jmp, "JAE", JAE, LBL, jae},
 {jmp, "JE", JE, LBL, je},
 {jmp, "JNE", JNE, LBL, jne},
 {call, "CALL", CALL, LBL, nothing},
 {ret, "RET", RET, EMP, nothing},
 {in, "IN", IN, EMP, nothing},
 {pushm, "PUSHM", PUSHM, REG, nothing},
 {popm, "POPM", POPM, REG, nothing},
 {draw, "DRAW", DRAW, EMP, nothing},
 {hlt, "HLT", HLT, EMP, nothing}
};

#endif // COMMANDS
